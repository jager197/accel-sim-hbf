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

#ifndef YY_CUOBJDUMP_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_LIBCUDA_CUOBJDUMP_PARSER_H_INCLUDED
# define YY_CUOBJDUMP_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_LIBCUDA_CUOBJDUMP_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int cuobjdump_debug;
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
    H_SEPARATOR = 258,             /* H_SEPARATOR  */
    H_ARCH = 259,                  /* H_ARCH  */
    H_CODEVERSION = 260,           /* H_CODEVERSION  */
    H_PRODUCER = 261,              /* H_PRODUCER  */
    H_HOST = 262,                  /* H_HOST  */
    H_COMPILESIZE = 263,           /* H_COMPILESIZE  */
    H_IDENTIFIER = 264,            /* H_IDENTIFIER  */
    H_UNKNOWN = 265,               /* H_UNKNOWN  */
    H_COMPRESSED = 266,            /* H_COMPRESSED  */
    CODEVERSION = 267,             /* CODEVERSION  */
    STRING = 268,                  /* STRING  */
    FILENAME = 269,                /* FILENAME  */
    DECIMAL = 270,                 /* DECIMAL  */
    PTXHEADER = 271,               /* PTXHEADER  */
    ELFHEADER = 272,               /* ELFHEADER  */
    PTXLINE = 273,                 /* PTXLINE  */
    ELFLINE = 274,                 /* ELFLINE  */
    SASSLINE = 275,                /* SASSLINE  */
    IDENTIFIER = 276,              /* IDENTIFIER  */
    NEWLINE = 277                  /* NEWLINE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 50 "cuobjdump.y"

	char* string_value;

#line 90 "libcuda/cuobjdump_parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int cuobjdump_parse (yyscan_t scanner, struct cuobjdump_parser* parser, std::list<cuobjdumpSection*> &cuobjdumpSectionList);


#endif /* !YY_CUOBJDUMP_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_LIBCUDA_CUOBJDUMP_PARSER_H_INCLUDED  */
