%{
/*
 * PokémonLang Lexical Analyzer (For Bison Integration)
 * File: pokelang_parser.l
 * Purpose: Tokenize PokémonLang source code for Bison parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pokelang.tab.h"

int line_number = 1;
extern int explain_trace_mode;
extern int indentation_error_count;

int brace_depth = 0;
int at_line_start = 1;
int current_indent = 0;

void check_indentation_and_update(int tok, const char* token_name, const char* lexeme) {
    if (at_line_start) {
        /* Simple rule: inside blocks, non-CloseBall statements must be indented by at least 4 spaces per depth. */
        if (brace_depth > 0 && tok != RBRACE) {
            int required = brace_depth * 4;
            if (current_indent < required) {
                fprintf(
                    stderr,
                    "Indentation error at line %d: token %s ('%s') has indent %d, expected at least %d. Fix: add at least %d leading spaces before this statement.\n",
                    line_number,
                    token_name,
                    lexeme,
                    current_indent,
                    required,
                    required
                );
                indentation_error_count++;
            }
        }
        at_line_start = 0;
    }

    if (tok == LBRACE) {
        brace_depth++;
    } else if (tok == RBRACE && brace_depth > 0) {
        brace_depth--;
    }
}

void trace_token(const char* token_name, const char* lexeme) {
    if (!explain_trace_mode) return;
    printf("[TRACE][TOKEN] line=%d token=%s lexeme='%s'\n", line_number, token_name, lexeme);
}

#define RETURN_TOKEN(tok) do { check_indentation_and_update(tok, #tok, yytext); trace_token(#tok, yytext); return tok; } while (0)
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
"ProfessorOak"                  { RETURN_TOKEN(INCLUDE); }
"PokeDefine"                    { RETURN_TOKEN(DEFINE); }

    /* ========== DATA TYPES ========== */
"Pikachu"                       { RETURN_TOKEN(INT); }
"Charizard"                     { RETURN_TOKEN(FLOAT); }
"Mewtwo"                        { RETURN_TOKEN(DOUBLE); }
"Snorlax"                       { RETURN_TOKEN(LONG); }
"Eevee"                         { RETURN_TOKEN(CHAR); }
"Togepi"                        { RETURN_TOKEN(BOOL); }
"EmptyBall"                     { RETURN_TOKEN(VOID); }

    /* ========== CONTROL FLOW STATEMENTS ========== */
"Misty"                         { RETURN_TOKEN(ELSE_IF); }
"Brock"                         { RETURN_TOKEN(IF); }
"Jessie"                        { RETURN_TOKEN(ELSE); }
"AshTrain"                      { RETURN_TOKEN(FOR); }
"WildBattle"                    { RETURN_TOKEN(WHILE); }
"RetryBattle"                   { RETURN_TOKEN(DO); }
"HandleGym"                     { RETURN_TOKEN(SWITCH); }
"TypeBadge"                     { RETURN_TOKEN(CASE); }
"DefaultBadge"                  { RETURN_TOKEN(DEFAULT); }

    /* ========== JUMP STATEMENTS ========== */
"RunAway"                       { RETURN_TOKEN(BREAK); }
"KeepFighting"                  { RETURN_TOKEN(CONTINUE); }
"back"                          { RETURN_TOKEN(RETURN); }

    /* ========== INPUT/OUTPUT FUNCTIONS ========== */
"pikapika"                      { RETURN_TOKEN(PRINT); }
"pokecatch"                     { RETURN_TOKEN(SCAN); }

    /* ========== MATHEMATICAL FUNCTIONS ========== */
"powerup"                       { RETURN_TOKEN(POW); }
"rootchu"                       { RETURN_TOKEN(SQRT); }
"floorchu"                      { RETURN_TOKEN(FLOOR); }
"ceilchu"                       { RETURN_TOKEN(CEIL); }
"heal"                          { RETURN_TOKEN(ABS); }
"sinchu"                        { RETURN_TOKEN(SIN); }
"coschu"                        { RETURN_TOKEN(COS); }
"tanchu"                        { RETURN_TOKEN(TAN); }
"asinchu"                       { RETURN_TOKEN(ASIN); }
"acoschu"                       { RETURN_TOKEN(ACOS); }
"atanchu"                       { RETURN_TOKEN(ATAN); }
"primechu"                      { RETURN_TOKEN(PRIME); }

    /* ========== SPECIAL FUNCTIONS ========== */
