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

#ifndef YY_PTXINFO_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUDA_SIM_PTXINFO_TAB_H_INCLUDED
# define YY_PTXINFO_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUDA_SIM_PTXINFO_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ptxinfo_debug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INT_OPERAND = 258,             /* INT_OPERAND  */
    HEADER = 259,                  /* HEADER  */
    INFO = 260,                    /* INFO  */
    FUNC = 261,                    /* FUNC  */
    USED = 262,                    /* USED  */
    REGS = 263,                    /* REGS  */
    BARRIERS = 264,                /* BARRIERS  */
    COMPILETIME = 265,             /* COMPILETIME  */
    BYTES = 266,                   /* BYTES  */
    LMEM = 267,                    /* LMEM  */
    SMEM = 268,                    /* SMEM  */
    CMEM = 269,                    /* CMEM  */
    GMEM = 270,                    /* GMEM  */
    IDENTIFIER = 271,              /* IDENTIFIER  */
    PLUS = 272,                    /* PLUS  */
    COMMA = 273,                   /* COMMA  */
    LEFT_SQUARE_BRACKET = 274,     /* LEFT_SQUARE_BRACKET  */
    RIGHT_SQUARE_BRACKET = 275,    /* RIGHT_SQUARE_BRACKET  */
    COLON = 276,                   /* COLON  */
    SEMICOLON = 277,               /* SEMICOLON  */
    QUOTE = 278,                   /* QUOTE  */
    LINE = 279,                    /* LINE  */
    WARNING = 280,                 /* WARNING  */
    FOR = 281,                     /* FOR  */
    TEXTURES = 282,                /* TEXTURES  */
    DUPLICATE = 283,               /* DUPLICATE  */
    FUNCTION = 284,                /* FUNCTION  */
    VARIABLE = 285,                /* VARIABLE  */
    FATAL = 286,                   /* FATAL  */
    PERIOD = 287,                  /* PERIOD  */
    MS = 288                       /* MS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 41 "ptxinfo.y"

  int    int_value;
  char * string_value;

#line 102 "cuda-sim/ptxinfo.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int ptxinfo_parse (yyscan_t scanner, ptxinfo_data* ptxinfo);


#endif /* !YY_PTXINFO_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUDA_SIM_PTXINFO_TAB_H_INCLUDED  */
