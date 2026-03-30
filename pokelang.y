%{
/*
 * PokemonLang Parser (Bison)
 * Enhanced with semantic analysis, basic runtime behavior, and IR emission.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int line_number;

void yyerror(const char* s);

int syntax_error_count = 0;
int semantic_error_count = 0;
int semantic_warning_count = 0;
int indentation_error_count = 0;
int explain_trace_mode = 0;

#define MAX_SYMBOLS 1024
#define MAX_FUNCTIONS 256
#define MAX_NAME_LEN 63

#ifndef POKELANG_TYPES_DEFINED
#define POKELANG_TYPES_DEFINED
typedef enum {
    TYPE_UNKNOWN = 0,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_LONG,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_STRING
} DataType;

typedef struct {
    double num;
    DataType type;
    int is_lvalue;
    char ident[64];
    int is_string;
    char str[256];
} ExprValue;
#endif

typedef struct {
    char name[MAX_NAME_LEN + 1];
    DataType type;
    double numeric_value;
    char string_value[256];
    int initialized;
    int is_array;
} Symbol;

typedef struct {
    char name[MAX_NAME_LEN + 1];
    DataType return_type;
    int param_count;
} FunctionInfo;

Symbol symbols[MAX_SYMBOLS];
int symbol_count = 0;

FunctionInfo functions[MAX_FUNCTIONS];
int function_count = 0;

DataType current_decl_type = TYPE_UNKNOWN;
DataType current_function_return_type = TYPE_VOID;
char current_function_name[MAX_NAME_LEN + 1] = "<global>";

FILE* ir_out = NULL;

int collect_print_args = 0;
char print_buffer[1024];

void trace_event(const char* stage, const char* fmt, ...) {
    va_list args;
    if (!explain_trace_mode) return;
    printf("[TRACE][%s] ", stage);
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void trace_reduce(const char* rule) {
    trace_event("REDUCE", "%s", rule);
}

void trace_semantic(const char* fmt, ...) {
    va_list args;
    if (!explain_trace_mode) return;
    printf("[TRACE][SEM] ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

const char* dtype_name(DataType t) {
    switch (t) {
        case TYPE_INT: return "Pikachu";
        case TYPE_FLOAT: return "Charizard";
        case TYPE_DOUBLE: return "Mewtwo";
        case TYPE_LONG: return "Snorlax";
        case TYPE_CHAR: return "Eevee";
        case TYPE_BOOL: return "Togepi";
        case TYPE_VOID: return "EmptyBall";
        case TYPE_STRING: return "String";
        default: return "Unknown";
    }
}

void emit_ir(const char* fmt, ...) {
    va_list args;
    char line[512];
    if (!ir_out) return;
    va_start(args, fmt);
    vsnprintf(line, sizeof(line), fmt, args);
    va_end(args);
    fprintf(ir_out, "%s\n", line);
    if (explain_trace_mode) {
        printf("[TRACE][IR] %s\n", line);
    }
}

void semantic_error(const char* fmt, ...) {
    va_list args;
    fprintf(stderr, "Semantic error at line %d: ", line_number);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    semantic_error_count++;
}

void semantic_warning(const char* fmt, ...) {
    va_list args;
    fprintf(stderr, "Semantic warning at line %d: ", line_number);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    semantic_warning_count++;
}

int is_numeric_type(DataType t) {
    return t == TYPE_INT || t == TYPE_FLOAT || t == TYPE_DOUBLE || t == TYPE_LONG || t == TYPE_CHAR || t == TYPE_BOOL;
}

DataType promote_numeric_type(DataType a, DataType b) {
    if (a == TYPE_DOUBLE || b == TYPE_DOUBLE) return TYPE_DOUBLE;
    if (a == TYPE_FLOAT || b == TYPE_FLOAT) return TYPE_FLOAT;
    if (a == TYPE_LONG || b == TYPE_LONG) return TYPE_LONG;
    return TYPE_INT;
}

int can_assign(DataType to, DataType from) {
    if (to == from) return 1;
    if (to == TYPE_VOID) return 0;
    if (to == TYPE_STRING || from == TYPE_STRING) return 0;
    if (is_numeric_type(to) && is_numeric_type(from)) return 1;
    return 0;
}

double coerce_numeric(DataType to, double value) {
    if (to == TYPE_BOOL) return value != 0.0 ? 1.0 : 0.0;
    if (to == TYPE_CHAR) return (double)((char)value);
    if (to == TYPE_INT) return (double)((int)value);
    if (to == TYPE_LONG) return (double)((long)value);
    return value;
}

ExprValue make_number(double n, DataType t) {
    ExprValue v;
    memset(&v, 0, sizeof(v));
    v.num = n;
    v.type = t;
    v.is_lvalue = 0;
    v.is_string = 0;
    return v;
}

ExprValue make_string(const char* s) {
    ExprValue v;
    memset(&v, 0, sizeof(v));
    v.type = TYPE_STRING;
    v.is_string = 1;
    snprintf(v.str, sizeof(v.str), "%s", s ? s : "");
    return v;
}

int find_symbol(const char* name) {
    int i;
    for (i = symbol_count - 1; i >= 0; --i) {
        if (strcmp(symbols[i].name, name) == 0) return i;
    }
    return -1;
}

int declare_symbol(const char* name, DataType type, int is_array) {
    int idx = find_symbol(name);
    if (idx >= 0) {
        semantic_error("redeclaration of variable '%s'", name);
        return idx;
    }

    if (symbol_count >= MAX_SYMBOLS) {
        semantic_error("symbol table overflow");
        return -1;
    }

    idx = symbol_count++;
    snprintf(symbols[idx].name, sizeof(symbols[idx].name), "%s", name);
    symbols[idx].type = type;
    symbols[idx].numeric_value = 0.0;
    symbols[idx].string_value[0] = '\0';
    symbols[idx].initialized = 0;
    symbols[idx].is_array = is_array;

    trace_semantic("declare symbol name=%s type=%s array=%d", name, dtype_name(type), is_array);

    emit_ir("DECL %s %s", dtype_name(type), name);
    return idx;
}

ExprValue read_symbol(const char* name) {
    int idx = find_symbol(name);
    ExprValue v;
    memset(&v, 0, sizeof(v));

    if (idx < 0) {
        semantic_error("use of undeclared variable '%s'", name);
        return make_number(0.0, TYPE_UNKNOWN);
    }

    if (!symbols[idx].initialized && !symbols[idx].is_array) {
        semantic_warning("variable '%s' used before initialization", name);
    }

    v.type = symbols[idx].type;
    v.num = symbols[idx].numeric_value;
    v.is_lvalue = 1;
    snprintf(v.ident, sizeof(v.ident), "%s", name);

    if (symbols[idx].type == TYPE_STRING) {
        v.is_string = 1;
        snprintf(v.str, sizeof(v.str), "%s", symbols[idx].string_value);
    }

    return v;
}

void assign_symbol(const char* name, ExprValue rhs) {
    int idx = find_symbol(name);
    if (idx < 0) {
        semantic_error("assignment to undeclared variable '%s'", name);
        return;
    }

    if (!can_assign(symbols[idx].type, rhs.type)) {
        semantic_error(
            "cannot assign value of type %s to variable '%s' of type %s",
            dtype_name(rhs.type),
            name,
            dtype_name(symbols[idx].type)
        );
        return;
    }

    if (rhs.type != symbols[idx].type && is_numeric_type(rhs.type) && is_numeric_type(symbols[idx].type)) {
        semantic_warning(
            "implicit conversion in assignment to '%s' from %s to %s",
            name,
            dtype_name(rhs.type),
            dtype_name(symbols[idx].type)
        );
    }

    symbols[idx].numeric_value = coerce_numeric(symbols[idx].type, rhs.num);
    symbols[idx].initialized = 1;

    trace_semantic("assign %s <= %.6f (%s)", name, rhs.num, dtype_name(rhs.type));

    emit_ir("%s = %.6f", name, symbols[idx].numeric_value);
}

int find_function(const char* name) {
    int i;
    for (i = 0; i < function_count; ++i) {
        if (strcmp(functions[i].name, name) == 0) return i;
    }
    return -1;
}

void register_function(const char* name, DataType return_type, int param_count) {
    int idx = find_function(name);
    if (idx >= 0) {
        semantic_error("redeclaration of function '%s'", name);
        return;
    }

    if (function_count >= MAX_FUNCTIONS) {
        semantic_error("function table overflow");
        return;
    }

    idx = function_count++;
    snprintf(functions[idx].name, sizeof(functions[idx].name), "%s", name);
    functions[idx].return_type = return_type;
    functions[idx].param_count = param_count;

    trace_semantic("register function name=%s return=%s params=%d", name, dtype_name(return_type), param_count);

    emit_ir("FUNC %s %s(%d)", dtype_name(return_type), name, param_count);
}

ExprValue evaluate_function_call(const char* name, int arg_count) {
    int idx = find_function(name);
    if (idx < 0) {
        semantic_error("call to undeclared function '%s'", name);
        return make_number(0.0, TYPE_UNKNOWN);
    }

    if (functions[idx].param_count != arg_count) {
        semantic_error(
            "function '%s' expects %d argument(s), but got %d",
            name,
            functions[idx].param_count,
            arg_count
        );
    }

    trace_semantic("call function name=%s args=%d", name, arg_count);

    /* Placeholder runtime value for user-defined functions. */
    return make_number(0.0, functions[idx].return_type);
}

