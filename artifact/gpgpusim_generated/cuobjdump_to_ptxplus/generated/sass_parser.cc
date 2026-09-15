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


/* Substitute the variable and function names.  */
#define yyparse         sass_parse
#define yylex           sass_lex
#define yyerror         sass_error
#define yydebug         sass_debug
#define yynerrs         sass_nerrs
#define yylval          sass_lval
#define yychar          sass_char

/* First part of user prologue.  */
#line 28 "sass.y"

#include <stdio.h>
#include "cuobjdumpInstList.h"

int yylex(void);
void yyerror(const char*);
void debug_print( const char *s );

extern cuobjdumpInstList *g_instList;

cuobjdumpInst *instEntry;

#line 91 "cuobjdump_to_ptxplus/sass_parser.cc"

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

#include "sass_parser.hh"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_BAR = 3,                        /* BAR  */
  YYSYMBOL_ADA = 4,                        /* ADA  */
  YYSYMBOL_AND = 5,                        /* AND  */
  YYSYMBOL_ANDS = 6,                       /* ANDS  */
  YYSYMBOL_BRA = 7,                        /* BRA  */
  YYSYMBOL_BRX = 8,                        /* BRX  */
  YYSYMBOL_CAL = 9,                        /* CAL  */
  YYSYMBOL_COS = 10,                       /* COS  */
  YYSYMBOL_DADD = 11,                      /* DADD  */
  YYSYMBOL_DMIN = 12,                      /* DMIN  */
  YYSYMBOL_DMAX = 13,                      /* DMAX  */
  YYSYMBOL_DFMA = 14,                      /* DFMA  */
  YYSYMBOL_DMUL = 15,                      /* DMUL  */
  YYSYMBOL_EX2 = 16,                       /* EX2  */
  YYSYMBOL_F2F = 17,                       /* F2F  */
  YYSYMBOL_F2I = 18,                       /* F2I  */
  YYSYMBOL_FADD = 19,                      /* FADD  */
  YYSYMBOL_FADD32 = 20,                    /* FADD32  */
  YYSYMBOL_FADD32I = 21,                   /* FADD32I  */
  YYSYMBOL_FMAD = 22,                      /* FMAD  */
  YYSYMBOL_FMAD32I = 23,                   /* FMAD32I  */
  YYSYMBOL_FMUL = 24,                      /* FMUL  */
  YYSYMBOL_FMUL32 = 25,                    /* FMUL32  */
  YYSYMBOL_FMUL32I = 26,                   /* FMUL32I  */
  YYSYMBOL_FSET = 27,                      /* FSET  */
  YYSYMBOL_DSET = 28,                      /* DSET  */
  YYSYMBOL_G2R = 29,                       /* G2R  */
  YYSYMBOL_GLD = 30,                       /* GLD  */
  YYSYMBOL_GST = 31,                       /* GST  */
  YYSYMBOL_I2F = 32,                       /* I2F  */
  YYSYMBOL_I2I = 33,                       /* I2I  */
  YYSYMBOL_IADD = 34,                      /* IADD  */
  YYSYMBOL_IADD32 = 35,                    /* IADD32  */
  YYSYMBOL_IADD32I = 36,                   /* IADD32I  */
  YYSYMBOL_IMAD = 37,                      /* IMAD  */
  YYSYMBOL_ISAD = 38,                      /* ISAD  */
  YYSYMBOL_IMAD24 = 39,                    /* IMAD24  */
  YYSYMBOL_IMAD32I = 40,                   /* IMAD32I  */
  YYSYMBOL_IMAD32 = 41,                    /* IMAD32  */
  YYSYMBOL_IADDCARRY = 42,                 /* IADDCARRY  */
  YYSYMBOL_IMUL = 43,                      /* IMUL  */
  YYSYMBOL_IMUL24 = 44,                    /* IMUL24  */
  YYSYMBOL_IMUL24H = 45,                   /* IMUL24H  */
  YYSYMBOL_IMULS24 = 46,                   /* IMULS24  */
  YYSYMBOL_IMUL32 = 47,                    /* IMUL32  */
  YYSYMBOL_IMUL32S24 = 48,                 /* IMUL32S24  */
  YYSYMBOL_IMUL32U24 = 49,                 /* IMUL32U24  */
  YYSYMBOL_IMUL32I = 50,                   /* IMUL32I  */
  YYSYMBOL_IMUL32I24 = 51,                 /* IMUL32I24  */
  YYSYMBOL_IMUL32IS24 = 52,                /* IMUL32IS24  */
  YYSYMBOL_ISET = 53,                      /* ISET  */
  YYSYMBOL_LG2 = 54,                       /* LG2  */
  YYSYMBOL_LLD = 55,                       /* LLD  */
  YYSYMBOL_LST = 56,                       /* LST  */
  YYSYMBOL_MOV = 57,                       /* MOV  */
  YYSYMBOL_MOV32 = 58,                     /* MOV32  */
  YYSYMBOL_MVC = 59,                       /* MVC  */
  YYSYMBOL_MVI = 60,                       /* MVI  */
  YYSYMBOL_NOP = 61,                       /* NOP  */
  YYSYMBOL_NOT = 62,                       /* NOT  */
  YYSYMBOL_NOTS = 63,                      /* NOTS  */
  YYSYMBOL_OR = 64,                        /* OR  */
  YYSYMBOL_ORS = 65,                       /* ORS  */
  YYSYMBOL_R2A = 66,                       /* R2A  */
  YYSYMBOL_R2G = 67,                       /* R2G  */
  YYSYMBOL_R2GU16U8 = 68,                  /* R2GU16U8  */
  YYSYMBOL_RCP = 69,                       /* RCP  */
  YYSYMBOL_RCP32 = 70,                     /* RCP32  */
  YYSYMBOL_RET = 71,                       /* RET  */
  YYSYMBOL_RRO = 72,                       /* RRO  */
  YYSYMBOL_RSQ = 73,                       /* RSQ  */
  YYSYMBOL_SIN = 74,                       /* SIN  */
  YYSYMBOL_SHL = 75,                       /* SHL  */
  YYSYMBOL_SHR = 76,                       /* SHR  */
  YYSYMBOL_SSY = 77,                       /* SSY  */
  YYSYMBOL_XOR = 78,                       /* XOR  */
  YYSYMBOL_XORS = 79,                      /* XORS  */
  YYSYMBOL_S2R = 80,                       /* S2R  */
  YYSYMBOL_SASS_LD = 81,                   /* SASS_LD  */
  YYSYMBOL_STS = 82,                       /* STS  */
  YYSYMBOL_LDS = 83,                       /* LDS  */
  YYSYMBOL_SASS_ST = 84,                   /* SASS_ST  */
  YYSYMBOL_IMIN = 85,                      /* IMIN  */
  YYSYMBOL_IMAX = 86,                      /* IMAX  */
  YYSYMBOL_A2R = 87,                       /* A2R  */
  YYSYMBOL_FMAX = 88,                      /* FMAX  */
  YYSYMBOL_FMIN = 89,                      /* FMIN  */
  YYSYMBOL_TEX = 90,                       /* TEX  */
  YYSYMBOL_TEX32 = 91,                     /* TEX32  */
  YYSYMBOL_C2R = 92,                       /* C2R  */
  YYSYMBOL_EXIT = 93,                      /* EXIT  */
  YYSYMBOL_GRED = 94,                      /* GRED  */
  YYSYMBOL_PBK = 95,                       /* PBK  */
  YYSYMBOL_BRK = 96,                       /* BRK  */
  YYSYMBOL_R2C = 97,                       /* R2C  */
  YYSYMBOL_GATOM = 98,                     /* GATOM  */
  YYSYMBOL_VOTE = 99,                      /* VOTE  */
  YYSYMBOL_EQ = 100,                       /* EQ  */
  YYSYMBOL_EQU = 101,                      /* EQU  */
  YYSYMBOL_GE = 102,                       /* GE  */
  YYSYMBOL_GEU = 103,                      /* GEU  */
  YYSYMBOL_GT = 104,                       /* GT  */
  YYSYMBOL_GTU = 105,                      /* GTU  */
  YYSYMBOL_LE = 106,                       /* LE  */
  YYSYMBOL_LEU = 107,                      /* LEU  */
  YYSYMBOL_LT = 108,                       /* LT  */
  YYSYMBOL_LTU = 109,                      /* LTU  */
  YYSYMBOL_NE = 110,                       /* NE  */
  YYSYMBOL_NEU = 111,                      /* NEU  */
  YYSYMBOL_DOTBEXT = 112,                  /* DOTBEXT  */
  YYSYMBOL_DOTS = 113,                     /* DOTS  */
  YYSYMBOL_DOTSFU = 114,                   /* DOTSFU  */
  YYSYMBOL_DOTTRUNC = 115,                 /* DOTTRUNC  */
  YYSYMBOL_DOTCEIL = 116,                  /* DOTCEIL  */
  YYSYMBOL_DOTFLOOR = 117,                 /* DOTFLOOR  */
  YYSYMBOL_DOTIR = 118,                    /* DOTIR  */
  YYSYMBOL_DOTUN = 119,                    /* DOTUN  */
  YYSYMBOL_DOTNODEP = 120,                 /* DOTNODEP  */
  YYSYMBOL_DOTSAT = 121,                   /* DOTSAT  */
  YYSYMBOL_DOTANY = 122,                   /* DOTANY  */
  YYSYMBOL_DOTALL = 123,                   /* DOTALL  */
  YYSYMBOL_DOTF16 = 124,                   /* DOTF16  */
  YYSYMBOL_DOTF32 = 125,                   /* DOTF32  */
  YYSYMBOL_DOTF64 = 126,                   /* DOTF64  */
  YYSYMBOL_DOTS8 = 127,                    /* DOTS8  */
  YYSYMBOL_DOTS16 = 128,                   /* DOTS16  */
  YYSYMBOL_DOTS32 = 129,                   /* DOTS32  */
  YYSYMBOL_DOTS64 = 130,                   /* DOTS64  */
  YYSYMBOL_DOTS128 = 131,                  /* DOTS128  */
  YYSYMBOL_DOTU8 = 132,                    /* DOTU8  */
  YYSYMBOL_DOTU16 = 133,                   /* DOTU16  */
  YYSYMBOL_DOTU32 = 134,                   /* DOTU32  */
  YYSYMBOL_DOTU24 = 135,                   /* DOTU24  */
  YYSYMBOL_DOTU64 = 136,                   /* DOTU64  */
  YYSYMBOL_DOTHI = 137,                    /* DOTHI  */
  YYSYMBOL_DOTNOINC = 138,                 /* DOTNOINC  */
  YYSYMBOL_DOTEQ = 139,                    /* DOTEQ  */
  YYSYMBOL_DOTEQU = 140,                   /* DOTEQU  */
  YYSYMBOL_DOTFALSE = 141,                 /* DOTFALSE  */
  YYSYMBOL_DOTGE = 142,                    /* DOTGE  */
  YYSYMBOL_DOTGEU = 143,                   /* DOTGEU  */
  YYSYMBOL_DOTGT = 144,                    /* DOTGT  */
  YYSYMBOL_DOTGTU = 145,                   /* DOTGTU  */
  YYSYMBOL_DOTLE = 146,                    /* DOTLE  */
  YYSYMBOL_DOTLEU = 147,                   /* DOTLEU  */
  YYSYMBOL_DOTLT = 148,                    /* DOTLT  */
  YYSYMBOL_DOTLTU = 149,                   /* DOTLTU  */
  YYSYMBOL_DOTNE = 150,                    /* DOTNE  */
  YYSYMBOL_DOTNEU = 151,                   /* DOTNEU  */
  YYSYMBOL_DOTNSF = 152,                   /* DOTNSF  */
  YYSYMBOL_DOTSF = 153,                    /* DOTSF  */
  YYSYMBOL_DOTCARRY = 154,                 /* DOTCARRY  */
  YYSYMBOL_DOTCC = 155,                    /* DOTCC  */
  YYSYMBOL_DOTX = 156,                     /* DOTX  */
  YYSYMBOL_DOTE = 157,                     /* DOTE  */
  YYSYMBOL_DOTRED = 158,                   /* DOTRED  */
  YYSYMBOL_DOTPOPC = 159,                  /* DOTPOPC  */
  YYSYMBOL_REGISTER = 160,                 /* REGISTER  */
  YYSYMBOL_REGISTERLO = 161,               /* REGISTERLO  */
  YYSYMBOL_REGISTERHI = 162,               /* REGISTERHI  */
  YYSYMBOL_OFFSETREGISTER = 163,           /* OFFSETREGISTER  */
  YYSYMBOL_PREDREGISTER = 164,             /* PREDREGISTER  */
  YYSYMBOL_PREDREGISTER2 = 165,            /* PREDREGISTER2  */
  YYSYMBOL_PREDREGISTER3 = 166,            /* PREDREGISTER3  */
  YYSYMBOL_SREGISTER = 167,                /* SREGISTER  */
  YYSYMBOL_VERSIONHEADER = 168,            /* VERSIONHEADER  */
  YYSYMBOL_FUNCTIONHEADER = 169,           /* FUNCTIONHEADER  */
  YYSYMBOL_SMEMLOCATION = 170,             /* SMEMLOCATION  */
  YYSYMBOL_ABSSMEMLOCATION = 171,          /* ABSSMEMLOCATION  */
  YYSYMBOL_GMEMLOCATION = 172,             /* GMEMLOCATION  */
  YYSYMBOL_CMEMLOCATION = 173,             /* CMEMLOCATION  */
  YYSYMBOL_LMEMLOCATION = 174,             /* LMEMLOCATION  */
  YYSYMBOL_IDENTIFIER = 175,               /* IDENTIFIER  */
  YYSYMBOL_HEXLITERAL = 176,               /* HEXLITERAL  */
  YYSYMBOL_LEFTBRACKET = 177,              /* LEFTBRACKET  */
  YYSYMBOL_RIGHTBRACKET = 178,             /* RIGHTBRACKET  */
  YYSYMBOL_PIPE = 179,                     /* PIPE  */
  YYSYMBOL_TILDE = 180,                    /* TILDE  */
  YYSYMBOL_NEWLINE = 181,                  /* NEWLINE  */
  YYSYMBOL_SEMICOLON = 182,                /* SEMICOLON  */
  YYSYMBOL_LABEL = 183,                    /* LABEL  */
  YYSYMBOL_LABELSTART = 184,               /* LABELSTART  */
  YYSYMBOL_LABELEND = 185,                 /* LABELEND  */
  YYSYMBOL_PTXHEADER = 186,                /* PTXHEADER  */
  YYSYMBOL_ELFHEADER = 187,                /* ELFHEADER  */
  YYSYMBOL_INFOARCHVERSION = 188,          /* INFOARCHVERSION  */
  YYSYMBOL_INFOCODEVERSION_HEADER = 189,   /* INFOCODEVERSION_HEADER  */
  YYSYMBOL_INFOCODEVERSION = 190,          /* INFOCODEVERSION  */
  YYSYMBOL_INFOPRODUCER = 191,             /* INFOPRODUCER  */
  YYSYMBOL_INFOHOST = 192,                 /* INFOHOST  */
  YYSYMBOL_INFOCOMPILESIZE_HEADER = 193,   /* INFOCOMPILESIZE_HEADER  */
  YYSYMBOL_INFOCOMPILESIZE = 194,          /* INFOCOMPILESIZE  */
  YYSYMBOL_INFOIDENTIFIER = 195,           /* INFOIDENTIFIER  */
  YYSYMBOL_DOT = 196,                      /* DOT  */
  YYSYMBOL_INSTHEX = 197,                  /* INSTHEX  */
  YYSYMBOL_OSQBRACKET = 198,               /* OSQBRACKET  */
  YYSYMBOL_CSQBRACKET = 199,               /* CSQBRACKET  */
  YYSYMBOL_YYACCEPT = 200,                 /* $accept  */
  YYSYMBOL_program = 201,                  /* program  */
  YYSYMBOL_sassCode = 202,                 /* sassCode  */
  YYSYMBOL_functionList = 203,             /* functionList  */
  YYSYMBOL_function = 204,                 /* function  */
  YYSYMBOL_205_1 = 205,                    /* $@1  */
  YYSYMBOL_statementList = 206,            /* statementList  */
  YYSYMBOL_statement = 207,                /* statement  */
  YYSYMBOL_208_2 = 208,                    /* $@2  */
  YYSYMBOL_statementend = 209,             /* statementend  */
  YYSYMBOL_instructionHex = 210,           /* instructionHex  */
  YYSYMBOL_instructionLabel = 211,         /* instructionLabel  */
  YYSYMBOL_assemblyInstruction = 212,      /* assemblyInstruction  */
  YYSYMBOL_baseInstruction = 213,          /* baseInstruction  */
  YYSYMBOL_simpleInstructions = 214,       /* simpleInstructions  */
  YYSYMBOL_pbkInstruction = 215,           /* pbkInstruction  */
  YYSYMBOL_216_3 = 216,                    /* $@3  */
  YYSYMBOL_branchInstructions = 217,       /* branchInstructions  */
  YYSYMBOL_218_4 = 218,                    /* $@4  */
  YYSYMBOL_219_5 = 219,                    /* $@5  */
  YYSYMBOL_220_6 = 220,                    /* $@6  */
  YYSYMBOL_221_7 = 221,                    /* $@7  */
  YYSYMBOL_modifierList = 222,             /* modifierList  */
  YYSYMBOL_modifier = 223,                 /* modifier  */
  YYSYMBOL_opTypes = 224,                  /* opTypes  */
  YYSYMBOL_operandList = 225,              /* operandList  */
  YYSYMBOL_226_8 = 226,                    /* $@8  */
  YYSYMBOL_operand = 227,                  /* operand  */
  YYSYMBOL_registerlocation = 228,         /* registerlocation  */
  YYSYMBOL_regMod = 229,                   /* regMod  */
  YYSYMBOL_memorylocation = 230,           /* memorylocation  */
  YYSYMBOL_immediateValue = 231,           /* immediateValue  */
  YYSYMBOL_extraModifier = 232,            /* extraModifier  */
  YYSYMBOL_instructionPredicate = 233,     /* instructionPredicate  */
  YYSYMBOL_operandPredicate = 234,         /* operandPredicate  */
  YYSYMBOL_preOperand = 235,               /* preOperand  */
  YYSYMBOL_predicateModifier = 236         /* predicateModifier  */
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
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   382

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  200
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  226
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  262

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   454


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    94,    94,    95,    97,    98,    99,   101,   102,   105,
     105,   117,   118,   119,   120,   121,   124,   124,   127,   128,
     131,   134,   151,   157,   158,   159,   160,   161,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     165,   165,   165,   165,   165,   165,   165,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   168,   168,   168,   168,   168,
     168,   168,   168,   169,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   174,   174,   174,   174,   174,   174,   174,
     177,   177,   199,   199,   217,   217,   235,   235,   254,   254,
     275,   277,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   314,   314,   316,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   332,   333,   334,   335,   336,
     337,   338,   339,   343,   344,   348,   349,   362,   363,   364,
     367,   368,   371,   372,   373,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   385,   388,   391,   399,   407,   408,
     409,   412,   413,   414,   415,   416,   417,   418,   419,   420,
     421,   422,   423,   424,   425,   426,   427
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
  "\"end of file\"", "error", "\"invalid token\"", "BAR", "ADA", "AND",
  "ANDS", "BRA", "BRX", "CAL", "COS", "DADD", "DMIN", "DMAX", "DFMA",
  "DMUL", "EX2", "F2F", "F2I", "FADD", "FADD32", "FADD32I", "FMAD",
  "FMAD32I", "FMUL", "FMUL32", "FMUL32I", "FSET", "DSET", "G2R", "GLD",
  "GST", "I2F", "I2I", "IADD", "IADD32", "IADD32I", "IMAD", "ISAD",
  "IMAD24", "IMAD32I", "IMAD32", "IADDCARRY", "IMUL", "IMUL24", "IMUL24H",
  "IMULS24", "IMUL32", "IMUL32S24", "IMUL32U24", "IMUL32I", "IMUL32I24",
  "IMUL32IS24", "ISET", "LG2", "LLD", "LST", "MOV", "MOV32", "MVC", "MVI",
  "NOP", "NOT", "NOTS", "OR", "ORS", "R2A", "R2G", "R2GU16U8", "RCP",
  "RCP32", "RET", "RRO", "RSQ", "SIN", "SHL", "SHR", "SSY", "XOR", "XORS",
  "S2R", "SASS_LD", "STS", "LDS", "SASS_ST", "IMIN", "IMAX", "A2R", "FMAX",
  "FMIN", "TEX", "TEX32", "C2R", "EXIT", "GRED", "PBK", "BRK", "R2C",
  "GATOM", "VOTE", "EQ", "EQU", "GE", "GEU", "GT", "GTU", "LE", "LEU",
  "LT", "LTU", "NE", "NEU", "DOTBEXT", "DOTS", "DOTSFU", "DOTTRUNC",
  "DOTCEIL", "DOTFLOOR", "DOTIR", "DOTUN", "DOTNODEP", "DOTSAT", "DOTANY",
  "DOTALL", "DOTF16", "DOTF32", "DOTF64", "DOTS8", "DOTS16", "DOTS32",
  "DOTS64", "DOTS128", "DOTU8", "DOTU16", "DOTU32", "DOTU24", "DOTU64",
  "DOTHI", "DOTNOINC", "DOTEQ", "DOTEQU", "DOTFALSE", "DOTGE", "DOTGEU",
  "DOTGT", "DOTGTU", "DOTLE", "DOTLEU", "DOTLT", "DOTLTU", "DOTNE",
  "DOTNEU", "DOTNSF", "DOTSF", "DOTCARRY", "DOTCC", "DOTX", "DOTE",
  "DOTRED", "DOTPOPC", "REGISTER", "REGISTERLO", "REGISTERHI",
  "OFFSETREGISTER", "PREDREGISTER", "PREDREGISTER2", "PREDREGISTER3",
  "SREGISTER", "VERSIONHEADER", "FUNCTIONHEADER", "SMEMLOCATION",
  "ABSSMEMLOCATION", "GMEMLOCATION", "CMEMLOCATION", "LMEMLOCATION",
  "IDENTIFIER", "HEXLITERAL", "LEFTBRACKET", "RIGHTBRACKET", "PIPE",
  "TILDE", "NEWLINE", "SEMICOLON", "LABEL", "LABELSTART", "LABELEND",
  "PTXHEADER", "ELFHEADER", "INFOARCHVERSION", "INFOCODEVERSION_HEADER",
  "INFOCODEVERSION", "INFOPRODUCER", "INFOHOST", "INFOCOMPILESIZE_HEADER",
  "INFOCOMPILESIZE", "INFOIDENTIFIER", "DOT", "INSTHEX", "OSQBRACKET",
  "CSQBRACKET", "$accept", "program", "sassCode", "functionList",
  "function", "$@1", "statementList", "statement", "$@2", "statementend",
  "instructionHex", "instructionLabel", "assemblyInstruction",
  "baseInstruction", "simpleInstructions", "pbkInstruction", "$@3",
  "branchInstructions", "$@4", "$@5", "$@6", "$@7", "modifierList",
  "modifier", "opTypes", "operandList", "$@8", "operand",
  "registerlocation", "regMod", "memorylocation", "immediateValue",
  "extraModifier", "instructionPredicate", "operandPredicate",
  "preOperand", "predicateModifier", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-184)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-129)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -126,  -174,  -163,     3,  -184,  -165,  -117,  -184,  -184,  -128,
    -121,  -116,  -128,  -184,  -128,  -184,  -184,  -128,  -120,  -184,
    -119,  -136,  -118,  -184,  -134,  -184,  -114,  -115,  -109,  -184,
    -112,  -184,   -96,  -184,  -184,   186,  -184,  -184,  -184,  -184,
    -184,  -184,  -113,  -184,   -73,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,   -80,
    -184,  -184,  -184,   -79,  -184,  -184,  -105,  -184,  -184,  -184,
     -48,  -106,   -57,   -18,   283,   -55,   283,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -105,  -184,   -67,
     -54,  -184,  -184,   -53,  -184,  -184,  -184,  -132,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,   -10,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,   -31,  -184,  -184,  -184,  -122,   -67,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,   -48,  -127,
    -127,   -35,  -184,  -184,   -22,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,   -52,   -51,  -184,   -72,  -184,  -184,
    -184,  -184
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     3,     0,     0,     1,     2,     6,
       0,     0,     4,     8,     0,     9,     7,     5,    16,    15,
      16,     0,     0,    10,     0,    13,     0,     0,    19,    11,
       0,    14,     0,    20,    17,     0,    12,    21,   107,    28,
      29,    30,   122,    31,   126,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,   118,    64,    65,
      66,    67,    68,    69,    73,    70,    71,    72,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    97,
      95,    96,    98,    99,   100,   101,   102,   103,   104,   105,
     108,   109,   110,   111,   112,   113,   114,   115,   106,     0,
     120,   116,   117,     0,   119,    18,   131,    23,    27,    24,
       0,     0,     0,     0,     0,     0,     0,   133,   134,   135,
     136,   137,   138,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   139,   140,   141,   142,   164,   131,   132,   205,
       0,   125,   127,     0,    25,   121,    26,   162,   130,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   204,   123,   129,     0,   210,
     208,   209,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   184,   177,   178,   180,     0,   207,
     179,   185,   186,   187,   188,   189,   190,   191,     0,     0,
       0,     0,   163,   165,   170,   171,   172,   173,   174,   183,
     175,   182,   181,   206,     0,     0,   167,     0,   169,   168,
     166,   176
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -184,  -184,   126,   116,    27,  -184,  -184,   111,  -184,  -184,
    -184,  -184,  -184,  -184,  -142,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,   -45,  -184,  -111,  -184,  -184,  -184,  -183,  -184,
    -184,  -184,  -184,  -104,  -184,  -184,   -94
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     3,     4,    12,    13,    18,    20,    21,    22,    34,
      35,    28,   135,   136,   137,   138,   145,   139,   140,   141,
     142,   143,   176,   177,   178,   187,   208,   242,   243,   250,
     244,   245,   246,   180,   247,   248,   205
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     209,     5,   184,     7,   186,     6,   210,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     9,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   224,   225,   226,   227,   228,   251,    16,
     230,    11,     1,   252,    16,    25,    26,    29,    30,   -22,
     -22,   172,   173,   174,   175,     2,   255,   256,    10,    15,
      14,    19,    23,  -124,   211,  -128,    27,    31,    32,    36,
     181,   241,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,    33,    37,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   144,   146,   179,   182,
     183,   185,   206,   207,   249,   257,   259,   261,   260,     8,
      17,    24,   188,   258,   254,   253,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     224,   225,   226,   227,   228,     0,   229,   230,     0,     0,
     231,   232,   233,   234,   235,   236,   237,   238,     0,   239,
     240,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     2,     0,     0,     0,   241,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,    38,    39,    40,    41,
       0,    43,     0,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,     0,     0,   131,
     132,     0,   134
};

