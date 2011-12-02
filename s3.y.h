/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     INST_SEP = 258,
     ENDLINE = 259,
     LPAREN = 260,
     RPAREN = 261,
     COMMA = 262,
     EQ = 263,
     BR_CC = 264,
     PCODE_NE = 265,
     PCODE_GT = 266,
     PCODE_GE = 267,
     PCODE_EQ = 268,
     PCODE_LE = 269,
     PCODE_LT = 270,
     PTYPE_AND = 271,
     PTYPE_OR = 272,
     PTYPE_UNC = 273,
     ADD = 274,
     AND = 275,
     ANDL = 276,
     BR = 277,
     BRA = 278,
     DIV = 279,
     JMP = 280,
     JSRR = 281,
     JSR = 282,
     LDI = 283,
     LDR = 284,
     LD = 285,
     LEA = 286,
     MUL = 287,
     NOP = 288,
     NOT = 289,
     NOTL = 290,
     OR = 291,
     ORL = 292,
     RTI = 293,
     RET = 294,
     SET = 295,
     STI = 296,
     STR = 297,
     ST = 298,
     SUB = 299,
     TRAP = 300,
     HALT = 301,
     IN = 302,
     OUT = 303,
     GETC = 304,
     PUTS = 305,
     PUTSP = 306,
     ORIG = 307,
     END = 308,
     REGISTER = 309,
     LABEL = 310,
     IMM = 311,
     PREDICATE = 312,
     STRING = 313
   };
#endif
/* Tokens.  */
#define INST_SEP 258
#define ENDLINE 259
#define LPAREN 260
#define RPAREN 261
#define COMMA 262
#define EQ 263
#define BR_CC 264
#define PCODE_NE 265
#define PCODE_GT 266
#define PCODE_GE 267
#define PCODE_EQ 268
#define PCODE_LE 269
#define PCODE_LT 270
#define PTYPE_AND 271
#define PTYPE_OR 272
#define PTYPE_UNC 273
#define ADD 274
#define AND 275
#define ANDL 276
#define BR 277
#define BRA 278
#define DIV 279
#define JMP 280
#define JSRR 281
#define JSR 282
#define LDI 283
#define LDR 284
#define LD 285
#define LEA 286
#define MUL 287
#define NOP 288
#define NOT 289
#define NOTL 290
#define OR 291
#define ORL 292
#define RTI 293
#define RET 294
#define SET 295
#define STI 296
#define STR 297
#define ST 298
#define SUB 299
#define TRAP 300
#define HALT 301
#define IN 302
#define OUT 303
#define GETC 304
#define PUTS 305
#define PUTSP 306
#define ORIG 307
#define END 308
#define REGISTER 309
#define LABEL 310
#define IMM 311
#define PREDICATE 312
#define STRING 313




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 23 "s3.y"
{
  char * label;
  char * string;
  int    reg;
  int    pred;
  int    imm;
  int    nzp;
  struct inst_d inst;
  inst_t inst_ptr;
}
/* Line 1529 of yacc.c.  */
#line 176 "s3.y.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

