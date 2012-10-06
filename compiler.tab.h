/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     AND = 258,
     ARRAY = 259,
     BEGIN_ = 260,
     CONST = 261,
     CONTINUE = 262,
     DO = 263,
     ELSE = 264,
     END = 265,
     EXIT = 266,
     FUNCTION = 267,
     IF = 268,
     NOT = 269,
     OF = 270,
     OR = 271,
     PROCEDURE = 272,
     PROGRAM = 273,
     RECORD = 274,
     THEN = 275,
     TYPE = 276,
     VAR = 277,
     WHILE = 278,
     ISEQUAL = 279,
     NOTEQUAL = 280,
     LESSTHAN = 281,
     GREATERTHAN = 282,
     LESSTHANEQUALS = 283,
     GREATERTHANEQUALS = 284,
     PLUS = 285,
     MINUS = 286,
     MULTIPLY = 287,
     DIVIDE = 288,
     DIV = 289,
     MOD = 290,
     ASSIGN = 291,
     LEFTPAREN = 292,
     RIGHTPAREN = 293,
     PERIOD = 294,
     SEMICOLON = 295,
     COLON = 296,
     LEFTBRACKET = 297,
     RIGHTBRACKET = 298,
     COMMA = 299,
     DOUBLEPERIOD = 300,
     INVALIDTOKEN = 301,
     ID = 302,
     RETURN = 303,
     UNKNOWN = 304,
     UNCLOSED_COMMENT = 305,
     UNCLOSED_QUOTE = 306,
     INT_CONST = 307,
     REAL_CONST = 308,
     BOOL = 309,
     CHAR = 310,
     INT = 311,
     REAL = 312,
     STRING = 313
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 42 "compiler.y"

        int number;
        char string[4096];



/* Line 2068 of yacc.c  */
#line 115 "compiler.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


