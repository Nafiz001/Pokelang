/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "pokelang.y"

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
    if (!ir_out) return;
    va_start(args, fmt);
    vfprintf(ir_out, fmt, args);
    fprintf(ir_out, "\n");
    va_end(args);
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

    /* Placeholder runtime value for user-defined functions. */
    return make_number(0.0, functions[idx].return_type);
}

void enter_function_context(const char* name, DataType return_type) {
    snprintf(current_function_name, sizeof(current_function_name), "%s", name);
    current_function_return_type = return_type;
    symbol_count = 0; /* Simplified per-function scope. */
    emit_ir("ENTER %s", current_function_name);
}

void leave_function_context(void) {
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

#line 479 "pokelang.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "pokelang.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INCLUDE = 3,                    /* INCLUDE  */
  YYSYMBOL_DEFINE = 4,                     /* DEFINE  */
  YYSYMBOL_INT = 5,                        /* INT  */
  YYSYMBOL_FLOAT = 6,                      /* FLOAT  */
  YYSYMBOL_DOUBLE = 7,                     /* DOUBLE  */
  YYSYMBOL_LONG = 8,                       /* LONG  */
  YYSYMBOL_CHAR = 9,                       /* CHAR  */
  YYSYMBOL_BOOL = 10,                      /* BOOL  */
  YYSYMBOL_VOID = 11,                      /* VOID  */
  YYSYMBOL_IF = 12,                        /* IF  */
  YYSYMBOL_ELSE_IF = 13,                   /* ELSE_IF  */
  YYSYMBOL_ELSE = 14,                      /* ELSE  */
  YYSYMBOL_FOR = 15,                       /* FOR  */
  YYSYMBOL_WHILE = 16,                     /* WHILE  */
  YYSYMBOL_DO = 17,                        /* DO  */
  YYSYMBOL_SWITCH = 18,                    /* SWITCH  */
  YYSYMBOL_CASE = 19,                      /* CASE  */
  YYSYMBOL_DEFAULT = 20,                   /* DEFAULT  */
  YYSYMBOL_BREAK = 21,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 22,                  /* CONTINUE  */
  YYSYMBOL_RETURN = 23,                    /* RETURN  */
  YYSYMBOL_PRINT = 24,                     /* PRINT  */
  YYSYMBOL_SCAN = 25,                      /* SCAN  */
  YYSYMBOL_POW = 26,                       /* POW  */
  YYSYMBOL_SQRT = 27,                      /* SQRT  */
  YYSYMBOL_FLOOR = 28,                     /* FLOOR  */
  YYSYMBOL_CEIL = 29,                      /* CEIL  */
  YYSYMBOL_ABS = 30,                       /* ABS  */
  YYSYMBOL_SIN = 31,                       /* SIN  */
  YYSYMBOL_COS = 32,                       /* COS  */
  YYSYMBOL_TAN = 33,                       /* TAN  */
  YYSYMBOL_ASIN = 34,                      /* ASIN  */
  YYSYMBOL_ACOS = 35,                      /* ACOS  */
  YYSYMBOL_ATAN = 36,                      /* ATAN  */
  YYSYMBOL_PRIME = 37,                     /* PRIME  */
  YYSYMBOL_FUNCTION = 38,                  /* FUNCTION  */
  YYSYMBOL_MAIN = 39,                      /* MAIN  */
  YYSYMBOL_PLUS = 40,                      /* PLUS  */
  YYSYMBOL_MINUS = 41,                     /* MINUS  */
  YYSYMBOL_MULTIPLY = 42,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 43,                    /* DIVIDE  */
  YYSYMBOL_MODULO = 44,                    /* MODULO  */
  YYSYMBOL_AND = 45,                       /* AND  */
  YYSYMBOL_OR = 46,                        /* OR  */
  YYSYMBOL_NOT = 47,                       /* NOT  */
  YYSYMBOL_ASSIGN = 48,                    /* ASSIGN  */
  YYSYMBOL_LESS_THAN = 49,                 /* LESS_THAN  */
  YYSYMBOL_GREATER_THAN = 50,              /* GREATER_THAN  */
  YYSYMBOL_LESS_EQUAL = 51,                /* LESS_EQUAL  */
  YYSYMBOL_GREATER_EQUAL = 52,             /* GREATER_EQUAL  */
  YYSYMBOL_EQUAL = 53,                     /* EQUAL  */
  YYSYMBOL_NOT_EQUAL = 54,                 /* NOT_EQUAL  */
  YYSYMBOL_SEMICOLON = 55,                 /* SEMICOLON  */
  YYSYMBOL_LPAREN = 56,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 57,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 58,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 59,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 60,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 61,                  /* RBRACKET  */
  YYSYMBOL_COMMA = 62,                     /* COMMA  */
  YYSYMBOL_INTEGER_LITERAL = 63,           /* INTEGER_LITERAL  */
  YYSYMBOL_FLOAT_LITERAL = 64,             /* FLOAT_LITERAL  */
  YYSYMBOL_STRING_LITERAL = 65,            /* STRING_LITERAL  */
  YYSYMBOL_CHAR_LITERAL = 66,              /* CHAR_LITERAL  */
  YYSYMBOL_IDENTIFIER = 67,                /* IDENTIFIER  */
  YYSYMBOL_UNARY_MINUS = 68,               /* UNARY_MINUS  */
  YYSYMBOL_YYACCEPT = 69,                  /* $accept  */
  YYSYMBOL_program = 70,                   /* program  */
  YYSYMBOL_declaration_list = 71,          /* declaration_list  */
  YYSYMBOL_declaration = 72,               /* declaration  */
  YYSYMBOL_preprocessor_directive = 73,    /* preprocessor_directive  */
  YYSYMBOL_function_declaration = 74,      /* function_declaration  */
  YYSYMBOL_75_1 = 75,                      /* $@1  */
  YYSYMBOL_76_2 = 76,                      /* $@2  */
  YYSYMBOL_77_3 = 77,                      /* $@3  */
  YYSYMBOL_78_4 = 78,                      /* $@4  */
  YYSYMBOL_79_5 = 79,                      /* $@5  */
  YYSYMBOL_80_6 = 80,                      /* $@6  */
  YYSYMBOL_parameter_list = 81,            /* parameter_list  */
  YYSYMBOL_parameter = 82,                 /* parameter  */
  YYSYMBOL_type_specifier = 83,            /* type_specifier  */
  YYSYMBOL_variable_declaration = 84,      /* variable_declaration  */
  YYSYMBOL_declarator_list = 85,           /* declarator_list  */
  YYSYMBOL_declarator = 86,                /* declarator  */
  YYSYMBOL_statement_list = 87,            /* statement_list  */
  YYSYMBOL_statement = 88,                 /* statement  */
  YYSYMBOL_compound_statement = 89,        /* compound_statement  */
  YYSYMBOL_expression_statement = 90,      /* expression_statement  */
  YYSYMBOL_selection_statement = 91,       /* selection_statement  */
  YYSYMBOL_case_list = 92,                 /* case_list  */
  YYSYMBOL_case_statement = 93,            /* case_statement  */
  YYSYMBOL_iteration_statement = 94,       /* iteration_statement  */
  YYSYMBOL_jump_statement = 95,            /* jump_statement  */
  YYSYMBOL_expression = 96,                /* expression  */
  YYSYMBOL_assignment_expression = 97,     /* assignment_expression  */
  YYSYMBOL_logical_or_expression = 98,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 99,    /* logical_and_expression  */
  YYSYMBOL_equality_expression = 100,      /* equality_expression  */
  YYSYMBOL_relational_expression = 101,    /* relational_expression  */
  YYSYMBOL_additive_expression = 102,      /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 103, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 104,         /* unary_expression  */
  YYSYMBOL_postfix_expression = 105,       /* postfix_expression  */
  YYSYMBOL_primary_expression = 106,       /* primary_expression  */
  YYSYMBOL_constant = 107,                 /* constant  */
  YYSYMBOL_constant_expression = 108,      /* constant_expression  */
  YYSYMBOL_argument_list = 109,            /* argument_list  */
  YYSYMBOL_math_function = 110,            /* math_function  */
  YYSYMBOL_111_7 = 111                     /* $@7  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  22
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   600

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  130
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  281

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   323


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   515,   515,   517,   521,   522,   526,   527,   528,   532,
     533,   534,   535,   536,   537,   543,   549,   542,   558,   557,
     568,   573,   567,   582,   581,   593,   594,   598,   604,   610,
     619,   620,   621,   622,   623,   624,   625,   630,   634,   635,
     639,   644,   650,   655,   663,   665,   669,   670,   671,   672,
     673,   674,   678,   682,   683,   688,   694,   700,   706,   712,
     716,   717,   721,   722,   727,   733,   739,   740,   746,   747,
     757,   758,   759,   769,   788,   789,   793,   794,   807,   808,
     817,   818,   827,   828,   832,   839,   840,   844,   848,   852,
     859,   860,   864,   871,   872,   876,   880,   887,   888,   893,
     901,   902,   921,   930,   942,   956,   957,   962,   963,   967,
     968,   969,   973,   974,   978,   983,   992,   998,  1004,  1009,
    1014,  1019,  1024,  1029,  1034,  1039,  1044,  1049,  1068,  1067,
    1076
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INCLUDE", "DEFINE",
  "INT", "FLOAT", "DOUBLE", "LONG", "CHAR", "BOOL", "VOID", "IF",
  "ELSE_IF", "ELSE", "FOR", "WHILE", "DO", "SWITCH", "CASE", "DEFAULT",
  "BREAK", "CONTINUE", "RETURN", "PRINT", "SCAN", "POW", "SQRT", "FLOOR",
  "CEIL", "ABS", "SIN", "COS", "TAN", "ASIN", "ACOS", "ATAN", "PRIME",
  "FUNCTION", "MAIN", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MODULO",
  "AND", "OR", "NOT", "ASSIGN", "LESS_THAN", "GREATER_THAN", "LESS_EQUAL",
  "GREATER_EQUAL", "EQUAL", "NOT_EQUAL", "SEMICOLON", "LPAREN", "RPAREN",
  "LBRACE", "RBRACE", "LBRACKET", "RBRACKET", "COMMA", "INTEGER_LITERAL",
  "FLOAT_LITERAL", "STRING_LITERAL", "CHAR_LITERAL", "IDENTIFIER",
  "UNARY_MINUS", "$accept", "program", "declaration_list", "declaration",
  "preprocessor_directive", "function_declaration", "$@1", "$@2", "$@3",
  "$@4", "$@5", "$@6", "parameter_list", "parameter", "type_specifier",
  "variable_declaration", "declarator_list", "declarator",
  "statement_list", "statement", "compound_statement",
  "expression_statement", "selection_statement", "case_list",
  "case_statement", "iteration_statement", "jump_statement", "expression",
  "assignment_expression", "logical_or_expression",
  "logical_and_expression", "equality_expression", "relational_expression",
  "additive_expression", "multiplicative_expression", "unary_expression",
  "postfix_expression", "primary_expression", "constant",
  "constant_expression", "argument_list", "math_function", "$@7", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-208)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-24)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     187,   -40,   -52,  -208,  -208,  -208,  -208,  -208,  -208,  -208,
     125,    18,   187,  -208,  -208,  -208,   -24,    -4,     2,  -208,
     203,   -33,  -208,  -208,   -31,    15,  -208,  -208,   -10,  -208,
    -208,  -208,    37,    72,   533,   -22,   -24,    43,  -208,   109,
     132,   154,   163,   202,   207,   259,   266,   267,   270,   272,
     273,   274,   295,   296,   297,   533,   533,   533,  -208,  -208,
    -208,  -208,  -208,   137,  -208,   155,   199,    66,   126,    39,
     265,  -208,   -28,  -208,  -208,  -208,  -208,   260,  -208,   198,
     125,   298,   125,   299,  -208,   533,   533,   533,   533,   533,
     533,   533,   533,   533,   533,   533,   533,   533,  -208,    35,
    -208,   -43,   533,   533,   533,   533,   533,   533,   533,   533,
     533,   533,   533,   533,   533,   533,   533,   313,   533,  -208,
    -208,   -35,  -208,   264,   300,   -15,   300,   533,  -208,    14,
     301,    16,    24,    26,    30,    52,    54,    55,    56,    99,
     102,   103,  -208,  -208,   199,    66,   126,   126,    39,    39,
      39,    39,   265,   265,  -208,  -208,  -208,  -208,  -208,   105,
      77,  -208,   125,   302,  -208,  -208,  -208,  -208,   111,  -208,
     533,   533,  -208,  -208,  -208,  -208,  -208,  -208,  -208,  -208,
    -208,  -208,  -208,  -208,  -208,   300,  -208,   124,   206,   300,
    -208,  -208,   112,  -208,  -208,  -208,   303,   305,   309,   310,
     269,   311,   304,   316,   357,  -208,  -208,   317,  -208,  -208,
    -208,  -208,  -208,  -208,   -18,  -208,  -208,  -208,   533,   116,
     533,   341,   533,  -208,  -208,  -208,   -17,  -208,  -208,   143,
     318,   401,   145,   312,   146,  -208,   300,   401,   445,   269,
     533,   337,   275,   489,   269,   188,  -208,   189,   263,   319,
     300,   269,   192,  -208,   269,   342,   193,   344,    -9,  -208,
     533,  -208,  -208,   269,  -208,  -208,  -208,  -208,   345,  -208,
    -208,  -208,   195,  -208,  -208,   269,   300,   269,   360,   300,
    -208
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,    30,    31,    32,    33,    34,    35,    36,
       0,     0,     3,     4,     6,     7,     0,     0,     0,    11,
       0,     0,     1,     5,    40,    37,    38,     8,     0,    12,
      13,    14,     0,     0,     0,     0,     0,     0,     9,    20,
      15,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   109,   110,
     106,   111,   104,    41,    74,    76,    78,    80,    82,    85,
      90,    93,    97,   100,   105,   108,    43,     0,    39,     0,
       0,     0,     0,     0,   128,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    98,    97,
      99,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    42,
      10,     0,    25,     0,     0,     0,     0,     0,   114,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   107,    75,    79,    81,    83,    84,    86,    87,
      88,    89,    91,    92,    94,    95,    96,    77,   102,     0,
       0,    21,     0,    27,    44,    24,    16,    19,     0,   130,
       0,     0,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   103,   101,     0,    26,     0,     0,     0,
     129,   115,     0,    74,    22,    28,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    53,    52,     0,    45,    46,
      47,    48,    49,    50,     0,    17,   116,    29,     0,     0,
       0,     0,     0,    70,    71,    72,     0,    51,    54,     0,
       0,     0,     0,     0,     0,    73,     0,     0,     0,     0,
       0,     0,    55,     0,     0,     0,    64,     0,     0,     0,
       0,     0,     0,    66,     0,     0,     0,     0,     0,    60,
       0,    58,    68,     0,    67,    65,   112,   113,     0,    44,
      59,    61,     0,    69,    44,    63,     0,    62,    56,     0,
      57
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -208,  -208,  -208,   384,  -208,  -208,  -208,  -208,  -208,  -208,
    -208,  -208,   320,   239,    -8,     1,  -208,   367,   -14,  -169,
    -121,  -207,  -208,  -208,   147,  -208,  -208,   -34,   -81,  -208,
     306,   307,   212,   204,   208,   -48,    -7,  -208,  -208,  -208,
    -101,  -208,  -208
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    11,    12,    13,    14,    15,    82,   189,    83,    80,
     185,    81,   121,   122,    16,   207,    25,    26,   188,   208,
     209,   210,   211,   258,   259,   212,   213,   214,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,   268,
     129,    75,   127
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      63,    17,    21,   165,   128,   167,    32,    98,   100,    18,
     256,   257,   231,    17,   142,    20,   159,    34,    22,   102,
     116,   143,   161,   101,   238,    19,   168,   162,   117,    35,
     243,   221,   118,    37,    33,   157,   128,   228,   235,    76,
      38,    77,   166,    24,   102,   102,   128,   162,    99,    99,
     270,    27,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   194,   154,   155,   156,   215,    28,
     246,   169,   123,   172,   123,   253,   170,    36,   102,   111,
     112,   173,   262,   174,   160,   264,   102,   175,   102,   191,
     193,   117,   102,    39,   273,   118,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,   176,
      79,   177,   178,   179,   102,   242,   102,   102,   102,   105,
     106,     3,     4,     5,     6,     7,     8,     9,    40,   261,
       3,     4,     5,     6,     7,     8,     9,   192,   184,   102,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,   123,   278,   180,    55,   280,   181,
     182,   102,   183,    56,   102,   102,   -23,   170,   190,   216,
     226,   205,    57,   170,   102,   107,   108,   109,   110,    58,
      59,    60,    61,    62,   229,   195,   232,   196,   234,   -18,
       1,     2,     3,     4,     5,     6,     7,     8,     9,   102,
     236,   103,   239,   241,   245,   102,   247,   102,   102,   252,
      84,     3,     4,     5,     6,     7,     8,     9,   197,    85,
     230,   198,   199,   200,   201,    10,   272,   202,   203,   204,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,   104,   254,   255,    55,   120,   263,
     102,   102,   276,    56,   102,   275,   266,   102,    86,   267,
     277,   205,    57,    87,   164,   206,    29,    30,    31,    58,
      59,    60,    61,    62,     3,     4,     5,     6,     7,     8,
       9,   197,   256,   257,   198,   199,   200,   201,   249,   250,
     202,   203,   204,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,   113,   114,   115,
      55,   148,   149,   150,   151,    88,    56,   146,   147,   152,
     153,   119,    89,    90,   205,    57,    91,   164,    92,    93,
      94,   163,    58,    59,    60,    61,    62,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    95,    96,    97,    55,   124,   126,   233,   164,   223,
      56,   218,   187,   171,   217,   219,   220,   222,   240,    57,
     158,   224,   227,   237,   279,   260,    58,    59,    60,    61,
      62,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,   248,    23,   265,    55,   269,
     274,   186,   125,    78,    56,   271,     0,     0,     0,   144,
       0,   145,   225,    57,     0,     0,     0,     0,     0,     0,
      58,    59,    60,    61,    62,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,     0,
       0,     0,    55,     0,     0,     0,     0,     0,    56,     0,
       0,     0,     0,     0,     0,     0,   205,    57,     0,     0,
       0,     0,     0,     0,    58,    59,    60,    61,    62,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,     0,     0,     0,    55,     0,     0,     0,
       0,     0,    56,     0,     0,     0,     0,     0,     0,     0,
       0,    57,   244,     0,     0,     0,     0,     0,    58,    59,
      60,    61,    62,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,     0,     0,     0,
      55,     0,     0,     0,     0,     0,    56,     0,     0,     0,
       0,     0,     0,     0,     0,    57,   251,     0,     0,     0,
       0,     0,    58,    59,    60,    61,    62,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,     0,     0,     0,    55,     0,     0,     0,     0,     0,
      56,     0,     0,     0,     0,     0,     0,     0,     0,    57,
       0,     0,     0,     0,     0,     0,    58,    59,    60,    61,
      62
};

static const yytype_int16 yycheck[] =
{
      34,     0,    10,   124,    85,   126,    39,    55,    56,    49,
      19,    20,   219,    12,    57,    67,   117,    48,     0,    62,
      48,   102,    57,    57,   231,    65,   127,    62,    56,    60,
     237,   200,    60,    43,    67,   116,   117,    55,    55,    61,
      50,    63,    57,    67,    62,    62,   127,    62,    55,    56,
      59,    55,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,   185,   113,   114,   115,   189,    67,
     239,    57,    80,    57,    82,   244,    62,    62,    62,    40,
      41,    57,   251,    57,   118,   254,    62,    57,    62,   170,
     171,    56,    62,    56,   263,    60,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,    57,
      67,    57,    57,    57,    62,   236,    62,    62,    62,    53,
      54,     5,     6,     7,     8,     9,    10,    11,    56,   250,
       5,     6,     7,     8,     9,    10,    11,   171,    61,    62,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,   162,   276,    57,    41,   279,    57,
      57,    62,    57,    47,    62,    62,    57,    62,    57,    57,
     204,    55,    56,    62,    62,    49,    50,    51,    52,    63,
      64,    65,    66,    67,   218,    61,   220,    63,   222,    57,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    62,
      57,    46,    57,    57,   238,    62,   240,    62,    62,   243,
      56,     5,     6,     7,     8,     9,    10,    11,    12,    56,
     219,    15,    16,    17,    18,    38,   260,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    45,    57,    57,    41,    50,    57,
      62,    62,    57,    47,    62,   269,    63,    62,    56,    66,
     274,    55,    56,    56,    58,    59,    63,    64,    65,    63,
      64,    65,    66,    67,     5,     6,     7,     8,     9,    10,
      11,    12,    19,    20,    15,    16,    17,    18,    13,    14,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    42,    43,    44,
      41,   107,   108,   109,   110,    56,    47,   105,   106,   111,
     112,    61,    56,    56,    55,    56,    56,    58,    56,    56,
      56,    67,    63,    64,    65,    66,    67,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    56,    56,    56,    41,    57,    57,    16,    58,    55,
      47,    56,    60,    62,    61,    56,    56,    56,    56,    56,
      57,    55,    55,    55,    14,    56,    63,    64,    65,    66,
      67,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    58,    12,    55,    41,    55,
      55,   162,    82,    36,    47,   258,    -1,    -1,    -1,   103,
      -1,   104,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      63,    64,    65,    66,    67,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    64,    65,    66,    67,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    -1,    -1,    41,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    57,    -1,    -1,    -1,    -1,    -1,    63,    64,
      65,    66,    67,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    63,    64,    65,    66,    67,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    65,    66,
      67
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      38,    70,    71,    72,    73,    74,    83,    84,    49,    65,
      67,    83,     0,    72,    67,    85,    86,    55,    67,    63,
      64,    65,    39,    67,    48,    60,    62,    43,    50,    56,
      56,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    41,    47,    56,    63,    64,
      65,    66,    67,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   110,    61,    63,    86,    67,
      78,    80,    75,    77,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,   104,   105,
     104,    96,    62,    46,    45,    53,    54,    49,    50,    51,
      52,    40,    41,    42,    43,    44,    48,    56,    60,    61,
      50,    81,    82,    83,    57,    81,    57,   111,    97,   109,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    57,    97,    99,   100,   101,   101,   102,   102,
     102,   102,   103,   103,   104,   104,   104,    97,    57,   109,
      96,    57,    62,    67,    58,    89,    57,    89,   109,    57,
      62,    62,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    61,    79,    82,    60,    87,    76,
      57,    97,    96,    97,    89,    61,    63,    12,    15,    16,
      17,    18,    21,    22,    23,    55,    59,    84,    88,    89,
      90,    91,    94,    95,    96,    89,    57,    61,    56,    56,
      56,    88,    56,    55,    55,    55,    96,    55,    55,    96,
      84,    90,    96,    16,    96,    55,    57,    55,    90,    57,
      56,    57,    89,    90,    57,    96,    88,    96,    58,    13,
      14,    57,    96,    88,    57,    57,    19,    20,    92,    93,
      56,    89,    88,    57,    88,    55,    63,    66,   108,    55,
      59,    93,    96,    88,    55,    87,    57,    87,    89,    14,
      89
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    69,    70,    70,    71,    71,    72,    72,    72,    73,
      73,    73,    73,    73,    73,    75,    76,    74,    77,    74,
      78,    79,    74,    80,    74,    81,    81,    82,    82,    82,
      83,    83,    83,    83,    83,    83,    83,    84,    85,    85,
      86,    86,    86,    86,    87,    87,    88,    88,    88,    88,
      88,    88,    89,    90,    90,    91,    91,    91,    91,    91,
      92,    92,    93,    93,    94,    94,    94,    94,    94,    94,
      95,    95,    95,    95,    96,    96,    97,    97,    98,    98,
      99,    99,   100,   100,   100,   101,   101,   101,   101,   101,
     102,   102,   102,   103,   103,   103,   103,   104,   104,   104,
     105,   105,   105,   105,   106,   106,   106,   106,   106,   107,
     107,   107,   108,   108,   109,   109,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   111,   110,
     110
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     1,     2,     4,
       6,     2,     3,     3,     3,     0,     0,     9,     0,     7,
       0,     0,     9,     0,     7,     1,     3,     2,     4,     5,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     3,
       1,     3,     4,     3,     0,     2,     1,     1,     1,     1,
       1,     2,     3,     1,     2,     5,    10,    12,     7,     7,
       1,     2,     4,     3,     5,     7,     6,     7,     7,     8,
       2,     2,     2,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     1,     2,     2,
       1,     4,     3,     4,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     3,     6,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     0,     5,
       4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 15: /* $@1: %empty  */
#line 543 "pokelang.y"
    {
        register_function((yyvsp[-1].strval), (DataType)(yyvsp[-2].intval), 0);
        enter_function_context((yyvsp[-1].strval), (DataType)(yyvsp[-2].intval));
        free((yyvsp[-1].strval));
    }
#line 1845 "pokelang.tab.c"
    break;

  case 16: /* $@2: %empty  */
#line 549 "pokelang.y"
    {
        int idx = find_function(current_function_name);
        if (idx >= 0) functions[idx].param_count = (yyvsp[-1].intval);
    }
#line 1854 "pokelang.tab.c"
    break;

  case 17: /* function_declaration: FUNCTION type_specifier IDENTIFIER LPAREN $@1 parameter_list RPAREN $@2 compound_statement  */
#line 554 "pokelang.y"
    {
        leave_function_context();
    }
#line 1862 "pokelang.tab.c"
    break;

  case 18: /* $@3: %empty  */
#line 558 "pokelang.y"
    {
        register_function((yyvsp[-1].strval), (DataType)(yyvsp[-2].intval), 0);
        enter_function_context((yyvsp[-1].strval), (DataType)(yyvsp[-2].intval));
        free((yyvsp[-1].strval));
    }
#line 1872 "pokelang.tab.c"
    break;

  case 19: /* function_declaration: FUNCTION type_specifier IDENTIFIER LPAREN $@3 RPAREN compound_statement  */
#line 564 "pokelang.y"
    {
        leave_function_context();
    }
#line 1880 "pokelang.tab.c"
    break;

  case 20: /* $@4: %empty  */
#line 568 "pokelang.y"
    {
        register_function("StartBattle", (DataType)(yyvsp[-2].intval), 0);
        enter_function_context("StartBattle", (DataType)(yyvsp[-2].intval));
    }
#line 1889 "pokelang.tab.c"
    break;

  case 21: /* $@5: %empty  */
#line 573 "pokelang.y"
    {
        int idx = find_function("StartBattle");
        if (idx >= 0) functions[idx].param_count = (yyvsp[-1].intval);
    }
#line 1898 "pokelang.tab.c"
    break;

  case 22: /* function_declaration: FUNCTION type_specifier MAIN LPAREN $@4 parameter_list RPAREN $@5 compound_statement  */
#line 578 "pokelang.y"
    {
        leave_function_context();
    }
#line 1906 "pokelang.tab.c"
    break;

  case 23: /* $@6: %empty  */
#line 582 "pokelang.y"
    {
        register_function("StartBattle", (DataType)(yyvsp[-2].intval), 0);
        enter_function_context("StartBattle", (DataType)(yyvsp[-2].intval));
    }
#line 1915 "pokelang.tab.c"
    break;

  case 24: /* function_declaration: FUNCTION type_specifier MAIN LPAREN $@6 RPAREN compound_statement  */
#line 587 "pokelang.y"
    {
        leave_function_context();
    }
#line 1923 "pokelang.tab.c"
    break;

  case 25: /* parameter_list: parameter  */
#line 593 "pokelang.y"
              { (yyval.intval) = 1; }
#line 1929 "pokelang.tab.c"
    break;

  case 26: /* parameter_list: parameter_list COMMA parameter  */
#line 594 "pokelang.y"
                                     { (yyval.intval) = (yyvsp[-2].intval) + 1; }
#line 1935 "pokelang.tab.c"
    break;

  case 27: /* parameter: type_specifier IDENTIFIER  */
#line 599 "pokelang.y"
    {
        int idx = declare_symbol((yyvsp[0].strval), (DataType)(yyvsp[-1].intval), 0);
        if (idx >= 0) symbols[idx].initialized = 1;
        free((yyvsp[0].strval));
    }
#line 1945 "pokelang.tab.c"
    break;

  case 28: /* parameter: type_specifier IDENTIFIER LBRACKET RBRACKET  */
#line 605 "pokelang.y"
    {
        int idx = declare_symbol((yyvsp[-2].strval), (DataType)(yyvsp[-3].intval), 1);
        if (idx >= 0) symbols[idx].initialized = 1;
        free((yyvsp[-2].strval));
    }
#line 1955 "pokelang.tab.c"
    break;

  case 29: /* parameter: type_specifier IDENTIFIER LBRACKET INTEGER_LITERAL RBRACKET  */
#line 611 "pokelang.y"
    {
        int idx = declare_symbol((yyvsp[-3].strval), (DataType)(yyvsp[-4].intval), 1);
        if (idx >= 0) symbols[idx].initialized = 1;
        free((yyvsp[-3].strval));
    }
#line 1965 "pokelang.tab.c"
    break;

  case 30: /* type_specifier: INT  */
#line 619 "pokelang.y"
           { (yyval.intval) = TYPE_INT; current_decl_type = TYPE_INT; }
#line 1971 "pokelang.tab.c"
    break;

  case 31: /* type_specifier: FLOAT  */
#line 620 "pokelang.y"
             { (yyval.intval) = TYPE_FLOAT; current_decl_type = TYPE_FLOAT; }
#line 1977 "pokelang.tab.c"
    break;

  case 32: /* type_specifier: DOUBLE  */
#line 621 "pokelang.y"
             { (yyval.intval) = TYPE_DOUBLE; current_decl_type = TYPE_DOUBLE; }
#line 1983 "pokelang.tab.c"
    break;

  case 33: /* type_specifier: LONG  */
#line 622 "pokelang.y"
             { (yyval.intval) = TYPE_LONG; current_decl_type = TYPE_LONG; }
#line 1989 "pokelang.tab.c"
    break;

  case 34: /* type_specifier: CHAR  */
#line 623 "pokelang.y"
             { (yyval.intval) = TYPE_CHAR; current_decl_type = TYPE_CHAR; }
#line 1995 "pokelang.tab.c"
    break;

  case 35: /* type_specifier: BOOL  */
#line 624 "pokelang.y"
             { (yyval.intval) = TYPE_BOOL; current_decl_type = TYPE_BOOL; }
#line 2001 "pokelang.tab.c"
    break;

  case 36: /* type_specifier: VOID  */
#line 625 "pokelang.y"
             { (yyval.intval) = TYPE_VOID; current_decl_type = TYPE_VOID; }
#line 2007 "pokelang.tab.c"
    break;

  case 40: /* declarator: IDENTIFIER  */
#line 640 "pokelang.y"
    {
        declare_symbol((yyvsp[0].strval), current_decl_type, 0);
        free((yyvsp[0].strval));
    }
#line 2016 "pokelang.tab.c"
    break;

  case 41: /* declarator: IDENTIFIER ASSIGN expression  */
#line 645 "pokelang.y"
    {
        declare_symbol((yyvsp[-2].strval), current_decl_type, 0);
        assign_symbol((yyvsp[-2].strval), (yyvsp[0].expr));
        free((yyvsp[-2].strval));
    }
#line 2026 "pokelang.tab.c"
    break;

  case 42: /* declarator: IDENTIFIER LBRACKET INTEGER_LITERAL RBRACKET  */
#line 651 "pokelang.y"
    {
        declare_symbol((yyvsp[-3].strval), current_decl_type, 1);
        free((yyvsp[-3].strval));
    }
#line 2035 "pokelang.tab.c"
    break;

  case 43: /* declarator: IDENTIFIER LBRACKET RBRACKET  */
#line 656 "pokelang.y"
    {
        declare_symbol((yyvsp[-2].strval), current_decl_type, 1);
        free((yyvsp[-2].strval));
    }
#line 2044 "pokelang.tab.c"
    break;

  case 55: /* selection_statement: IF LPAREN expression RPAREN compound_statement  */
#line 689 "pokelang.y"
    {
        if ((yyvsp[-2].expr).type == TYPE_STRING) {
            semantic_error("if condition cannot be string");
        }
    }
#line 2054 "pokelang.tab.c"
    break;

  case 56: /* selection_statement: IF LPAREN expression RPAREN compound_statement ELSE_IF LPAREN expression RPAREN compound_statement  */
#line 695 "pokelang.y"
    {
        if ((yyvsp[-7].expr).type == TYPE_STRING || (yyvsp[-2].expr).type == TYPE_STRING) {
            semantic_error("if/else-if condition cannot be string");
        }
    }
#line 2064 "pokelang.tab.c"
    break;

  case 57: /* selection_statement: IF LPAREN expression RPAREN compound_statement ELSE_IF LPAREN expression RPAREN compound_statement ELSE compound_statement  */
#line 701 "pokelang.y"
    {
        if ((yyvsp[-9].expr).type == TYPE_STRING || (yyvsp[-4].expr).type == TYPE_STRING) {
            semantic_error("if/else-if condition cannot be string");
        }
    }
#line 2074 "pokelang.tab.c"
    break;

  case 58: /* selection_statement: IF LPAREN expression RPAREN compound_statement ELSE compound_statement  */
#line 707 "pokelang.y"
    {
        if ((yyvsp[-4].expr).type == TYPE_STRING) {
            semantic_error("if condition cannot be string");
        }
    }
#line 2084 "pokelang.tab.c"
    break;

  case 64: /* iteration_statement: WHILE LPAREN expression RPAREN statement  */
#line 728 "pokelang.y"
    {
        if ((yyvsp[-2].expr).type == TYPE_STRING) {
            semantic_error("while condition cannot be string");
        }
    }
#line 2094 "pokelang.tab.c"
    break;

  case 65: /* iteration_statement: DO statement WHILE LPAREN expression RPAREN SEMICOLON  */
#line 734 "pokelang.y"
    {
        if ((yyvsp[-2].expr).type == TYPE_STRING) {
            semantic_error("do-while condition cannot be string");
        }
    }
#line 2104 "pokelang.tab.c"
    break;

  case 67: /* iteration_statement: FOR LPAREN expression_statement expression_statement expression RPAREN statement  */
#line 741 "pokelang.y"
    {
        if ((yyvsp[-2].expr).type == TYPE_STRING) {
            semantic_error("for condition/update cannot be string where numeric expected");
        }
    }
#line 2114 "pokelang.tab.c"
    break;

  case 69: /* iteration_statement: FOR LPAREN variable_declaration SEMICOLON expression_statement expression RPAREN statement  */
#line 748 "pokelang.y"
    {
        if ((yyvsp[-2].expr).type == TYPE_STRING) {
            semantic_error("for update cannot be string");
        }
    }
#line 2124 "pokelang.tab.c"
    break;

  case 72: /* jump_statement: RETURN SEMICOLON  */
#line 760 "pokelang.y"
    {
        if (current_function_return_type != TYPE_VOID) {
            semantic_error(
                "function '%s' must return %s value",
                current_function_name,
                dtype_name(current_function_return_type)
            );
        }
    }
#line 2138 "pokelang.tab.c"
    break;

  case 73: /* jump_statement: RETURN expression SEMICOLON  */
#line 770 "pokelang.y"
    {
        if (current_function_return_type == TYPE_VOID) {
            if (!(is_numeric_type((yyvsp[-1].expr).type) && (yyvsp[-1].expr).num == 0.0)) {
                semantic_error("void function '%s' cannot return a value", current_function_name);
            }
        } else if (!can_assign(current_function_return_type, (yyvsp[-1].expr).type)) {
            semantic_error(
                "return type mismatch in function '%s': expected %s, got %s",
                current_function_name,
                dtype_name(current_function_return_type),
                dtype_name((yyvsp[-1].expr).type)
            );
        }
    }
#line 2157 "pokelang.tab.c"
    break;

  case 74: /* expression: assignment_expression  */
#line 788 "pokelang.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 2163 "pokelang.tab.c"
    break;

  case 75: /* expression: expression COMMA assignment_expression  */
#line 789 "pokelang.y"
                                             { (yyval.expr) = (yyvsp[0].expr); }
#line 2169 "pokelang.tab.c"
    break;

  case 76: /* assignment_expression: logical_or_expression  */
#line 793 "pokelang.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 2175 "pokelang.tab.c"
    break;

  case 77: /* assignment_expression: postfix_expression ASSIGN assignment_expression  */
#line 795 "pokelang.y"
    {
        if (!(yyvsp[-2].expr).is_lvalue) {
            semantic_error("left side of assignment must be a variable");
            (yyval.expr) = (yyvsp[0].expr);
        } else {
            assign_symbol((yyvsp[-2].expr).ident, (yyvsp[0].expr));
            (yyval.expr) = (yyvsp[0].expr);
        }
    }
#line 2189 "pokelang.tab.c"
    break;

  case 78: /* logical_or_expression: logical_and_expression  */
#line 807 "pokelang.y"
                           { (yyval.expr) = (yyvsp[0].expr); }
#line 2195 "pokelang.tab.c"
    break;

  case 79: /* logical_or_expression: logical_or_expression OR logical_and_expression  */
#line 809 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-2].expr), "team");
        ExprValue b = ensure_numeric((yyvsp[0].expr), "team");
        (yyval.expr) = make_number((a.num != 0.0 || b.num != 0.0) ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2205 "pokelang.tab.c"
    break;

  case 80: /* logical_and_expression: equality_expression  */
#line 817 "pokelang.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 2211 "pokelang.tab.c"
    break;

  case 81: /* logical_and_expression: logical_and_expression AND equality_expression  */
#line 819 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-2].expr), "crit");
        ExprValue b = ensure_numeric((yyvsp[0].expr), "crit");
        (yyval.expr) = make_number((a.num != 0.0 && b.num != 0.0) ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2221 "pokelang.tab.c"
    break;

  case 82: /* equality_expression: relational_expression  */
#line 827 "pokelang.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 2227 "pokelang.tab.c"
    break;

  case 83: /* equality_expression: equality_expression EQUAL relational_expression  */
#line 829 "pokelang.y"
    {
        (yyval.expr) = make_number((yyvsp[-2].expr).num == (yyvsp[0].expr).num ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2235 "pokelang.tab.c"
    break;

  case 84: /* equality_expression: equality_expression NOT_EQUAL relational_expression  */
#line 833 "pokelang.y"
    {
        (yyval.expr) = make_number((yyvsp[-2].expr).num != (yyvsp[0].expr).num ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2243 "pokelang.tab.c"
    break;

  case 85: /* relational_expression: additive_expression  */
#line 839 "pokelang.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 2249 "pokelang.tab.c"
    break;

  case 86: /* relational_expression: relational_expression LESS_THAN additive_expression  */
#line 841 "pokelang.y"
    {
        (yyval.expr) = make_number((yyvsp[-2].expr).num < (yyvsp[0].expr).num ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2257 "pokelang.tab.c"
    break;

  case 87: /* relational_expression: relational_expression GREATER_THAN additive_expression  */
#line 845 "pokelang.y"
    {
        (yyval.expr) = make_number((yyvsp[-2].expr).num > (yyvsp[0].expr).num ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2265 "pokelang.tab.c"
    break;

  case 88: /* relational_expression: relational_expression LESS_EQUAL additive_expression  */
#line 849 "pokelang.y"
    {
        (yyval.expr) = make_number((yyvsp[-2].expr).num <= (yyvsp[0].expr).num ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2273 "pokelang.tab.c"
    break;

  case 89: /* relational_expression: relational_expression GREATER_EQUAL additive_expression  */
#line 853 "pokelang.y"
    {
        (yyval.expr) = make_number((yyvsp[-2].expr).num >= (yyvsp[0].expr).num ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2281 "pokelang.tab.c"
    break;

  case 90: /* additive_expression: multiplicative_expression  */
#line 859 "pokelang.y"
                              { (yyval.expr) = (yyvsp[0].expr); }
#line 2287 "pokelang.tab.c"
    break;

  case 91: /* additive_expression: additive_expression PLUS multiplicative_expression  */
#line 861 "pokelang.y"
    {
        (yyval.expr) = eval_binary_numeric((yyvsp[-2].expr), (yyvsp[0].expr), "atk", '+');
    }
#line 2295 "pokelang.tab.c"
    break;

  case 92: /* additive_expression: additive_expression MINUS multiplicative_expression  */
#line 865 "pokelang.y"
    {
        (yyval.expr) = eval_binary_numeric((yyvsp[-2].expr), (yyvsp[0].expr), "dmg", '-');
    }
#line 2303 "pokelang.tab.c"
    break;

  case 93: /* multiplicative_expression: unary_expression  */
#line 871 "pokelang.y"
                     { (yyval.expr) = (yyvsp[0].expr); }
#line 2309 "pokelang.tab.c"
    break;

  case 94: /* multiplicative_expression: multiplicative_expression MULTIPLY unary_expression  */
#line 873 "pokelang.y"
    {
        (yyval.expr) = eval_binary_numeric((yyvsp[-2].expr), (yyvsp[0].expr), "x", '*');
    }
#line 2317 "pokelang.tab.c"
    break;

  case 95: /* multiplicative_expression: multiplicative_expression DIVIDE unary_expression  */
#line 877 "pokelang.y"
    {
        (yyval.expr) = eval_binary_numeric((yyvsp[-2].expr), (yyvsp[0].expr), "hp", '/');
    }
#line 2325 "pokelang.tab.c"
    break;

  case 96: /* multiplicative_expression: multiplicative_expression MODULO unary_expression  */
#line 881 "pokelang.y"
    {
        (yyval.expr) = eval_binary_numeric((yyvsp[-2].expr), (yyvsp[0].expr), "mod", '%');
    }
#line 2333 "pokelang.tab.c"
    break;

  case 97: /* unary_expression: postfix_expression  */
#line 887 "pokelang.y"
                       { (yyval.expr) = (yyvsp[0].expr); }
#line 2339 "pokelang.tab.c"
    break;

  case 98: /* unary_expression: MINUS unary_expression  */
#line 889 "pokelang.y"
    {
        ExprValue v = ensure_numeric((yyvsp[0].expr), "unary minus");
        (yyval.expr) = make_number(-v.num, v.type);
    }
#line 2348 "pokelang.tab.c"
    break;

  case 99: /* unary_expression: NOT unary_expression  */
#line 894 "pokelang.y"
    {
        ExprValue v = ensure_numeric((yyvsp[0].expr), "confuse");
        (yyval.expr) = make_number(v.num == 0.0 ? 1.0 : 0.0, TYPE_BOOL);
    }
#line 2357 "pokelang.tab.c"
    break;

  case 100: /* postfix_expression: primary_expression  */
#line 901 "pokelang.y"
                       { (yyval.expr) = (yyvsp[0].expr); }
#line 2363 "pokelang.tab.c"
    break;

  case 101: /* postfix_expression: postfix_expression LBRACKET expression RBRACKET  */
#line 903 "pokelang.y"
    {
        if (!(yyvsp[-3].expr).is_lvalue) {
            semantic_error("array indexing requires variable identifier");
            (yyval.expr) = make_number(0.0, TYPE_UNKNOWN);
        } else {
            int idx = find_symbol((yyvsp[-3].expr).ident);
            if (idx >= 0 && !symbols[idx].is_array) {
                semantic_warning("variable '%s' used with array indexing but not declared as array", (yyvsp[-3].expr).ident);
            }
            if (idx >= 0) {
                (yyval.expr) = make_number(symbols[idx].numeric_value, symbols[idx].type);
            } else {
                (yyval.expr) = make_number(0.0, TYPE_UNKNOWN);
            }
            (yyval.expr).is_lvalue = 1;
            snprintf((yyval.expr).ident, sizeof((yyval.expr).ident), "%s", (yyvsp[-3].expr).ident);
        }
    }
#line 2386 "pokelang.tab.c"
    break;

  case 102: /* postfix_expression: postfix_expression LPAREN RPAREN  */
#line 922 "pokelang.y"
    {
        if (!(yyvsp[-2].expr).is_lvalue) {
            semantic_error("function call requires function identifier");
            (yyval.expr) = make_number(0.0, TYPE_UNKNOWN);
        } else {
            (yyval.expr) = evaluate_function_call((yyvsp[-2].expr).ident, 0);
        }
    }
#line 2399 "pokelang.tab.c"
    break;

  case 103: /* postfix_expression: postfix_expression LPAREN argument_list RPAREN  */
#line 931 "pokelang.y"
    {
        if (!(yyvsp[-3].expr).is_lvalue) {
            semantic_error("function call requires function identifier");
            (yyval.expr) = make_number(0.0, TYPE_UNKNOWN);
        } else {
            (yyval.expr) = evaluate_function_call((yyvsp[-3].expr).ident, (yyvsp[-1].intval));
        }
    }
#line 2412 "pokelang.tab.c"
    break;

  case 104: /* primary_expression: IDENTIFIER  */
#line 943 "pokelang.y"
    {
        if (find_symbol((yyvsp[0].strval)) >= 0) {
            (yyval.expr) = read_symbol((yyvsp[0].strval));
        } else if (find_function((yyvsp[0].strval)) >= 0) {
            (yyval.expr) = make_number(0.0, TYPE_UNKNOWN);
            (yyval.expr).is_lvalue = 1;
            snprintf((yyval.expr).ident, sizeof((yyval.expr).ident), "%s", (yyvsp[0].strval));
        } else {
            semantic_error("use of undeclared variable '%s'", (yyvsp[0].strval));
            (yyval.expr) = make_number(0.0, TYPE_UNKNOWN);
        }
        free((yyvsp[0].strval));
    }
#line 2430 "pokelang.tab.c"
    break;

  case 105: /* primary_expression: constant  */
#line 956 "pokelang.y"
               { (yyval.expr) = (yyvsp[0].expr); }
#line 2436 "pokelang.tab.c"
    break;

  case 106: /* primary_expression: STRING_LITERAL  */
#line 958 "pokelang.y"
    {
        (yyval.expr) = make_string((yyvsp[0].strval));
        free((yyvsp[0].strval));
    }
#line 2445 "pokelang.tab.c"
    break;

  case 107: /* primary_expression: LPAREN expression RPAREN  */
#line 962 "pokelang.y"
                               { (yyval.expr) = (yyvsp[-1].expr); }
#line 2451 "pokelang.tab.c"
    break;

  case 108: /* primary_expression: math_function  */
#line 963 "pokelang.y"
                    { (yyval.expr) = (yyvsp[0].expr); }
#line 2457 "pokelang.tab.c"
    break;

  case 109: /* constant: INTEGER_LITERAL  */
#line 967 "pokelang.y"
                    { (yyval.expr) = make_number((double)(yyvsp[0].intval), TYPE_INT); }
#line 2463 "pokelang.tab.c"
    break;

  case 110: /* constant: FLOAT_LITERAL  */
#line 968 "pokelang.y"
                    { (yyval.expr) = make_number((yyvsp[0].floatval), TYPE_FLOAT); }
#line 2469 "pokelang.tab.c"
    break;

  case 111: /* constant: CHAR_LITERAL  */
#line 969 "pokelang.y"
                   { (yyval.expr) = make_number((double)(yyvsp[0].charval), TYPE_CHAR); }
#line 2475 "pokelang.tab.c"
    break;

  case 114: /* argument_list: assignment_expression  */
#line 979 "pokelang.y"
    {
        if (collect_print_args) append_print_arg((yyvsp[0].expr));
        (yyval.intval) = 1;
    }
#line 2484 "pokelang.tab.c"
    break;

  case 115: /* argument_list: argument_list COMMA assignment_expression  */
#line 984 "pokelang.y"
    {
        if (collect_print_args) append_print_arg((yyvsp[0].expr));
        (yyval.intval) = (yyvsp[-2].intval) + 1;
    }
#line 2493 "pokelang.tab.c"
    break;

  case 116: /* math_function: POW LPAREN expression COMMA expression RPAREN  */
#line 993 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-3].expr), "powerup");
        ExprValue b = ensure_numeric((yyvsp[-1].expr), "powerup");
        (yyval.expr) = make_number(pow(a.num, b.num), TYPE_DOUBLE);
    }
#line 2503 "pokelang.tab.c"
    break;

  case 117: /* math_function: SQRT LPAREN expression RPAREN  */
#line 999 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "rootchu");
        if (a.num < 0.0) semantic_error("sqrt of negative value");
        (yyval.expr) = make_number(sqrt(fabs(a.num)), TYPE_DOUBLE);
    }
#line 2513 "pokelang.tab.c"
    break;

  case 118: /* math_function: FLOOR LPAREN expression RPAREN  */
#line 1005 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "floorchu");
        (yyval.expr) = make_number(floor(a.num), TYPE_DOUBLE);
    }
#line 2522 "pokelang.tab.c"
    break;

  case 119: /* math_function: CEIL LPAREN expression RPAREN  */
#line 1010 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "ceilchu");
        (yyval.expr) = make_number(ceil(a.num), TYPE_DOUBLE);
    }
#line 2531 "pokelang.tab.c"
    break;

  case 120: /* math_function: ABS LPAREN expression RPAREN  */
#line 1015 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "heal");
        (yyval.expr) = make_number(fabs(a.num), TYPE_DOUBLE);
    }
#line 2540 "pokelang.tab.c"
    break;

  case 121: /* math_function: SIN LPAREN expression RPAREN  */
#line 1020 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "sinchu");
        (yyval.expr) = make_number(sin(a.num), TYPE_DOUBLE);
    }
#line 2549 "pokelang.tab.c"
    break;

  case 122: /* math_function: COS LPAREN expression RPAREN  */
#line 1025 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "coschu");
        (yyval.expr) = make_number(cos(a.num), TYPE_DOUBLE);
    }
#line 2558 "pokelang.tab.c"
    break;

  case 123: /* math_function: TAN LPAREN expression RPAREN  */
#line 1030 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "tanchu");
        (yyval.expr) = make_number(tan(a.num), TYPE_DOUBLE);
    }
#line 2567 "pokelang.tab.c"
    break;

  case 124: /* math_function: ASIN LPAREN expression RPAREN  */
#line 1035 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "asinchu");
        (yyval.expr) = make_number(asin(a.num), TYPE_DOUBLE);
    }
#line 2576 "pokelang.tab.c"
    break;

  case 125: /* math_function: ACOS LPAREN expression RPAREN  */
#line 1040 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "acoschu");
        (yyval.expr) = make_number(acos(a.num), TYPE_DOUBLE);
    }
#line 2585 "pokelang.tab.c"
    break;

  case 126: /* math_function: ATAN LPAREN expression RPAREN  */
#line 1045 "pokelang.y"
    {
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "atanchu");
        (yyval.expr) = make_number(atan(a.num), TYPE_DOUBLE);
    }
#line 2594 "pokelang.tab.c"
    break;

  case 127: /* math_function: PRIME LPAREN expression RPAREN  */
#line 1050 "pokelang.y"
    {
        int i;
        int n;
        ExprValue a = ensure_numeric((yyvsp[-1].expr), "primechu");
        n = (int)a.num;
        if (n < 2) {
            (yyval.expr) = make_number(0.0, TYPE_BOOL);
        } else {
            (yyval.expr) = make_number(1.0, TYPE_BOOL);
            for (i = 2; i * i <= n; ++i) {
                if (n % i == 0) {
                    (yyval.expr).num = 0.0;
                    break;
                }
            }
        }
    }
#line 2616 "pokelang.tab.c"
    break;

  case 128: /* $@7: %empty  */
#line 1068 "pokelang.y"
    {
        begin_print_args();
    }
#line 2624 "pokelang.tab.c"
    break;

  case 129: /* math_function: PRINT LPAREN $@7 argument_list RPAREN  */
#line 1072 "pokelang.y"
    {
        end_print_args();
        (yyval.expr) = make_number(0.0, TYPE_INT);
    }
#line 2633 "pokelang.tab.c"
    break;

  case 130: /* math_function: SCAN LPAREN argument_list RPAREN  */
#line 1077 "pokelang.y"
    {
        semantic_warning("pokecatch runtime input is parsed but not executed in this build");
        (yyval.expr) = make_number(0.0, TYPE_INT);
    }
#line 2642 "pokelang.tab.c"
    break;


#line 2646 "pokelang.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1083 "pokelang.y"


void yyerror(const char* s) {
    fprintf(stderr, "Syntax error at line %d: %s\n", line_number, s);
    syntax_error_count++;
}

int main(int argc, char* argv[]) {
    int result;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.poke>\n", argv[0]);
        return 1;
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

    result = yyparse();

    printf("\n=== Compilation Result ===\n");
    if (result == 0 && syntax_error_count == 0 && semantic_error_count == 0) {
        printf("[SUCCESS] Parsing completed successfully.\n");
        printf("[SUCCESS] Semantic checks passed.\n");
        printf("[INFO] Semantic warnings: %d\n", semantic_warning_count);
        if (ir_out) {
            printf("[INFO] Intermediate code generated: intermediate_code.ir\n");
        }
    } else {
        printf("[FAILED] Compilation failed.\n");
        printf("[FAILED] Syntax errors: %d\n", syntax_error_count);
        printf("[FAILED] Semantic errors: %d\n", semantic_error_count);
        printf("[INFO] Semantic warnings: %d\n", semantic_warning_count);
    }

    if (yyin) fclose(yyin);
    if (ir_out) fclose(ir_out);

    return (result == 0 && syntax_error_count == 0 && semantic_error_count == 0) ? 0 : 1;
}