void enter_function_context(const char* name, DataType return_type) {
    snprintf(current_function_name, sizeof(current_function_name), "%s", name);
    current_function_return_type = return_type;
    symbol_count = 0; /* Simplified per-function scope. */
    trace_semantic("enter function context name=%s return=%s", name, dtype_name(return_type));
    emit_ir("ENTER %s", current_function_name);
}

void leave_function_context(void) {
    trace_semantic("leave function context name=%s", current_function_name);
    emit_ir("LEAVE %s", current_function_name);
    snprintf(current_function_name, sizeof(current_function_name), "%s", "<global>");
    current_function_return_type = TYPE_VOID;
    symbol_count = 0;
}

ExprValue ensure_numeric(ExprValue v, const char* op_name) {
    if (!is_numeric_type(v.type)) {
        semantic_error("operator '%s' requires numeric operands", op_name);
        return make_number(0.0, TYPE_UNKNOWN);
    }
    return v;
}

ExprValue eval_binary_numeric(ExprValue a, ExprValue b, const char* op_name, char op) {
    ExprValue left = ensure_numeric(a, op_name);
    ExprValue right = ensure_numeric(b, op_name);
    ExprValue out;

    if (!is_numeric_type(left.type) || !is_numeric_type(right.type)) {
        return make_number(0.0, TYPE_UNKNOWN);
    }

    out = make_number(0.0, promote_numeric_type(left.type, right.type));

    switch (op) {
        case '+': out.num = left.num + right.num; break;
        case '-': out.num = left.num - right.num; break;
        case '*': out.num = left.num * right.num; break;
        case '/':
            if (right.num == 0.0) {
                semantic_error("division by zero");
                out.num = 0.0;
            } else {
                out.num = left.num / right.num;
            }
            break;
        case '%':
            if ((int)right.num == 0) {
                semantic_error("modulo by zero");
                out.num = 0.0;
            } else {
                out.num = (double)((int)left.num % (int)right.num);
                out.type = TYPE_INT;
            }
            break;
    }

    out.is_lvalue = 0;
    return out;
}

