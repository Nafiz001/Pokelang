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
"ProfessorOak"                  { return INCLUDE; }
"PokeDefine"                    { return DEFINE; }

    /* ========== DATA TYPES ========== */
"Pikachu"                       { return INT; }
"Charizard"                     { return FLOAT; }
"Mewtwo"                        { return DOUBLE; }
"Snorlax"                       { return LONG; }
"Eevee"                         { return CHAR; }
"Togepi"                        { return BOOL; }
"EmptyBall"                     { return VOID; }

    /* ========== CONTROL FLOW STATEMENTS ========== */
"Misty"                         { return ELSE_IF; }
"Brock"                         { return IF; }
"Jessie"                        { return ELSE; }
"AshTrain"                      { return FOR; }
"WildBattle"                    { return WHILE; }
"RetryBattle"                   { return DO; }
"HandleGym"                     { return SWITCH; }
"TypeBadge"                     { return CASE; }
"DefaultBadge"                  { return DEFAULT; }

    /* ========== JUMP STATEMENTS ========== */
"RunAway"                       { return BREAK; }
"KeepFighting"                  { return CONTINUE; }
"back"                          { return RETURN; }

    /* ========== INPUT/OUTPUT FUNCTIONS ========== */
"pikapika"                      { return PRINT; }
"pokecatch"                     { return SCAN; }

    /* ========== MATHEMATICAL FUNCTIONS ========== */
"powerup"                       { return POW; }
"rootchu"                       { return SQRT; }
"floorchu"                      { return FLOOR; }
"ceilchu"                       { return CEIL; }
"heal"                          { return ABS; }
"sinchu"                        { return SIN; }
"coschu"                        { return COS; }
"tanchu"                        { return TAN; }
"asinchu"                       { return ASIN; }
"acoschu"                       { return ACOS; }
"atanchu"                       { return ATAN; }
"primechu"                      { return PRIME; }

    /* ========== SPECIAL FUNCTIONS ========== */
"Move"                          { return FUNCTION; }
"StartBattle"                   { return MAIN; }

    /* ========== MULTI-CHARACTER OPERATORS (Before single-char) ========== */
"<="                            { return LESS_EQUAL; }
">="                            { return GREATER_EQUAL; }
"=="                            { return EQUAL; }
"!="                            { return NOT_EQUAL; }

    /* ========== KEYWORD OPERATORS ========== */
"atk"                           { return PLUS; }
"dmg"                           { return MINUS; }
"crit"                          { return AND; }
"team"                          { return OR; }
"confuse"                       { return NOT; }
"evo"                           { return ASSIGN; }
"mod"                           { return MODULO; }
"x"                             { return MULTIPLY; }
"hp"                            { return DIVIDE; }

    /* ========== DELIMITERS ========== */
"dot"                           { return SEMICOLON; }
"fob"                           { return LPAREN; }
"fcb"                           { return RPAREN; }
"OpenBall"                      { return LBRACE; }
"CloseBall"                     { return RBRACE; }

    /* ========== SINGLE CHARACTER OPERATORS & DELIMITERS ========== */
"<"                             { return LESS_THAN; }
">"                             { return GREATER_THAN; }
"["                             { return LBRACKET; }
"]"                             { return RBRACKET; }
","                             { return COMMA; }

    /* ========== LITERALS ========== */
{FLOAT}                         { yylval.floatval = atof(yytext); return FLOAT_LITERAL; }
{INTEGER}                       { yylval.intval = atoi(yytext); return INTEGER_LITERAL; }

\"([^\"\n\\]|\\.)*\"           { 
                                  /* String literal - remove quotes */
                                  yylval.strval = (char*)malloc(strlen(yytext) - 1);
                                  strncpy(yylval.strval, yytext + 1, strlen(yytext) - 2);
                                  yylval.strval[strlen(yytext) - 2] = '\0';
                                  return STRING_LITERAL;
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
                                  return CHAR_LITERAL;
                                }

'([^'\n\\]|\\.)*                { 
                                  fprintf(stderr, "Error: Unterminated character literal at line %d\n", line_number);
                                }

    /* ========== IDENTIFIERS (Lower Priority) ========== */
{IDENTIFIER}                    { 
                                  yylval.strval = strdup(yytext);
                                  return IDENTIFIER;
                                }

    /* ========== WHITESPACE & NEWLINE ========== */
{WHITESPACE}                    { /* Ignore whitespace */ }
{NEWLINE}                       { line_number++; }

    /* ========== ERROR HANDLING ========== */
.                               { 
                                  fprintf(stderr, "Error: Invalid character '%s' at line %d\n", yytext, line_number);
                                }

%%
