/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_POKELANG_TAB_H_INCLUDED
# define YY_YY_POKELANG_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 409 "pokelang.y"

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

#line 75 "pokelang.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INCLUDE = 258,                 /* INCLUDE  */
    DEFINE = 259,                  /* DEFINE  */
    INT = 260,                     /* INT  */
    FLOAT = 261,                   /* FLOAT  */
    DOUBLE = 262,                  /* DOUBLE  */
    LONG = 263,                    /* LONG  */
    CHAR = 264,                    /* CHAR  */
    BOOL = 265,                    /* BOOL  */
    VOID = 266,                    /* VOID  */
    IF = 267,                      /* IF  */
    ELSE_IF = 268,                 /* ELSE_IF  */
    ELSE = 269,                    /* ELSE  */
    FOR = 270,                     /* FOR  */
    WHILE = 271,                   /* WHILE  */
    DO = 272,                      /* DO  */
    SWITCH = 273,                  /* SWITCH  */
    CASE = 274,                    /* CASE  */
    DEFAULT = 275,                 /* DEFAULT  */
    BREAK = 276,                   /* BREAK  */
    CONTINUE = 277,                /* CONTINUE  */
    RETURN = 278,                  /* RETURN  */
    PRINT = 279,                   /* PRINT  */
    SCAN = 280,                    /* SCAN  */
    POW = 281,                     /* POW  */
    SQRT = 282,                    /* SQRT  */
    FLOOR = 283,                   /* FLOOR  */
    CEIL = 284,                    /* CEIL  */
    ABS = 285,                     /* ABS  */
    SIN = 286,                     /* SIN  */
    COS = 287,                     /* COS  */
    TAN = 288,                     /* TAN  */
    ASIN = 289,                    /* ASIN  */
    ACOS = 290,                    /* ACOS  */
    ATAN = 291,                    /* ATAN  */
    PRIME = 292,                   /* PRIME  */
    FUNCTION = 293,                /* FUNCTION  */
    MAIN = 294,                    /* MAIN  */
    PLUS = 295,                    /* PLUS  */
    MINUS = 296,                   /* MINUS  */
    MULTIPLY = 297,                /* MULTIPLY  */
    DIVIDE = 298,                  /* DIVIDE  */
    MODULO = 299,                  /* MODULO  */
    AND = 300,                     /* AND  */
    OR = 301,                      /* OR  */
    NOT = 302,                     /* NOT  */
    ASSIGN = 303,                  /* ASSIGN  */
    LESS_THAN = 304,               /* LESS_THAN  */
    GREATER_THAN = 305,            /* GREATER_THAN  */
    LESS_EQUAL = 306,              /* LESS_EQUAL  */
    GREATER_EQUAL = 307,           /* GREATER_EQUAL  */
    EQUAL = 308,                   /* EQUAL  */
    NOT_EQUAL = 309,               /* NOT_EQUAL  */
    SEMICOLON = 310,               /* SEMICOLON  */
    LPAREN = 311,                  /* LPAREN  */
    RPAREN = 312,                  /* RPAREN  */
    LBRACE = 313,                  /* LBRACE  */
    RBRACE = 314,                  /* RBRACE  */
    LBRACKET = 315,                /* LBRACKET  */
    RBRACKET = 316,                /* RBRACKET  */
    COMMA = 317,                   /* COMMA  */
    INTEGER_LITERAL = 318,         /* INTEGER_LITERAL  */
    FLOAT_LITERAL = 319,           /* FLOAT_LITERAL  */
    STRING_LITERAL = 320,          /* STRING_LITERAL  */
    CHAR_LITERAL = 321,            /* CHAR_LITERAL  */
    IDENTIFIER = 322,              /* IDENTIFIER  */
    UNARY_MINUS = 323              /* UNARY_MINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 435 "pokelang.y"

    int intval;
    double floatval;
    char* strval;
    char charval;
    ExprValue expr;

#line 168 "pokelang.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_POKELANG_TAB_H_INCLUDED  */