void begin_print_args(void) {
    collect_print_args = 1;
    print_buffer[0] = '\0';
}

void append_print_arg(ExprValue v) {
    char chunk[256];
    if (!collect_print_args) return;

    if (strlen(print_buffer) > 0) {
        strncat(print_buffer, " ", sizeof(print_buffer) - strlen(print_buffer) - 1);
    }

    if (v.is_string || v.type == TYPE_STRING) {
        snprintf(chunk, sizeof(chunk), "%s", v.str);
    } else if (v.type == TYPE_CHAR) {
        snprintf(chunk, sizeof(chunk), "%c", (char)v.num);
    } else if (v.type == TYPE_BOOL) {
        snprintf(chunk, sizeof(chunk), "%s", v.num != 0.0 ? "true" : "false");
    } else if (is_numeric_type(v.type)) {
        if (v.type == TYPE_INT || v.type == TYPE_LONG) {
            snprintf(chunk, sizeof(chunk), "%lld", (long long)v.num);
        } else {
            snprintf(chunk, sizeof(chunk), "%.6f", v.num);
        }
    } else {
        snprintf(chunk, sizeof(chunk), "<unknown>");
    }

    strncat(print_buffer, chunk, sizeof(print_buffer) - strlen(print_buffer) - 1);
}

void end_print_args(void) {
    collect_print_args = 0;
    printf("[pikapika] %s\n", print_buffer);
    emit_ir("PRINT %s", print_buffer);
}
%}

%code requires {
#ifndef POKELANG_TYPES_DEFINED
#define POKELANG_TYPES_DEFINED
typedef enum {
    TYPE_UNKNOWN = 0,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_LONG,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_STRING
} DataType;

typedef struct {
    double num;
    DataType type;
    int is_lvalue;
    char ident[64];
    int is_string;
    char str[256];
} ExprValue;
#endif
}

%union {
    int intval;
    double floatval;
    char* strval;
    char charval;
    ExprValue expr;
}

/* Token declarations - matching lexer output */

/* Preprocessor directives */
%token INCLUDE DEFINE