static const yytype_int16 yycheck[] =
{
      10,   175,   144,     0,   146,   168,    16,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   181,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   160,   161,   162,   163,   164,   160,    12,
     167,   169,   168,   165,    17,   181,   182,   181,   182,   181,
     182,   156,   157,   158,   159,   181,   239,   240,   175,   175,
     181,   181,   181,   176,    74,   138,   184,   181,   183,   181,
     176,   198,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   197,   185,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   196,   196,   166,   176,
     138,   176,   176,   176,   155,   160,   178,   199,   179,     3,
      14,    20,   177,   244,   238,   229,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     160,   161,   162,   163,   164,    -1,   166,   167,    -1,    -1,
     170,   171,   172,   173,   174,   175,   176,   177,    -1,   179,
     180,   168,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   181,    -1,    -1,    -1,   198,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,     3,     4,     5,     6,
      -1,     8,    -1,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    -1,    96,
      97,    -1,    99
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   168,   181,   201,   202,   175,   168,     0,   202,   181,
     175,   169,   203,   204,   181,   175,   204,   203,   205,   181,
     206,   207,   208,   181,   207,   181,   182,   184,   211,   181,
     182,   181,   183,   197,   209,   210,   181,   185,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   212,   213,   214,   215,   217,
     218,   219,   220,   221,   196,   216,   196,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   156,   157,   158,   159,   222,   223,   224,   166,
     233,   176,   176,   138,   214,   176,   214,   225,   222,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   236,   176,   176,   226,    10,
      16,    74,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   160,   161,   162,   163,   164,   166,
     167,   170,   171,   172,   173,   174,   175,   176,   177,   179,
     180,   198,   227,   228,   230,   231,   232,   234,   235,   155,
     229,   160,   165,   236,   233,   228,   228,   160,   224,   178,
     179,   199
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   200,   201,   201,   202,   202,   202,   203,   203,   205,
     204,   206,   206,   206,   206,   206,   208,   207,   209,   209,
     210,   211,   212,   213,   213,   213,   213,   213,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     216,   215,   218,   217,   219,   217,   220,   217,   221,   217,
     222,   222,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   226,   225,   225,   227,   227,   227,   227,   227,
     227,   227,   227,   227,   227,   228,   228,   228,   228,   228,
     228,   228,   228,   229,   229,   230,   230,   230,   230,   230,
     231,   231,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   233,   233,   234,   234,   235,   235,
     235,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     4,     5,     3,     2,     1,     0,
       5,     3,     4,     2,     3,     1,     0,     3,     2,     0,
       1,     3,     3,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     3,     0,     4,     0,     3,     0,     3,     0,     4,
       2,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     3,     0,     1,     3,     2,     3,     2,
       1,     1,     1,     1,     1,     2,     3,     1,     1,     1,
       1,     2,     2,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1
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
  case 4: /* sassCode: VERSIONHEADER IDENTIFIER NEWLINE functionList  */
#line 97 "sass.y"
                                                                                { debug_print((yyvsp[-3].string_value)); debug_print((yyvsp[-2].string_value)); debug_print(" No parsing errors\n\n");  }
#line 1582 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 5: /* sassCode: NEWLINE VERSIONHEADER IDENTIFIER NEWLINE functionList  */
#line 98 "sass.y"
                                                                        { debug_print((yyvsp[-3].string_value)); debug_print((yyvsp[-2].string_value)); debug_print(" No parsing errors\n\n");  }
#line 1588 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 9: /* $@1: %empty  */
#line 105 "sass.y"
                                                  {
					debug_print((yyvsp[-1].string_value)); 
					debug_print((yyvsp[0].string_value));
					debug_print("\n");
					g_instList->addEntry((yyvsp[0].string_value));
					instEntry = new cuobjdumpInst();
					instEntry->setBase(".entry");
					g_instList->add(instEntry);
					g_instList->getListEnd().addOperand((yyvsp[0].string_value));}
#line 1602 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 11: /* statementList: statementList statement NEWLINE  */
#line 117 "sass.y"
                                                        { debug_print("\n"); }
#line 1608 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 12: /* statementList: statementList statement SEMICOLON NEWLINE  */
#line 118 "sass.y"
                                                                { debug_print(";\n"); }
#line 1614 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 13: /* statementList: statement NEWLINE  */
#line 119 "sass.y"
                                                        { debug_print("\n"); }
#line 1620 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 14: /* statementList: statement SEMICOLON NEWLINE  */
#line 120 "sass.y"
                                                                { debug_print(";\n"); }
#line 1626 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 15: /* statementList: NEWLINE  */
#line 121 "sass.y"
                                {}
#line 1632 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 16: /* $@2: %empty  */
#line 124 "sass.y"
                  { instEntry = new cuobjdumpInst(); }
#line 1638 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 19: /* statementend: %empty  */
#line 128 "sass.y"
                            {instEntry->setBase("NOP"); g_instList->add(instEntry); debug_print("NOP");}
#line 1644 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 21: /* instructionLabel: LABELSTART LABEL LABELEND  */
#line 134 "sass.y"
                                                        { char* tempInput = (yyvsp[-1].string_value);
							  char* tempLabel = new char[12];
							  tempLabel[0] = 'l';
							  tempLabel[1] = '0';
							  tempLabel[2] = 'x';
							  for(int i=0; i<(8-strlen(tempInput)); i++)
							  {
								tempLabel[3+i] = '0';
							  }
							  for(int i=(11-strlen(tempInput)); i<11; i++)
							  {
								tempLabel[i] = tempInput[i-(11-strlen(tempInput))];
							  }
							  tempLabel[11] = '\0';
							  instEntry->setLabel(tempLabel); }
#line 1664 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 22: /* assemblyInstruction: baseInstruction modifierList operandList  */
#line 151 "sass.y"
                                                                        { }
#line 1670 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 23: /* baseInstruction: simpleInstructions  */
#line 157 "sass.y"
                                        { debug_print((yyvsp[0].string_value)); instEntry->setBase((yyvsp[0].string_value)); g_instList->add(instEntry);}
#line 1676 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 25: /* baseInstruction: GRED DOT simpleInstructions  */
#line 159 "sass.y"
                                                { debug_print((yyvsp[-2].string_value)); instEntry->setBase((yyvsp[-2].string_value)); g_instList->add(instEntry); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 1682 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 26: /* baseInstruction: GATOM DOT simpleInstructions  */
#line 160 "sass.y"
                                                { debug_print((yyvsp[-2].string_value)); instEntry->setBase((yyvsp[-2].string_value)); g_instList->add(instEntry); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 1688 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 120: /* $@3: %empty  */
#line 177 "sass.y"
                            {
						debug_print((yyvsp[0].string_value)); instEntry->setBase((yyvsp[0].string_value)); g_instList->add(instEntry);
					}
#line 1696 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 121: /* pbkInstruction: PBK $@3 HEXLITERAL  */
#line 179 "sass.y"
                                                     {
						char* tempInput = (yyvsp[0].string_value);
						char* tempLabel = new char[12];
						tempLabel[0] = 'l';
						tempLabel[1] = '0';
						tempLabel[2] = 'x';
						for(int i=0; i<(10-strlen(tempInput)); i++)
						{
							tempLabel[3+i] = '0';
						}
						for(int i=(13-strlen(tempInput)); i<11; i++)
						{
							tempLabel[i] = tempInput[i-(11-strlen(tempInput))];
						}
						tempLabel[11] = '\0';
						g_instList->getListEnd().addOperand(tempLabel);
						g_instList->addCubojdumpLabel(tempLabel);
					}
#line 1719 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 122: /* $@4: %empty  */
#line 199 "sass.y"
                              {debug_print((yyvsp[0].string_value)); instEntry->setBase((yyvsp[0].string_value)); g_instList->add(instEntry);}
#line 1725 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 123: /* branchInstructions: BRA $@4 instructionPredicate HEXLITERAL  */
#line 200 "sass.y"
                                { debug_print((yyvsp[0].string_value));
				  char* tempInput = (yyvsp[0].string_value);
				  char* tempLabel = new char[12];
				  tempLabel[0] = 'l';
				  tempLabel[1] = '0';
				  tempLabel[2] = 'x';
				  for(int i=0; i<(10-strlen(tempInput)); i++)
				  {
					tempLabel[3+i] = '0';
				  }
				  for(int i=(13-strlen(tempInput)); i<11; i++)
				  {
					tempLabel[i] = tempInput[i-(11-strlen(tempInput))];
				  }
				  tempLabel[11] = '\0';
				  g_instList->getListEnd().addOperand(tempLabel);
				  g_instList->addCubojdumpLabel(tempLabel);}
#line 1747 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 124: /* $@5: %empty  */
#line 217 "sass.y"
                              {debug_print((yyvsp[0].string_value)); instEntry->setBase((yyvsp[0].string_value)); g_instList->add(instEntry);}
#line 1753 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 125: /* branchInstructions: BRA $@5 HEXLITERAL  */
#line 218 "sass.y"
                                { debug_print((yyvsp[0].string_value));
				  char* tempInput = (yyvsp[0].string_value);
				  char* tempLabel = new char[12];
				  tempLabel[0] = 'l';
				  tempLabel[1] = '0';
				  tempLabel[2] = 'x';
				  for(int i=0; i<(10-strlen(tempInput)); i++)
				  {
					tempLabel[3+i] = '0';
				  }
				  for(int i=(13-strlen(tempInput)); i<11; i++)
				  {
					tempLabel[i] = tempInput[i-(11-strlen(tempInput))];
				  }
				  tempLabel[11] = '\0';
				  g_instList->getListEnd().addOperand(tempLabel);
				  g_instList->addCubojdumpLabel(tempLabel);}
#line 1775 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 126: /* $@6: %empty  */
#line 235 "sass.y"
                              {debug_print((yyvsp[0].string_value)); instEntry->setBase((yyvsp[0].string_value)); g_instList->add(instEntry);}
#line 1781 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 127: /* branchInstructions: CAL $@6 HEXLITERAL  */
#line 236 "sass.y"
                                { debug_print((yyvsp[0].string_value));
				  char* tempInput = (yyvsp[0].string_value);
				  char* tempLabel = new char[12];
				  tempLabel[0] = 'l';
				  tempLabel[1] = '0';
				  tempLabel[2] = 'x';
				  for(int i=0; i<(10-strlen(tempInput)); i++)
				  {
					tempLabel[3+i] = '0';
				  }
				  for(int i=(13-strlen(tempInput)); i<11; i++)
				  {
					tempLabel[i] = tempInput[i-(11-strlen(tempInput))];
				  }
				  tempLabel[11] = '\0';
				  g_instList->getListEnd().addOperand(tempLabel);
				  g_instList->addCubojdumpLabel(tempLabel);}
#line 1803 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 128: /* $@7: %empty  */
#line 254 "sass.y"
                              {debug_print((yyvsp[0].string_value)); instEntry->setBase((yyvsp[0].string_value)); g_instList->add(instEntry);}
#line 1809 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 129: /* branchInstructions: CAL $@7 DOTNOINC HEXLITERAL  */
#line 255 "sass.y"
                                { debug_print((yyvsp[0].string_value));
				  char* tempInput = (yyvsp[0].string_value);
				  char* tempLabel = new char[12];
				  tempLabel[0] = 'l';
				  tempLabel[1] = '0';
				  tempLabel[2] = 'x';
				  for(int i=0; i<(10-strlen(tempInput)); i++)
				  {
					tempLabel[3+i] = '0';
				  }
				  for(int i=(13-strlen(tempInput)); i<11; i++)
				  {
					tempLabel[i] = tempInput[i-(11-strlen(tempInput))];
				  }
				  tempLabel[11] = '\0';
				  g_instList->getListEnd().addOperand(tempLabel);
				  g_instList->addCubojdumpLabel(tempLabel);}
#line 1831 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 132: /* modifier: opTypes  */
#line 280 "sass.y"
                                { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addTypeModifier((yyvsp[0].string_value));}
#line 1837 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 133: /* modifier: DOTBEXT  */
#line 281 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".bext"); }
#line 1843 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 134: /* modifier: DOTS  */
#line 282 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".s"); }
#line 1849 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 135: /* modifier: DOTSFU  */
#line 283 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".sfu"); }
#line 1855 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 136: /* modifier: DOTTRUNC  */
#line 284 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".rz"); }
#line 1861 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 137: /* modifier: DOTCEIL  */
#line 285 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".rp"); }
#line 1867 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 138: /* modifier: DOTFLOOR  */
#line 286 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".rm"); }
#line 1873 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 139: /* modifier: DOTX  */
#line 287 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".x"); }
#line 1879 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 140: /* modifier: DOTE  */
#line 288 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".e"); }
#line 1885 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 141: /* modifier: DOTRED  */
#line 289 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".red"); }
#line 1891 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 142: /* modifier: DOTPOPC  */
#line 290 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".popc"); }
#line 1897 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 143: /* modifier: DOTIR  */
#line 291 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".ir"); }
#line 1903 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 144: /* modifier: DOTUN  */
#line 292 "sass.y"
                                        { /*g_instList->getListEnd().addBaseModifier(".un"); */}
#line 1909 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 145: /* modifier: DOTNODEP  */
#line 293 "sass.y"
                                        { /*g_instList->getListEnd().addBaseModifier(".nodep"); */}
#line 1915 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 146: /* modifier: DOTANY  */
#line 294 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".any"); }
#line 1921 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 147: /* modifier: DOTALL  */
#line 295 "sass.y"
                                        { g_instList->getListEnd().addBaseModifier(".all"); }
#line 1927 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 162: /* $@8: %empty  */
#line 314 "sass.y"
                              { debug_print(" "); }
#line 1933 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 163: /* operandList: operandList $@8 operand  */
#line 314 "sass.y"
                                                                        {}
#line 1939 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 166: /* operand: PIPE registerlocation PIPE  */
#line 320 "sass.y"
                                                { g_instList->getListEnd().addBaseModifier(".abs"); }
#line 1945 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 169: /* operand: memorylocation opTypes  */
#line 323 "sass.y"
                                         { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addTypeModifier((yyvsp[0].string_value));}
#line 1951 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 175: /* registerlocation: REGISTER regMod  */
#line 332 "sass.y"
                                                { debug_print((yyvsp[-1].string_value)); g_instList->addCuobjdumpRegister((yyvsp[-1].string_value));}
#line 1957 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 176: /* registerlocation: OSQBRACKET REGISTER CSQBRACKET  */
#line 333 "sass.y"
                                                                { debug_print((yyvsp[-2].string_value)); debug_print((yyvsp[-1].string_value)); debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpRegister((yyvsp[-1].string_value));}
#line 1963 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 177: /* registerlocation: REGISTERLO  */
#line 334 "sass.y"
                                        { debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpRegister((yyvsp[0].string_value),true);}
#line 1969 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 178: /* registerlocation: REGISTERHI  */
#line 335 "sass.y"
                                        { debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpRegister((yyvsp[0].string_value),true);}
#line 1975 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 179: /* registerlocation: SREGISTER  */
#line 336 "sass.y"
                                                { debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpRegister((yyvsp[0].string_value),false);}
#line 1981 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 180: /* registerlocation: OFFSETREGISTER  */
#line 337 "sass.y"
                                                { debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpRegister((yyvsp[0].string_value));}
#line 1987 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 181: /* registerlocation: PREDREGISTER PREDREGISTER2  */
#line 338 "sass.y"
                                                        { debug_print((yyvsp[-1].string_value)); debug_print(" "); debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpDoublePredReg((yyvsp[-1].string_value), (yyvsp[0].string_value));}
#line 1993 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 182: /* registerlocation: PREDREGISTER REGISTER  */
#line 339 "sass.y"
                                                { debug_print((yyvsp[-1].string_value)); debug_print(" "); debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpDoublePredReg((yyvsp[-1].string_value), (yyvsp[0].string_value));}
#line 1999 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 185: /* memorylocation: SMEMLOCATION  */
#line 348 "sass.y"
                                { debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpMemoryOperand((yyvsp[0].string_value),1);}
#line 2005 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 186: /* memorylocation: ABSSMEMLOCATION  */
#line 349 "sass.y"
                                        {
				debug_print((yyvsp[0].string_value));
				char* input = (yyvsp[0].string_value);
				char* temp = new char[99];
				temp[0] = input[1];
				unsigned i=1;
				while (i < strlen(input)-2) {
					temp[i] = input[i+2];
					i++;
				}
				g_instList->addCuobjdumpMemoryOperand(temp,1);
				g_instList->getListEnd().addBaseModifier(".abs");
			}
#line 2023 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 187: /* memorylocation: GMEMLOCATION  */
#line 362 "sass.y"
                                { debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpMemoryOperand((yyvsp[0].string_value),2);}
#line 2029 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 188: /* memorylocation: CMEMLOCATION  */
#line 363 "sass.y"
                                { debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpMemoryOperand((yyvsp[0].string_value),0);}
#line 2035 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 189: /* memorylocation: LMEMLOCATION  */
#line 364 "sass.y"
                                { debug_print((yyvsp[0].string_value)); g_instList->addCuobjdumpMemoryOperand((yyvsp[0].string_value),3);}
#line 2041 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 190: /* immediateValue: IDENTIFIER  */
#line 367 "sass.y"
                             { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addOperand((yyvsp[0].string_value));}
#line 2047 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 191: /* immediateValue: HEXLITERAL  */
#line 368 "sass.y"
                             { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addOperand((yyvsp[0].string_value));}
#line 2053 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 192: /* extraModifier: EQ  */
#line 371 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2059 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 193: /* extraModifier: EQU  */
#line 372 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2065 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 194: /* extraModifier: GE  */
#line 373 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2071 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 195: /* extraModifier: GEU  */
#line 374 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2077 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 196: /* extraModifier: GT  */
#line 375 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2083 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 197: /* extraModifier: GTU  */
#line 376 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2089 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 198: /* extraModifier: LE  */
#line 377 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2095 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 199: /* extraModifier: LEU  */
#line 378 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2101 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 200: /* extraModifier: LT  */
#line 379 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2107 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 201: /* extraModifier: LTU  */
#line 380 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2113 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 202: /* extraModifier: NE  */
#line 381 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2119 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 203: /* extraModifier: NEU  */
#line 382 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier((yyvsp[0].string_value));}
#line 2125 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 204: /* instructionPredicate: PREDREGISTER3 predicateModifier  */
#line 385 "sass.y"
                                                          {debug_print((yyvsp[-1].string_value)); debug_print((yyvsp[0].string_value));
								g_instList->getListEnd().setPredicate((yyvsp[-1].string_value));
								g_instList->getListEnd().addPredicateModifier((yyvsp[0].string_value));}
#line 2133 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 205: /* instructionPredicate: PREDREGISTER3  */
#line 388 "sass.y"
                                                                {debug_print((yyvsp[0].string_value)); g_instList->getListEnd().setPredicate((yyvsp[0].string_value));}
#line 2139 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 206: /* operandPredicate: PREDREGISTER3 predicateModifier  */
#line 391 "sass.y"
                                                                  {
							debug_print((yyvsp[-1].string_value)); 
							debug_print((yyvsp[0].string_value));
							//g_instList->getListEnd().addOperand($1);
							g_instList->getListEnd().setPredicate((yyvsp[-1].string_value));
							g_instList->getListEnd().addPredicateModifier((yyvsp[0].string_value));
							/*May be the modifier needs to be added too*/
						}
#line 2152 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 207: /* operandPredicate: PREDREGISTER3  */
#line 399 "sass.y"
                                                              {
							debug_print("HELLO: "); 
							debug_print((yyvsp[0].string_value)); 
							g_instList->getListEnd().addOperand((yyvsp[0].string_value));
						}
#line 2162 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 208: /* preOperand: EX2  */
#line 407 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier("ex2");}
#line 2168 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 209: /* preOperand: SIN  */
#line 408 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier("sin");}
#line 2174 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 210: /* preOperand: COS  */
#line 409 "sass.y"
                        { debug_print((yyvsp[0].string_value)); g_instList->getListEnd().addBaseModifier("cos");}
#line 2180 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 211: /* predicateModifier: DOTEQ  */
#line 412 "sass.y"
                                { }
#line 2186 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 212: /* predicateModifier: DOTEQU  */
#line 413 "sass.y"
                                        { }
#line 2192 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 213: /* predicateModifier: DOTFALSE  */
#line 414 "sass.y"
                                        { }
#line 2198 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 214: /* predicateModifier: DOTGE  */
#line 415 "sass.y"
                                { }
#line 2204 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 215: /* predicateModifier: DOTGEU  */
#line 416 "sass.y"
                                        { }
#line 2210 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 216: /* predicateModifier: DOTGT  */
#line 417 "sass.y"
                                { }
#line 2216 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 217: /* predicateModifier: DOTGTU  */
#line 418 "sass.y"
                                        { }
#line 2222 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 218: /* predicateModifier: DOTLE  */
#line 419 "sass.y"
                                { }
#line 2228 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 219: /* predicateModifier: DOTLEU  */
#line 420 "sass.y"
                                        { }
#line 2234 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 220: /* predicateModifier: DOTLT  */
#line 421 "sass.y"
                                { }
#line 2240 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 221: /* predicateModifier: DOTLTU  */
#line 422 "sass.y"
                                        { }
#line 2246 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 222: /* predicateModifier: DOTNE  */
#line 423 "sass.y"
                                { }
#line 2252 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 223: /* predicateModifier: DOTNEU  */
#line 424 "sass.y"
                                        { }
#line 2258 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 224: /* predicateModifier: DOTNSF  */
#line 425 "sass.y"
                                        { }
#line 2264 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 225: /* predicateModifier: DOTSF  */
#line 426 "sass.y"
                                { }
#line 2270 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;

  case 226: /* predicateModifier: DOTCARRY  */
#line 427 "sass.y"
                                        { }
#line 2276 "cuobjdump_to_ptxplus/sass_parser.cc"
    break;


#line 2280 "cuobjdump_to_ptxplus/sass_parser.cc"

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

#line 430 "sass.y"


/*support c++ functions go here*/

void debug_print( const char *s )
{
	// uncomment to debug
	// printf("%s",s);
}