"Move"                          { RETURN_TOKEN(FUNCTION); }
"StartBattle"                   { RETURN_TOKEN(MAIN); }

    /* ========== MULTI-CHARACTER OPERATORS (Before single-char) ========== */
"<="                            { RETURN_TOKEN(LESS_EQUAL); }
">="                            { RETURN_TOKEN(GREATER_EQUAL); }
"=="                            { RETURN_TOKEN(EQUAL); }
"!="                            { RETURN_TOKEN(NOT_EQUAL); }

    /* ========== KEYWORD OPERATORS ========== */
"atk"                           { RETURN_TOKEN(PLUS); }
"dmg"                           { RETURN_TOKEN(MINUS); }
"crit"                          { RETURN_TOKEN(AND); }
"team"                          { RETURN_TOKEN(OR); }
"confuse"                       { RETURN_TOKEN(NOT); }
"evo"                           { RETURN_TOKEN(ASSIGN); }
"mod"                           { RETURN_TOKEN(MODULO); }
"x"                             { RETURN_TOKEN(MULTIPLY); }
"hp"                            { RETURN_TOKEN(DIVIDE); }

    /* ========== DELIMITERS ========== */
"dot"                           { RETURN_TOKEN(SEMICOLON); }
"fob"                           { RETURN_TOKEN(LPAREN); }
"fcb"                           { RETURN_TOKEN(RPAREN); }
"OpenBall"                      { RETURN_TOKEN(LBRACE); }
"CloseBall"                     { RETURN_TOKEN(RBRACE); }

    /* ========== SINGLE CHARACTER OPERATORS & DELIMITERS ========== */
"<"                             { RETURN_TOKEN(LESS_THAN); }
">"                             { RETURN_TOKEN(GREATER_THAN); }
"["                             { RETURN_TOKEN(LBRACKET); }
"]"                             { RETURN_TOKEN(RBRACKET); }
","                             { RETURN_TOKEN(COMMA); }

    /* ========== LITERALS ========== */
{FLOAT}                         { yylval.floatval = atof(yytext); RETURN_TOKEN(FLOAT_LITERAL); }
{INTEGER}                       { yylval.intval = atoi(yytext); RETURN_TOKEN(INTEGER_LITERAL); }

\"([^\"\n\\]|\\.)*\"           { 
                                  /* String literal - remove quotes */
                                  yylval.strval = (char*)malloc(strlen(yytext) - 1);
                                  strncpy(yylval.strval, yytext + 1, strlen(yytext) - 2);
                                  yylval.strval[strlen(yytext) - 2] = '\0';
                                                                    RETURN_TOKEN(STRING_LITERAL);
                                }

\"([^\"\n\\]|\\.)*              { 
                                  fprintf(stderr, "Error: Unterminated string at line %d\n", line_number);
                                }

'([^'\n\\]|\\.)'                { 
                                  /* Character literal */
                                  if (yytext[1] == '\\') {
                                      /* Handle escape sequences */
                                      switch(yytext[2]) {
                                          case 'n': yylval.charval = '\n'; break;
                                          case 't': yylval.charval = '\t'; break;
                                          case 'r': yylval.charval = '\r'; break;
                                          case '0': yylval.charval = '\0'; break;
                                          case '\\': yylval.charval = '\\'; break;
                                          case '\'': yylval.charval = '\''; break;
                                          default: yylval.charval = yytext[2];
                                      }
                                  } else {
                                      yylval.charval = yytext[1];
                                  }
                                                                    RETURN_TOKEN(CHAR_LITERAL);
                                }

'([^'\n\\]|\\.)*                { 
                                  fprintf(stderr, "Error: Unterminated character literal at line %d\n", line_number);
                                }

    /* ========== IDENTIFIERS (Lower Priority) ========== */
{IDENTIFIER}                    { 
                                  yylval.strval = strdup(yytext);
                                                                    RETURN_TOKEN(IDENTIFIER);
                                }

    /* ========== WHITESPACE & NEWLINE ========== */
{WHITESPACE}                    {
                                  if (at_line_start) {
                                      int i;
                                      for (i = 0; yytext[i] != '\0'; ++i) {
                                          if (yytext[i] == ' ') current_indent += 1;
                                          else if (yytext[i] == '\t') current_indent += 4;
                                      }
                                  }
                                }
{NEWLINE}                       { line_number++; at_line_start = 1; current_indent = 0; }

    /* ========== ERROR HANDLING ========== */
.                               { 
                                  fprintf(stderr, "Error: Invalid character '%s' at line %d\n", yytext, line_number);
                                }

%%