/* Data types */
%token INT FLOAT DOUBLE LONG CHAR BOOL VOID

/* Control flow */
%token IF ELSE_IF ELSE
%token FOR WHILE DO
%token SWITCH CASE DEFAULT

/* Jump statements */
%token BREAK CONTINUE RETURN

/* I/O functions */
%token PRINT SCAN

/* Math functions */
%token POW SQRT FLOOR CEIL ABS
%token SIN COS TAN ASIN ACOS ATAN
%token PRIME

/* Special keywords */
%token FUNCTION MAIN

/* Operators */
%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token AND OR NOT
%token ASSIGN
%token LESS_THAN GREATER_THAN LESS_EQUAL GREATER_EQUAL EQUAL NOT_EQUAL

/* Delimiters */
%token SEMICOLON LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET COMMA

/* Literals and identifiers */
%token <intval> INTEGER_LITERAL
%token <floatval> FLOAT_LITERAL
%token <strval> STRING_LITERAL
%token <charval> CHAR_LITERAL
%token <strval> IDENTIFIER

%type <intval> type_specifier parameter_list argument_list

%type <expr> expression
%type <expr> assignment_expression
%type <expr> logical_or_expression
%type <expr> logical_and_expression
%type <expr> equality_expression
%type <expr> relational_expression
%type <expr> additive_expression
%type <expr> multiplicative_expression
%type <expr> unary_expression
%type <expr> postfix_expression
%type <expr> primary_expression
%type <expr> constant
%type <expr> math_function

/* Operator precedence and associativity (lowest to highest) */
%right ASSIGN
%left OR
%left AND
%left EQUAL NOT_EQUAL
%left LESS_THAN GREATER_THAN LESS_EQUAL GREATER_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT UNARY_MINUS
%left LPAREN RPAREN LBRACKET RBRACKET

%%

program:
    /* empty */
    | declaration_list { trace_reduce("program -> declaration_list"); }
    ;

declaration_list:
    declaration
    | declaration_list declaration
    ;

declaration:
    preprocessor_directive { trace_reduce("declaration -> preprocessor_directive"); }
    | function_declaration { trace_reduce("declaration -> function_declaration"); }
    | variable_declaration SEMICOLON { trace_reduce("declaration -> variable_declaration SEMICOLON"); }
    ;

preprocessor_directive:
    INCLUDE LESS_THAN IDENTIFIER GREATER_THAN
    | INCLUDE LESS_THAN IDENTIFIER DIVIDE IDENTIFIER GREATER_THAN
    | INCLUDE STRING_LITERAL
    | DEFINE IDENTIFIER INTEGER_LITERAL
    | DEFINE IDENTIFIER FLOAT_LITERAL
    | DEFINE IDENTIFIER STRING_LITERAL
    ;

/* Function declarations */
function_declaration:
    FUNCTION type_specifier IDENTIFIER LPAREN
    {
        register_function($3, (DataType)$2, 0);
        enter_function_context($3, (DataType)$2);
        free($3);
    }
    parameter_list RPAREN
    {
        int idx = find_function(current_function_name);
        if (idx >= 0) functions[idx].param_count = $6;
    }
    compound_statement
    {
        leave_function_context();
    }
    | FUNCTION type_specifier IDENTIFIER LPAREN
    {
        register_function($3, (DataType)$2, 0);
        enter_function_context($3, (DataType)$2);
        free($3);
    }
    RPAREN compound_statement
    {
        leave_function_context();
    }
    | FUNCTION type_specifier MAIN LPAREN
    {
        register_function("StartBattle", (DataType)$2, 0);
        enter_function_context("StartBattle", (DataType)$2);
    }
    parameter_list RPAREN
    {
        int idx = find_function("StartBattle");
        if (idx >= 0) functions[idx].param_count = $6;
    }
    compound_statement
    {
        leave_function_context();
    }
    | FUNCTION type_specifier MAIN LPAREN
    {
        register_function("StartBattle", (DataType)$2, 0);
        enter_function_context("StartBattle", (DataType)$2);
    }
    RPAREN compound_statement
    {
        leave_function_context();
    }
    ;

parameter_list:
    parameter { $$ = 1; }
    | parameter_list COMMA parameter { $$ = $1 + 1; }
    ;

