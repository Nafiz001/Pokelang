%{
/*
 * PokémonLang Lexical Analyzer
 * File: pokelang.l
 * Purpose: Tokenize PokémonLang source code for compilation
 * 
 * Build Instructions:
 *   1. Generate lexer: flex -o lex.yy.cpp pokelang.l
 *   2. Compile: g++ lex.yy.cpp -o pokelang_lexer
 *   3. Run: ./pokelang_lexer input.poke output.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int line_number = 1;

/* Function to print tokens in required format */
void print_token(const char* token_name, const char* lexeme, int line) {
    fprintf(yyout, "<%s, \"%s\", %d>\n", token_name, lexeme, line);
}
%}

%option noyywrap

/* Regular expression definitions */
DIGIT           [0-9]
INTEGER         {DIGIT}+
FLOAT           {DIGIT}+\.{DIGIT}+
LETTER          [A-Za-z]
IDENTIFIER      [A-Za-z_][A-Za-z0-9_]*
WHITESPACE      [ \t\r]+
NEWLINE         \n

%%

    /* ========== COMMENTS (Highest Priority) ========== */
"$$".*                          { /* Single-line comment - ignore */ }
"$*"([^*]|\*+[^*$])*"*$"       { /* Multi-line comment - ignore */ 
                                  for(int i = 0; yytext[i] != '\0'; i++) {
                                      if(yytext[i] == '\n') line_number++;
                                  }
                                }

    /* ========== PREPROCESSOR DIRECTIVES ========== */
"ProfessorOak"                  { print_token("INCLUDE", yytext, line_number); }
"PokeDefine"                    { print_token("DEFINE", yytext, line_number); }

    /* ========== DATA TYPES ========== */
"Pikachu"                       { print_token("INT", yytext, line_number); }
"Charizard"                     { print_token("FLOAT", yytext, line_number); }
"Mewtwo"                        { print_token("DOUBLE", yytext, line_number); }
"Snorlax"                       { print_token("LONG", yytext, line_number); }
"Eevee"                         { print_token("CHAR", yytext, line_number); }
"Togepi"                        { print_token("BOOL", yytext, line_number); }
"EmptyBall"                     { print_token("VOID", yytext, line_number); }

    /* ========== CONTROL FLOW STATEMENTS ========== */
"Misty"                         { print_token("ELSE_IF", yytext, line_number); }
"Brock"                         { print_token("IF", yytext, line_number); }
"Jessie"                        { print_token("ELSE", yytext, line_number); }
"AshTrain"                      { print_token("FOR", yytext, line_number); }
"WildBattle"                    { print_token("WHILE", yytext, line_number); }
"RetryBattle"                   { print_token("DO", yytext, line_number); }
"HandleGym"                     { print_token("SWITCH", yytext, line_number); }
"TypeBadge"                     { print_token("CASE", yytext, line_number); }
"DefaultBadge"                  { print_token("DEFAULT", yytext, line_number); }

    /* ========== JUMP STATEMENTS ========== */
"RunAway"                       { print_token("BREAK", yytext, line_number); }
"KeepFighting"                  { print_token("CONTINUE", yytext, line_number); }
"back"                          { print_token("RETURN", yytext, line_number); }

    /* ========== INPUT/OUTPUT FUNCTIONS ========== */
"pikapika"                      { print_token("PRINT", yytext, line_number); }
"pokecatch"                     { print_token("SCAN", yytext, line_number); }

    /* ========== MATHEMATICAL FUNCTIONS ========== */
"powerup"                       { print_token("POW", yytext, line_number); }
"rootchu"                       { print_token("SQRT", yytext, line_number); }
"floorchu"                      { print_token("FLOOR", yytext, line_number); }
"ceilchu"                       { print_token("CEIL", yytext, line_number); }
"heal"                          { print_token("ABS", yytext, line_number); }
"sinchu"                        { print_token("SIN", yytext, line_number); }
"coschu"                        { print_token("COS", yytext, line_number); }
"tanchu"                        { print_token("TAN", yytext, line_number); }
"asinchu"                       { print_token("ASIN", yytext, line_number); }
"acoschu"                       { print_token("ACOS", yytext, line_number); }
"atanchu"                       { print_token("ATAN", yytext, line_number); }
"primechu"                      { print_token("PRIME", yytext, line_number); }

    /* ========== SPECIAL FUNCTIONS ========== */
"Move"                          { print_token("FUNCTION", yytext, line_number); }
"StartBattle"                   { print_token("MAIN", yytext, line_number); }

    /* ========== MULTI-CHARACTER OPERATORS (Before single-char) ========== */
