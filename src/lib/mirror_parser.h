/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_MIRROR_PARSER_H_INCLUDED
# define YY_YY_MIRROR_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    CLASS = 258,
    ATTRIBUTE = 259,
    METHOD = 260,
    INSTANCE_SIDE = 261,
    CLASS_SIDE = 262,
    RETURN = 263,
    VAR = 264,
    ASSIGNMENT = 265,
    IF = 266,
    ELSE = 267,
    IMPORT = 268,
    PRIMITIVE = 269,
    MESSAGE_SEND = 270,
    NIL = 271,
    TRUE = 272,
    FALSE = 273,
    SELF = 274,
    SUPER = 275,
    ELSIF = 276,
    WHILE = 277,
    FOR = 278,
    INDEXED = 279,
    BYTE = 280,
    BREAK = 281,
    CONTINUE = 282,
    NAMESPACE = 283,
    THIS_CONTEXT = 284,
    BINARY_MESSAGE = 285,
    IDENT = 286,
    STRING = 287,
    CHAR = 288,
    INTEGER = 289,
    FLOAT = 290,
    BINARY_MESSAGE_EXPR = 291,
    MESSAGE_SEND_EXPR = 292,
    UMINUS = 293,
    BLOCK_PARM = 294
  };
#endif
/* Tokens.  */
#define CLASS 258
#define ATTRIBUTE 259
#define METHOD 260
#define INSTANCE_SIDE 261
#define CLASS_SIDE 262
#define RETURN 263
#define VAR 264
#define ASSIGNMENT 265
#define IF 266
#define ELSE 267
#define IMPORT 268
#define PRIMITIVE 269
#define MESSAGE_SEND 270
#define NIL 271
#define TRUE 272
#define FALSE 273
#define SELF 274
#define SUPER 275
#define ELSIF 276
#define WHILE 277
#define FOR 278
#define INDEXED 279
#define BYTE 280
#define BREAK 281
#define CONTINUE 282
#define NAMESPACE 283
#define THIS_CONTEXT 284
#define BINARY_MESSAGE 285
#define IDENT 286
#define STRING 287
#define CHAR 288
#define INTEGER 289
#define FLOAT 290
#define BINARY_MESSAGE_EXPR 291
#define MESSAGE_SEND_EXPR 292
#define UMINUS 293
#define BLOCK_PARM 294

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 38 "mirror_parser.y" /* yacc.c:1909  */

  char		cval;
  double	fval;
  long		ival;
  char		*sval;
  mmAbstractNode*	node;

#line 140 "mirror_parser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MIRROR_PARSER_H_INCLUDED  */