parameter:
    type_specifier IDENTIFIER
    {
        int idx = declare_symbol($2, (DataType)$1, 0);
        if (idx >= 0) symbols[idx].initialized = 1;
        free($2);
    }
    | type_specifier IDENTIFIER LBRACKET RBRACKET
    {
        int idx = declare_symbol($2, (DataType)$1, 1);
        if (idx >= 0) symbols[idx].initialized = 1;
        free($2);
    }
    | type_specifier IDENTIFIER LBRACKET INTEGER_LITERAL RBRACKET
    {
        int idx = declare_symbol($2, (DataType)$1, 1);
        if (idx >= 0) symbols[idx].initialized = 1;
        free($2);
    }
    ;

type_specifier:
    INT    { $$ = TYPE_INT; current_decl_type = TYPE_INT; }
    | FLOAT  { $$ = TYPE_FLOAT; current_decl_type = TYPE_FLOAT; }
    | DOUBLE { $$ = TYPE_DOUBLE; current_decl_type = TYPE_DOUBLE; }
    | LONG   { $$ = TYPE_LONG; current_decl_type = TYPE_LONG; }
    | CHAR   { $$ = TYPE_CHAR; current_decl_type = TYPE_CHAR; }
    | BOOL   { $$ = TYPE_BOOL; current_decl_type = TYPE_BOOL; }
    | VOID   { $$ = TYPE_VOID; current_decl_type = TYPE_VOID; }
    ;

/* Variable declarations */
variable_declaration:
    type_specifier declarator_list { trace_reduce("variable_declaration -> type_specifier declarator_list"); }
    ;

declarator_list:
    declarator
    | declarator_list COMMA declarator
    ;

declarator:
    IDENTIFIER
    {
        declare_symbol($1, current_decl_type, 0);
        free($1);
    }
    | IDENTIFIER ASSIGN expression
    {
        declare_symbol($1, current_decl_type, 0);
        assign_symbol($1, $3);
        free($1);
    }
    | IDENTIFIER LBRACKET INTEGER_LITERAL RBRACKET
    {
        declare_symbol($1, current_decl_type, 1);
        free($1);
    }
    | IDENTIFIER LBRACKET RBRACKET
    {
        declare_symbol($1, current_decl_type, 1);
        free($1);
    }
    ;

/* Statements */
statement_list:
    /* empty */
    | statement_list statement
    ;

statement:
    compound_statement { trace_reduce("statement -> compound_statement"); }
    | expression_statement { trace_reduce("statement -> expression_statement"); }
    | selection_statement { trace_reduce("statement -> selection_statement"); }
    | iteration_statement { trace_reduce("statement -> iteration_statement"); }
    | jump_statement { trace_reduce("statement -> jump_statement"); }
    | variable_declaration SEMICOLON { trace_reduce("statement -> variable_declaration SEMICOLON"); }
    ;

compound_statement:
    LBRACE statement_list RBRACE
    ;

expression_statement:
    SEMICOLON
    | expression SEMICOLON
    ;

/* Selection statements (if-else, switch) */
selection_statement:
    IF LPAREN expression RPAREN compound_statement
    {
        trace_reduce("selection_statement -> IF (...) compound_statement");
        if ($3.type == TYPE_STRING) {
            semantic_error("if condition cannot be string");
        }
    }
    | IF LPAREN expression RPAREN compound_statement ELSE_IF LPAREN expression RPAREN compound_statement
    {
        trace_reduce("selection_statement -> IF ... ELSE_IF ...");
        if ($3.type == TYPE_STRING || $8.type == TYPE_STRING) {
            semantic_error("if/else-if condition cannot be string");
        }
    }
    | IF LPAREN expression RPAREN compound_statement ELSE_IF LPAREN expression RPAREN compound_statement ELSE compound_statement
    {
        trace_reduce("selection_statement -> IF ... ELSE_IF ... ELSE ...");
        if ($3.type == TYPE_STRING || $8.type == TYPE_STRING) {
            semantic_error("if/else-if condition cannot be string");
        }
    }
    | IF LPAREN expression RPAREN compound_statement ELSE compound_statement
    {
        trace_reduce("selection_statement -> IF ... ELSE ...");
        if ($3.type == TYPE_STRING) {
            semantic_error("if condition cannot be string");
        }
    }
    | SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE { trace_reduce("selection_statement -> SWITCH (...) { case_list }"); }
    ;

case_list:
    case_statement
    | case_list case_statement
    ;

case_statement:
    CASE constant_expression SEMICOLON statement_list
    | DEFAULT SEMICOLON statement_list
    ;

