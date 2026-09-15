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

#ifndef YY_SASS_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUOBJDUMP_TO_PTXPLUS_SASS_PARSER_HH_INCLUDED
# define YY_SASS_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUOBJDUMP_TO_PTXPLUS_SASS_PARSER_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int sass_debug;
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
    BAR = 258,                     /* BAR  */
    ADA = 259,                     /* ADA  */
    AND = 260,                     /* AND  */
    ANDS = 261,                    /* ANDS  */
    BRA = 262,                     /* BRA  */
    BRX = 263,                     /* BRX  */
    CAL = 264,                     /* CAL  */
    COS = 265,                     /* COS  */
    DADD = 266,                    /* DADD  */
    DMIN = 267,                    /* DMIN  */
    DMAX = 268,                    /* DMAX  */
    DFMA = 269,                    /* DFMA  */
    DMUL = 270,                    /* DMUL  */
    EX2 = 271,                     /* EX2  */
    F2F = 272,                     /* F2F  */
    F2I = 273,                     /* F2I  */
    FADD = 274,                    /* FADD  */
    FADD32 = 275,                  /* FADD32  */
    FADD32I = 276,                 /* FADD32I  */
    FMAD = 277,                    /* FMAD  */
    FMAD32I = 278,                 /* FMAD32I  */
    FMUL = 279,                    /* FMUL  */
    FMUL32 = 280,                  /* FMUL32  */
    FMUL32I = 281,                 /* FMUL32I  */
    FSET = 282,                    /* FSET  */
    DSET = 283,                    /* DSET  */
    G2R = 284,                     /* G2R  */
    GLD = 285,                     /* GLD  */
    GST = 286,                     /* GST  */
    I2F = 287,                     /* I2F  */
    I2I = 288,                     /* I2I  */
    IADD = 289,                    /* IADD  */
    IADD32 = 290,                  /* IADD32  */
    IADD32I = 291,                 /* IADD32I  */
    IMAD = 292,                    /* IMAD  */
    ISAD = 293,                    /* ISAD  */
    IMAD24 = 294,                  /* IMAD24  */
    IMAD32I = 295,                 /* IMAD32I  */
    IMAD32 = 296,                  /* IMAD32  */
    IADDCARRY = 297,               /* IADDCARRY  */
    IMUL = 298,                    /* IMUL  */
    IMUL24 = 299,                  /* IMUL24  */
    IMUL24H = 300,                 /* IMUL24H  */
    IMULS24 = 301,                 /* IMULS24  */
    IMUL32 = 302,                  /* IMUL32  */
    IMUL32S24 = 303,               /* IMUL32S24  */
    IMUL32U24 = 304,               /* IMUL32U24  */
    IMUL32I = 305,                 /* IMUL32I  */
    IMUL32I24 = 306,               /* IMUL32I24  */
    IMUL32IS24 = 307,              /* IMUL32IS24  */
    ISET = 308,                    /* ISET  */
    LG2 = 309,                     /* LG2  */
    LLD = 310,                     /* LLD  */
    LST = 311,                     /* LST  */
    MOV = 312,                     /* MOV  */
    MOV32 = 313,                   /* MOV32  */
    MVC = 314,                     /* MVC  */
    MVI = 315,                     /* MVI  */
    NOP = 316,                     /* NOP  */
    NOT = 317,                     /* NOT  */
    NOTS = 318,                    /* NOTS  */
    OR = 319,                      /* OR  */
    ORS = 320,                     /* ORS  */
    R2A = 321,                     /* R2A  */
    R2G = 322,                     /* R2G  */
    R2GU16U8 = 323,                /* R2GU16U8  */
    RCP = 324,                     /* RCP  */
    RCP32 = 325,                   /* RCP32  */
    RET = 326,                     /* RET  */
    RRO = 327,                     /* RRO  */
    RSQ = 328,                     /* RSQ  */
    SIN = 329,                     /* SIN  */
    SHL = 330,                     /* SHL  */
    SHR = 331,                     /* SHR  */
    SSY = 332,                     /* SSY  */
    XOR = 333,                     /* XOR  */
    XORS = 334,                    /* XORS  */
    S2R = 335,                     /* S2R  */
    SASS_LD = 336,                 /* SASS_LD  */
    STS = 337,                     /* STS  */
    LDS = 338,                     /* LDS  */
    SASS_ST = 339,                 /* SASS_ST  */
    IMIN = 340,                    /* IMIN  */
    IMAX = 341,                    /* IMAX  */
    A2R = 342,                     /* A2R  */
    FMAX = 343,                    /* FMAX  */
    FMIN = 344,                    /* FMIN  */
    TEX = 345,                     /* TEX  */
    TEX32 = 346,                   /* TEX32  */
    C2R = 347,                     /* C2R  */
    EXIT = 348,                    /* EXIT  */
    GRED = 349,                    /* GRED  */
    PBK = 350,                     /* PBK  */
    BRK = 351,                     /* BRK  */
    R2C = 352,                     /* R2C  */
    GATOM = 353,                   /* GATOM  */
    VOTE = 354,                    /* VOTE  */
    EQ = 355,                      /* EQ  */
    EQU = 356,                     /* EQU  */
    GE = 357,                      /* GE  */
    GEU = 358,                     /* GEU  */
    GT = 359,                      /* GT  */
    GTU = 360,                     /* GTU  */
    LE = 361,                      /* LE  */
    LEU = 362,                     /* LEU  */
    LT = 363,                      /* LT  */
    LTU = 364,                     /* LTU  */
    NE = 365,                      /* NE  */
    NEU = 366,                     /* NEU  */
    DOTBEXT = 367,                 /* DOTBEXT  */
    DOTS = 368,                    /* DOTS  */
    DOTSFU = 369,                  /* DOTSFU  */
    DOTTRUNC = 370,                /* DOTTRUNC  */
    DOTCEIL = 371,                 /* DOTCEIL  */
    DOTFLOOR = 372,                /* DOTFLOOR  */
    DOTIR = 373,                   /* DOTIR  */
    DOTUN = 374,                   /* DOTUN  */
    DOTNODEP = 375,                /* DOTNODEP  */
    DOTSAT = 376,                  /* DOTSAT  */
    DOTANY = 377,                  /* DOTANY  */
    DOTALL = 378,                  /* DOTALL  */
    DOTF16 = 379,                  /* DOTF16  */
    DOTF32 = 380,                  /* DOTF32  */
    DOTF64 = 381,                  /* DOTF64  */
    DOTS8 = 382,                   /* DOTS8  */
    DOTS16 = 383,                  /* DOTS16  */
    DOTS32 = 384,                  /* DOTS32  */
    DOTS64 = 385,                  /* DOTS64  */
    DOTS128 = 386,                 /* DOTS128  */
    DOTU8 = 387,                   /* DOTU8  */
    DOTU16 = 388,                  /* DOTU16  */
    DOTU32 = 389,                  /* DOTU32  */
    DOTU24 = 390,                  /* DOTU24  */
    DOTU64 = 391,                  /* DOTU64  */
    DOTHI = 392,                   /* DOTHI  */
    DOTNOINC = 393,                /* DOTNOINC  */
    DOTEQ = 394,                   /* DOTEQ  */
    DOTEQU = 395,                  /* DOTEQU  */
    DOTFALSE = 396,                /* DOTFALSE  */
    DOTGE = 397,                   /* DOTGE  */
    DOTGEU = 398,                  /* DOTGEU  */
    DOTGT = 399,                   /* DOTGT  */
    DOTGTU = 400,                  /* DOTGTU  */
    DOTLE = 401,                   /* DOTLE  */
    DOTLEU = 402,                  /* DOTLEU  */
    DOTLT = 403,                   /* DOTLT  */
    DOTLTU = 404,                  /* DOTLTU  */
    DOTNE = 405,                   /* DOTNE  */
    DOTNEU = 406,                  /* DOTNEU  */
    DOTNSF = 407,                  /* DOTNSF  */
    DOTSF = 408,                   /* DOTSF  */
    DOTCARRY = 409,                /* DOTCARRY  */
    DOTCC = 410,                   /* DOTCC  */
    DOTX = 411,                    /* DOTX  */
    DOTE = 412,                    /* DOTE  */
    DOTRED = 413,                  /* DOTRED  */
    DOTPOPC = 414,                 /* DOTPOPC  */
    REGISTER = 415,                /* REGISTER  */
    REGISTERLO = 416,              /* REGISTERLO  */
    REGISTERHI = 417,              /* REGISTERHI  */
    OFFSETREGISTER = 418,          /* OFFSETREGISTER  */
    PREDREGISTER = 419,            /* PREDREGISTER  */
    PREDREGISTER2 = 420,           /* PREDREGISTER2  */
    PREDREGISTER3 = 421,           /* PREDREGISTER3  */
    SREGISTER = 422,               /* SREGISTER  */
    VERSIONHEADER = 423,           /* VERSIONHEADER  */
    FUNCTIONHEADER = 424,          /* FUNCTIONHEADER  */
    SMEMLOCATION = 425,            /* SMEMLOCATION  */
    ABSSMEMLOCATION = 426,         /* ABSSMEMLOCATION  */
    GMEMLOCATION = 427,            /* GMEMLOCATION  */
    CMEMLOCATION = 428,            /* CMEMLOCATION  */
    LMEMLOCATION = 429,            /* LMEMLOCATION  */
    IDENTIFIER = 430,              /* IDENTIFIER  */
    HEXLITERAL = 431,              /* HEXLITERAL  */
    LEFTBRACKET = 432,             /* LEFTBRACKET  */
    RIGHTBRACKET = 433,            /* RIGHTBRACKET  */
    PIPE = 434,                    /* PIPE  */
    TILDE = 435,                   /* TILDE  */
    NEWLINE = 436,                 /* NEWLINE  */
    SEMICOLON = 437,               /* SEMICOLON  */
    LABEL = 438,                   /* LABEL  */
    LABELSTART = 439,              /* LABELSTART  */
    LABELEND = 440,                /* LABELEND  */
    PTXHEADER = 441,               /* PTXHEADER  */
    ELFHEADER = 442,               /* ELFHEADER  */
    INFOARCHVERSION = 443,         /* INFOARCHVERSION  */
    INFOCODEVERSION_HEADER = 444,  /* INFOCODEVERSION_HEADER  */
    INFOCODEVERSION = 445,         /* INFOCODEVERSION  */
    INFOPRODUCER = 446,            /* INFOPRODUCER  */
    INFOHOST = 447,                /* INFOHOST  */
    INFOCOMPILESIZE_HEADER = 448,  /* INFOCOMPILESIZE_HEADER  */
    INFOCOMPILESIZE = 449,         /* INFOCOMPILESIZE  */
    INFOIDENTIFIER = 450,          /* INFOIDENTIFIER  */
    DOT = 451,                     /* DOT  */
    INSTHEX = 452,                 /* INSTHEX  */
    OSQBRACKET = 453,              /* OSQBRACKET  */
    CSQBRACKET = 454               /* CSQBRACKET  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 42 "sass.y"

  double double_value;
  float  float_value;
  int    int_value;
  char * string_value;
  void * ptr_value;

#line 271 "cuobjdump_to_ptxplus/sass_parser.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE sass_lval;


int sass_parse (void);


#endif /* !YY_SASS_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUOBJDUMP_TO_PTXPLUS_SASS_PARSER_HH_INCLUDED  */