"<="                            { print_token("LESS_EQUAL", yytext, line_number); }
">="                            { print_token("GREATER_EQUAL", yytext, line_number); }
"=="                            { print_token("EQUAL", yytext, line_number); }
"!="                            { print_token("NOT_EQUAL", yytext, line_number); }

    /* ========== KEYWORD OPERATORS (Before 'x' identifier) ========== */
"atk"                           { print_token("PLUS", yytext, line_number); }
"dmg"                           { print_token("MINUS", yytext, line_number); }
"crit"                          { print_token("AND", yytext, line_number); }
"team"                          { print_token("OR", yytext, line_number); }
"confuse"                       { print_token("NOT", yytext, line_number); }
"evo"                           { print_token("ASSIGN", yytext, line_number); }
"mod"                           { print_token("MODULO", yytext, line_number); }
"x"                             { print_token("MULTIPLY", yytext, line_number); }
"hp"                            { print_token("DIVIDE", yytext, line_number); }

    /* ========== DELIMITERS ========== */
"dot"                           { print_token("SEMICOLON", yytext, line_number); }
"fob"                           { print_token("LPAREN", yytext, line_number); }
"fcb"                           { print_token("RPAREN", yytext, line_number); }
"OpenBall"                      { print_token("LBRACE", yytext, line_number); }
"CloseBall"                     { print_token("RBRACE", yytext, line_number); }

    /* ========== SINGLE CHARACTER OPERATORS & DELIMITERS ========== */
"<"                             { print_token("LESS_THAN", yytext, line_number); }
">"                             { print_token("GREATER_THAN", yytext, line_number); }
"["                             { print_token("LBRACKET", yytext, line_number); }
"]"                             { print_token("RBRACKET", yytext, line_number); }
","                             { print_token("COMMA", yytext, line_number); }

    /* ========== LITERALS ========== */
{FLOAT}                         { print_token("FLOAT_LITERAL", yytext, line_number); }
{INTEGER}                       { print_token("INTEGER_LITERAL", yytext, line_number); }

\"([^\"\n\\]|\\.)*\"           { 
                                  /* String literal - remove quotes from lexeme */
                                  char* str_content = (char*)malloc(strlen(yytext) - 1);
                                  strncpy(str_content, yytext + 1, strlen(yytext) - 2);
                                  str_content[strlen(yytext) - 2] = '\0';
                                  print_token("STRING_LITERAL", str_content, line_number);
                                  free(str_content);
                                }

\"([^\"\n\\]|\\.)*              { 
                                  fprintf(stderr, "Error: Unterminated string at line %d\n", line_number);
                                }

'([^'\n\\]|\\.)'{1}             { 
                                  /* Character literal - show content without quotes */
                                  char char_content[3];
                                  strncpy(char_content, yytext + 1, strlen(yytext) - 2);
                                  char_content[strlen(yytext) - 2] = '\0';
                                  print_token("CHAR_LITERAL", char_content, line_number);
                                }

'([^'\n\\]|\\.)*                { 
                                  fprintf(stderr, "Error: Unterminated character literal at line %d\n", line_number);
                                }

    /* ========== IDENTIFIERS (Lower Priority) ========== */
{IDENTIFIER}                    { print_token("IDENTIFIER", yytext, line_number); }

    /* ========== WHITESPACE & NEWLINE ========== */
{WHITESPACE}                    { /* Ignore whitespace */ }
{NEWLINE}                       { line_number++; }

    /* ========== ERROR HANDLING ========== */
.                               { 
                                  fprintf(stderr, "Error: Invalid character '%s' at line %d\n", yytext, line_number);
                                }

%%

/*
 * Main function - handles file I/O and invokes lexical analyzer
 */
int main(int argc, char* argv[]) {
    /* Handle command-line arguments */
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: Cannot open input file '%s'\n", argv[1]);
            return 1;
        }
    } else {
        yyin = stdin;
    }

    if (argc > 2) {
        yyout = fopen(argv[2], "w");
        if (!yyout) {
            fprintf(stderr, "Error: Cannot open output file '%s'\n", argv[2]);
            if (yyin != stdin) fclose(yyin);
            return 1;
        }
    } else {
        yyout = stdout;
    }

    /* Print header */
    fprintf(yyout, "=== PokémonLang Lexical Analysis ===\n");
    fprintf(yyout, "Format: <TOKEN_NAME, \"lexeme\", line_number>\n\n");

    /* Run lexical analyzer */
    yylex();

    /* Print footer */
    fprintf(yyout, "\n=== Lexical Analysis Complete ===\n");
    fprintf(yyout, "Total lines processed: %d\n", line_number - 1);

    /* Close files */
    if (yyin != stdin) fclose(yyin);
    if (yyout != stdout) fclose(yyout);

    return 0;
}