/* Iteration statements (loops) */
iteration_statement:
    WHILE LPAREN expression RPAREN statement
    {
        trace_reduce("iteration_statement -> WHILE (...) statement");
        if ($3.type == TYPE_STRING) {
            semantic_error("while condition cannot be string");
        }
    }
    | DO statement WHILE LPAREN expression RPAREN SEMICOLON
    {
        trace_reduce("iteration_statement -> DO statement WHILE (...) ;");
        if ($5.type == TYPE_STRING) {
            semantic_error("do-while condition cannot be string");
        }
    }
    | FOR LPAREN expression_statement expression_statement RPAREN statement { trace_reduce("iteration_statement -> FOR (init;cond;) statement"); }
    | FOR LPAREN expression_statement expression_statement expression RPAREN statement
    {
        trace_reduce("iteration_statement -> FOR (init;cond;update) statement");
        if ($5.type == TYPE_STRING) {
            semantic_error("for condition/update cannot be string where numeric expected");
        }
    }
    | FOR LPAREN variable_declaration SEMICOLON expression_statement RPAREN statement { trace_reduce("iteration_statement -> FOR (decl;cond;) statement"); }
    | FOR LPAREN variable_declaration SEMICOLON expression_statement expression RPAREN statement
    {
        trace_reduce("iteration_statement -> FOR (decl;cond;update) statement");
        if ($6.type == TYPE_STRING) {
            semantic_error("for update cannot be string");
        }
    }
    ;

/* Jump statements */
jump_statement:
    BREAK SEMICOLON { trace_reduce("jump_statement -> BREAK ;"); }
    | CONTINUE SEMICOLON { trace_reduce("jump_statement -> CONTINUE ;"); }
    | RETURN SEMICOLON
    {
        trace_reduce("jump_statement -> RETURN ;");
        if (current_function_return_type != TYPE_VOID) {
            semantic_error(
                "function '%s' must return %s value",
                current_function_name,
                dtype_name(current_function_return_type)
            );
        }
    }
    | RETURN expression SEMICOLON
    {
        trace_reduce("jump_statement -> RETURN expression ;");
        if (current_function_return_type == TYPE_VOID) {
            if (!(is_numeric_type($2.type) && $2.num == 0.0)) {
                semantic_error("void function '%s' cannot return a value", current_function_name);
            }
        } else if (!can_assign(current_function_return_type, $2.type)) {
            semantic_error(
                "return type mismatch in function '%s': expected %s, got %s",
                current_function_name,
                dtype_name(current_function_return_type),
                dtype_name($2.type)
            );
        }
    }
    ;

/* Expressions */
expression:
    assignment_expression { $$ = $1; }
    | expression COMMA assignment_expression { $$ = $3; }
    ;

assignment_expression:
    logical_or_expression { $$ = $1; trace_reduce("assignment_expression -> logical_or_expression"); }
    | postfix_expression ASSIGN assignment_expression
    {
        trace_reduce("assignment_expression -> postfix_expression ASSIGN assignment_expression");
        if (!$1.is_lvalue) {
            semantic_error("left side of assignment must be a variable");
            $$ = $3;
        } else {
            assign_symbol($1.ident, $3);
            $$ = $3;
        }
    }
    ;

logical_or_expression:
    logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression
    {
        ExprValue a = ensure_numeric($1, "team");
        ExprValue b = ensure_numeric($3, "team");
        $$ = make_number((a.num != 0.0 || b.num != 0.0) ? 1.0 : 0.0, TYPE_BOOL);
    }
    ;

logical_and_expression:
    equality_expression { $$ = $1; }
    | logical_and_expression AND equality_expression
    {
        ExprValue a = ensure_numeric($1, "crit");
        ExprValue b = ensure_numeric($3, "crit");
        $$ = make_number((a.num != 0.0 && b.num != 0.0) ? 1.0 : 0.0, TYPE_BOOL);
    }
    ;

equality_expression:
    relational_expression { $$ = $1; }
    | equality_expression EQUAL relational_expression
    {
        $$ = make_number($1.num == $3.num ? 1.0 : 0.0, TYPE_BOOL);
    }
    | equality_expression NOT_EQUAL relational_expression
    {
        $$ = make_number($1.num != $3.num ? 1.0 : 0.0, TYPE_BOOL);
    }
    ;

relational_expression:
    additive_expression { $$ = $1; }
    | relational_expression LESS_THAN additive_expression
    {
        $$ = make_number($1.num < $3.num ? 1.0 : 0.0, TYPE_BOOL);
    }
    | relational_expression GREATER_THAN additive_expression
    {
        $$ = make_number($1.num > $3.num ? 1.0 : 0.0, TYPE_BOOL);
    }
    | relational_expression LESS_EQUAL additive_expression
    {
        $$ = make_number($1.num <= $3.num ? 1.0 : 0.0, TYPE_BOOL);
    }
    | relational_expression GREATER_EQUAL additive_expression
    {
        $$ = make_number($1.num >= $3.num ? 1.0 : 0.0, TYPE_BOOL);
    }
    ;

additive_expression:
    multiplicative_expression { $$ = $1; }
    | additive_expression PLUS multiplicative_expression
    {
        $$ = eval_binary_numeric($1, $3, "atk", '+');
    }
    | additive_expression MINUS multiplicative_expression
    {
        $$ = eval_binary_numeric($1, $3, "dmg", '-');
    }
    ;

multiplicative_expression:
    unary_expression { $$ = $1; }
    | multiplicative_expression MULTIPLY unary_expression
    {
        $$ = eval_binary_numeric($1, $3, "x", '*');
    }
    | multiplicative_expression DIVIDE unary_expression
    {
        $$ = eval_binary_numeric($1, $3, "hp", '/');
    }
    | multiplicative_expression MODULO unary_expression
    {
        $$ = eval_binary_numeric($1, $3, "mod", '%');
    }
    ;

unary_expression:
    postfix_expression { $$ = $1; }
    | MINUS unary_expression %prec UNARY_MINUS
    {
        ExprValue v = ensure_numeric($2, "unary minus");
        $$ = make_number(-v.num, v.type);
    }
    | NOT unary_expression
    {
        ExprValue v = ensure_numeric($2, "confuse");
        $$ = make_number(v.num == 0.0 ? 1.0 : 0.0, TYPE_BOOL);
    }
    ;

postfix_expression:
    primary_expression { $$ = $1; }
    | postfix_expression LBRACKET expression RBRACKET
    {
        if (!$1.is_lvalue) {
            semantic_error("array indexing requires variable identifier");
            $$ = make_number(0.0, TYPE_UNKNOWN);
        } else {
            int idx = find_symbol($1.ident);
            if (idx >= 0 && !symbols[idx].is_array) {
                semantic_warning("variable '%s' used with array indexing but not declared as array", $1.ident);
            }
            if (idx >= 0) {
                $$ = make_number(symbols[idx].numeric_value, symbols[idx].type);
            } else {
                $$ = make_number(0.0, TYPE_UNKNOWN);
            }
            $$.is_lvalue = 1;
            snprintf($$.ident, sizeof($$.ident), "%s", $1.ident);
        }
    }
    | postfix_expression LPAREN RPAREN
    {
        if (!$1.is_lvalue) {
            semantic_error("function call requires function identifier");
            $$ = make_number(0.0, TYPE_UNKNOWN);
        } else {
            $$ = evaluate_function_call($1.ident, 0);
        }
    }
    | postfix_expression LPAREN argument_list RPAREN
    {
        if (!$1.is_lvalue) {
            semantic_error("function call requires function identifier");
            $$ = make_number(0.0, TYPE_UNKNOWN);
        } else {
            $$ = evaluate_function_call($1.ident, $3);
        }
    }
    ;

primary_expression:
    IDENTIFIER
    {
        if (find_symbol($1) >= 0) {
            $$ = read_symbol($1);
        } else if (find_function($1) >= 0) {
            $$ = make_number(0.0, TYPE_UNKNOWN);
            $$.is_lvalue = 1;
            snprintf($$.ident, sizeof($$.ident), "%s", $1);
        } else {
            semantic_error("use of undeclared variable '%s'", $1);
            $$ = make_number(0.0, TYPE_UNKNOWN);
        }
        free($1);
    }
    | constant { $$ = $1; }
    | STRING_LITERAL
    {
        $$ = make_string($1);
        free($1);
    }
    | LPAREN expression RPAREN { $$ = $2; }
    | math_function { $$ = $1; }
    ;

constant:
    INTEGER_LITERAL { $$ = make_number((double)$1, TYPE_INT); }
    | FLOAT_LITERAL { $$ = make_number($1, TYPE_FLOAT); }
    | CHAR_LITERAL { $$ = make_number((double)$1, TYPE_CHAR); }
    ;

constant_expression:
    INTEGER_LITERAL
    | CHAR_LITERAL
    ;

argument_list:
    assignment_expression
    {
        if (collect_print_args) append_print_arg($1);
        $$ = 1;
    }
    | argument_list COMMA assignment_expression
    {
        if (collect_print_args) append_print_arg($3);
        $$ = $1 + 1;
    }
    ;

/* Math functions */
math_function:
    POW LPAREN expression COMMA expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "powerup");
        ExprValue b = ensure_numeric($5, "powerup");
        $$ = make_number(pow(a.num, b.num), TYPE_DOUBLE);
    }
    | SQRT LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "rootchu");
        if (a.num < 0.0) semantic_error("sqrt of negative value");
        $$ = make_number(sqrt(fabs(a.num)), TYPE_DOUBLE);
    }
    | FLOOR LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "floorchu");
        $$ = make_number(floor(a.num), TYPE_DOUBLE);
    }
    | CEIL LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "ceilchu");
        $$ = make_number(ceil(a.num), TYPE_DOUBLE);
    }
    | ABS LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "heal");
        $$ = make_number(fabs(a.num), TYPE_DOUBLE);
    }
    | SIN LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "sinchu");
        $$ = make_number(sin(a.num), TYPE_DOUBLE);
    }
    | COS LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "coschu");
        $$ = make_number(cos(a.num), TYPE_DOUBLE);
    }
    | TAN LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "tanchu");
        $$ = make_number(tan(a.num), TYPE_DOUBLE);
    }
    | ASIN LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "asinchu");
        $$ = make_number(asin(a.num), TYPE_DOUBLE);
    }
    | ACOS LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "acoschu");
        $$ = make_number(acos(a.num), TYPE_DOUBLE);
    }
    | ATAN LPAREN expression RPAREN
    {
        ExprValue a = ensure_numeric($3, "atanchu");
        $$ = make_number(atan(a.num), TYPE_DOUBLE);
    }
    | PRIME LPAREN expression RPAREN
    {
        int i;
        int n;
        ExprValue a = ensure_numeric($3, "primechu");
        n = (int)a.num;
        if (n < 2) {
            $$ = make_number(0.0, TYPE_BOOL);
        } else {
            $$ = make_number(1.0, TYPE_BOOL);
            for (i = 2; i * i <= n; ++i) {
                if (n % i == 0) {
                    $$.num = 0.0;
                    break;
                }
            }
        }
    }
    | PRINT LPAREN
    {
        begin_print_args();
    }
    argument_list RPAREN
    {
        end_print_args();
        $$ = make_number(0.0, TYPE_INT);
    }
    | SCAN LPAREN argument_list RPAREN
    {
        semantic_warning("pokecatch runtime input is parsed but not executed in this build");
        $$ = make_number(0.0, TYPE_INT);
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Syntax error at line %d: %s\n", line_number, s);
    syntax_error_count++;
}

int main(int argc, char* argv[]) {
    int result;
    int i;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.poke> [--trace]\n", argv[0]);
        return 1;
    }

    for (i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--trace") == 0 || strcmp(argv[i], "-t") == 0) {
            explain_trace_mode = 1;
        }
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", argv[1]);
        return 1;
    }

    ir_out = fopen("intermediate_code.ir", "w");
    if (ir_out) {
        fprintf(ir_out, "# PokemonLang Intermediate Representation\n");
        fprintf(ir_out, "# Source: %s\n\n", argv[1]);
    }

    printf("=== PokemonLang Compiler ===\n");
    printf("Input file: %s\n", argv[1]);
    printf("Parsing + semantic analysis...\n\n");
    if (explain_trace_mode) {
        printf("[TRACE] Explainability mode enabled\n");
        printf("[TRACE] Stages: token stream -> grammar reductions -> semantic actions -> IR lines\n\n");
    }

    result = yyparse();

    printf("\n=== Compilation Result ===\n");
    if (result == 0 && syntax_error_count == 0 && semantic_error_count == 0 && indentation_error_count == 0) {
        printf("[SUCCESS] Parsing completed successfully.\n");
        printf("[SUCCESS] Semantic checks passed.\n");
        printf("[SUCCESS] Indentation checks passed.\n");
        printf("[INFO] Semantic warnings: %d\n", semantic_warning_count);
        if (ir_out) {
            printf("[INFO] Intermediate code generated: intermediate_code.ir\n");
        }
    } else {
        printf("[FAILED] Compilation failed.\n");
        printf("[FAILED] Syntax errors: %d\n", syntax_error_count);
        printf("[FAILED] Semantic errors: %d\n", semantic_error_count);
        printf("[FAILED] Indentation errors: %d\n", indentation_error_count);
        printf("[INFO] Semantic warnings: %d\n", semantic_warning_count);
    }

    if (yyin) fclose(yyin);
    if (ir_out) fclose(ir_out);

    if (explain_trace_mode) {
        printf("[TRACE] Explainability run complete.\n");
    }

    return (result == 0 && syntax_error_count == 0 && semantic_error_count == 0 && indentation_error_count == 0) ? 0 : 1;
}
