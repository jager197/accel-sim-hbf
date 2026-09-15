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

#ifndef YY_PTX_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUDA_SIM_PTX_TAB_H_INCLUDED
# define YY_PTX_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUDA_SIM_PTX_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ptx_debug;
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
    STRING = 258,                  /* STRING  */
    OPCODE = 259,                  /* OPCODE  */
    WMMA_DIRECTIVE = 260,          /* WMMA_DIRECTIVE  */
    LAYOUT = 261,                  /* LAYOUT  */
    CONFIGURATION = 262,           /* CONFIGURATION  */
    ALIGN_DIRECTIVE = 263,         /* ALIGN_DIRECTIVE  */
    BRANCHTARGETS_DIRECTIVE = 264, /* BRANCHTARGETS_DIRECTIVE  */
    BYTE_DIRECTIVE = 265,          /* BYTE_DIRECTIVE  */
    CALLPROTOTYPE_DIRECTIVE = 266, /* CALLPROTOTYPE_DIRECTIVE  */
    CALLTARGETS_DIRECTIVE = 267,   /* CALLTARGETS_DIRECTIVE  */
    CONST_DIRECTIVE = 268,         /* CONST_DIRECTIVE  */
    CONSTPTR_DIRECTIVE = 269,      /* CONSTPTR_DIRECTIVE  */
    PTR_DIRECTIVE = 270,           /* PTR_DIRECTIVE  */
    ENTRY_DIRECTIVE = 271,         /* ENTRY_DIRECTIVE  */
    EXTERN_DIRECTIVE = 272,        /* EXTERN_DIRECTIVE  */
    FILE_DIRECTIVE = 273,          /* FILE_DIRECTIVE  */
    FUNC_DIRECTIVE = 274,          /* FUNC_DIRECTIVE  */
    GLOBAL_DIRECTIVE = 275,        /* GLOBAL_DIRECTIVE  */
    LOCAL_DIRECTIVE = 276,         /* LOCAL_DIRECTIVE  */
    LOC_DIRECTIVE = 277,           /* LOC_DIRECTIVE  */
    MAXNCTAPERSM_DIRECTIVE = 278,  /* MAXNCTAPERSM_DIRECTIVE  */
    MAXNNREG_DIRECTIVE = 279,      /* MAXNNREG_DIRECTIVE  */
    MAXNTID_DIRECTIVE = 280,       /* MAXNTID_DIRECTIVE  */
    MINNCTAPERSM_DIRECTIVE = 281,  /* MINNCTAPERSM_DIRECTIVE  */
    PARAM_DIRECTIVE = 282,         /* PARAM_DIRECTIVE  */
    PRAGMA_DIRECTIVE = 283,        /* PRAGMA_DIRECTIVE  */
    REG_DIRECTIVE = 284,           /* REG_DIRECTIVE  */
    REQNTID_DIRECTIVE = 285,       /* REQNTID_DIRECTIVE  */
    SECTION_DIRECTIVE = 286,       /* SECTION_DIRECTIVE  */
    SHARED_DIRECTIVE = 287,        /* SHARED_DIRECTIVE  */
    SREG_DIRECTIVE = 288,          /* SREG_DIRECTIVE  */
    SSTARR_DIRECTIVE = 289,        /* SSTARR_DIRECTIVE  */
    STRUCT_DIRECTIVE = 290,        /* STRUCT_DIRECTIVE  */
    SURF_DIRECTIVE = 291,          /* SURF_DIRECTIVE  */
    TARGET_DIRECTIVE = 292,        /* TARGET_DIRECTIVE  */
    TEX_DIRECTIVE = 293,           /* TEX_DIRECTIVE  */
    UNION_DIRECTIVE = 294,         /* UNION_DIRECTIVE  */
    VERSION_DIRECTIVE = 295,       /* VERSION_DIRECTIVE  */
    ADDRESS_SIZE_DIRECTIVE = 296,  /* ADDRESS_SIZE_DIRECTIVE  */
    VISIBLE_DIRECTIVE = 297,       /* VISIBLE_DIRECTIVE  */
    WEAK_DIRECTIVE = 298,          /* WEAK_DIRECTIVE  */
    IDENTIFIER = 299,              /* IDENTIFIER  */
    INT_OPERAND = 300,             /* INT_OPERAND  */
    FLOAT_OPERAND = 301,           /* FLOAT_OPERAND  */
    DOUBLE_OPERAND = 302,          /* DOUBLE_OPERAND  */
    S8_TYPE = 303,                 /* S8_TYPE  */
    S16_TYPE = 304,                /* S16_TYPE  */
    S32_TYPE = 305,                /* S32_TYPE  */
    S64_TYPE = 306,                /* S64_TYPE  */
    U8_TYPE = 307,                 /* U8_TYPE  */
    U16_TYPE = 308,                /* U16_TYPE  */
    U32_TYPE = 309,                /* U32_TYPE  */
    U64_TYPE = 310,                /* U64_TYPE  */
    F16_TYPE = 311,                /* F16_TYPE  */
    F32_TYPE = 312,                /* F32_TYPE  */
    F64_TYPE = 313,                /* F64_TYPE  */
    FF64_TYPE = 314,               /* FF64_TYPE  */
    B8_TYPE = 315,                 /* B8_TYPE  */
    B16_TYPE = 316,                /* B16_TYPE  */
    B32_TYPE = 317,                /* B32_TYPE  */
    B64_TYPE = 318,                /* B64_TYPE  */
    BB64_TYPE = 319,               /* BB64_TYPE  */
    BB128_TYPE = 320,              /* BB128_TYPE  */
    PRED_TYPE = 321,               /* PRED_TYPE  */
    TEXREF_TYPE = 322,             /* TEXREF_TYPE  */
    SAMPLERREF_TYPE = 323,         /* SAMPLERREF_TYPE  */
    SURFREF_TYPE = 324,            /* SURFREF_TYPE  */
    V2_TYPE = 325,                 /* V2_TYPE  */
    V3_TYPE = 326,                 /* V3_TYPE  */
    V4_TYPE = 327,                 /* V4_TYPE  */
    COMMA = 328,                   /* COMMA  */
    PRED = 329,                    /* PRED  */
    HALF_OPTION = 330,             /* HALF_OPTION  */
    EXTP_OPTION = 331,             /* EXTP_OPTION  */
    EQ_OPTION = 332,               /* EQ_OPTION  */
    NE_OPTION = 333,               /* NE_OPTION  */
    LT_OPTION = 334,               /* LT_OPTION  */
    LE_OPTION = 335,               /* LE_OPTION  */
    GT_OPTION = 336,               /* GT_OPTION  */
    GE_OPTION = 337,               /* GE_OPTION  */
    LO_OPTION = 338,               /* LO_OPTION  */
    LS_OPTION = 339,               /* LS_OPTION  */
    HI_OPTION = 340,               /* HI_OPTION  */
    HS_OPTION = 341,               /* HS_OPTION  */
    EQU_OPTION = 342,              /* EQU_OPTION  */
    NEU_OPTION = 343,              /* NEU_OPTION  */
    LTU_OPTION = 344,              /* LTU_OPTION  */
    LEU_OPTION = 345,              /* LEU_OPTION  */
    GTU_OPTION = 346,              /* GTU_OPTION  */
    GEU_OPTION = 347,              /* GEU_OPTION  */
    NUM_OPTION = 348,              /* NUM_OPTION  */
    NAN_OPTION = 349,              /* NAN_OPTION  */
    CF_OPTION = 350,               /* CF_OPTION  */
    SF_OPTION = 351,               /* SF_OPTION  */
    NSF_OPTION = 352,              /* NSF_OPTION  */
    LEFT_SQUARE_BRACKET = 353,     /* LEFT_SQUARE_BRACKET  */
    RIGHT_SQUARE_BRACKET = 354,    /* RIGHT_SQUARE_BRACKET  */
    WIDE_OPTION = 355,             /* WIDE_OPTION  */
    SPECIAL_REGISTER = 356,        /* SPECIAL_REGISTER  */
    MINUS = 357,                   /* MINUS  */
    PLUS = 358,                    /* PLUS  */
    COLON = 359,                   /* COLON  */
    SEMI_COLON = 360,              /* SEMI_COLON  */
    EXCLAMATION = 361,             /* EXCLAMATION  */
    PIPE = 362,                    /* PIPE  */
    RIGHT_BRACE = 363,             /* RIGHT_BRACE  */
    LEFT_BRACE = 364,              /* LEFT_BRACE  */
    EQUALS = 365,                  /* EQUALS  */
    PERIOD = 366,                  /* PERIOD  */
    BACKSLASH = 367,               /* BACKSLASH  */
    DIMENSION_MODIFIER = 368,      /* DIMENSION_MODIFIER  */
    RN_OPTION = 369,               /* RN_OPTION  */
    RZ_OPTION = 370,               /* RZ_OPTION  */
    RM_OPTION = 371,               /* RM_OPTION  */
    RP_OPTION = 372,               /* RP_OPTION  */
    RNI_OPTION = 373,              /* RNI_OPTION  */
    RZI_OPTION = 374,              /* RZI_OPTION  */
    RMI_OPTION = 375,              /* RMI_OPTION  */
    RPI_OPTION = 376,              /* RPI_OPTION  */
    UNI_OPTION = 377,              /* UNI_OPTION  */
    GEOM_MODIFIER_1D = 378,        /* GEOM_MODIFIER_1D  */
    GEOM_MODIFIER_2D = 379,        /* GEOM_MODIFIER_2D  */
    GEOM_MODIFIER_3D = 380,        /* GEOM_MODIFIER_3D  */
    SAT_OPTION = 381,              /* SAT_OPTION  */
    FTZ_OPTION = 382,              /* FTZ_OPTION  */
    NEG_OPTION = 383,              /* NEG_OPTION  */
    SYNC_OPTION = 384,             /* SYNC_OPTION  */
    RED_OPTION = 385,              /* RED_OPTION  */
    ARRIVE_OPTION = 386,           /* ARRIVE_OPTION  */
    ATOMIC_POPC = 387,             /* ATOMIC_POPC  */
    ATOMIC_AND = 388,              /* ATOMIC_AND  */
    ATOMIC_OR = 389,               /* ATOMIC_OR  */
    ATOMIC_XOR = 390,              /* ATOMIC_XOR  */
    ATOMIC_CAS = 391,              /* ATOMIC_CAS  */
    ATOMIC_EXCH = 392,             /* ATOMIC_EXCH  */
    ATOMIC_ADD = 393,              /* ATOMIC_ADD  */
    ATOMIC_INC = 394,              /* ATOMIC_INC  */
    ATOMIC_DEC = 395,              /* ATOMIC_DEC  */
    ATOMIC_MIN = 396,              /* ATOMIC_MIN  */
    ATOMIC_MAX = 397,              /* ATOMIC_MAX  */
    LEFT_ANGLE_BRACKET = 398,      /* LEFT_ANGLE_BRACKET  */
    RIGHT_ANGLE_BRACKET = 399,     /* RIGHT_ANGLE_BRACKET  */
    LEFT_PAREN = 400,              /* LEFT_PAREN  */
    RIGHT_PAREN = 401,             /* RIGHT_PAREN  */
    APPROX_OPTION = 402,           /* APPROX_OPTION  */
    FULL_OPTION = 403,             /* FULL_OPTION  */
    ANY_OPTION = 404,              /* ANY_OPTION  */
    ALL_OPTION = 405,              /* ALL_OPTION  */
    BALLOT_OPTION = 406,           /* BALLOT_OPTION  */
    GLOBAL_OPTION = 407,           /* GLOBAL_OPTION  */
    CTA_OPTION = 408,              /* CTA_OPTION  */
    SYS_OPTION = 409,              /* SYS_OPTION  */
    EXIT_OPTION = 410,             /* EXIT_OPTION  */
    ABS_OPTION = 411,              /* ABS_OPTION  */
    TO_OPTION = 412,               /* TO_OPTION  */
    CA_OPTION = 413,               /* CA_OPTION  */
    CG_OPTION = 414,               /* CG_OPTION  */
    CS_OPTION = 415,               /* CS_OPTION  */
    LU_OPTION = 416,               /* LU_OPTION  */
    CV_OPTION = 417,               /* CV_OPTION  */
    WB_OPTION = 418,               /* WB_OPTION  */
    WT_OPTION = 419,               /* WT_OPTION  */
    NC_OPTION = 420,               /* NC_OPTION  */
    UP_OPTION = 421,               /* UP_OPTION  */
    DOWN_OPTION = 422,             /* DOWN_OPTION  */
    BFLY_OPTION = 423,             /* BFLY_OPTION  */
    IDX_OPTION = 424,              /* IDX_OPTION  */
    PRMT_F4E_MODE = 425,           /* PRMT_F4E_MODE  */
    PRMT_B4E_MODE = 426,           /* PRMT_B4E_MODE  */
    PRMT_RC8_MODE = 427,           /* PRMT_RC8_MODE  */
    PRMT_RC16_MODE = 428,          /* PRMT_RC16_MODE  */
    PRMT_ECL_MODE = 429,           /* PRMT_ECL_MODE  */
    PRMT_ECR_MODE = 430,           /* PRMT_ECR_MODE  */
    WRAP_OPTION = 431,             /* WRAP_OPTION  */
    CLAMP_OPTION = 432,            /* CLAMP_OPTION  */
    LEFT_OPTION = 433,             /* LEFT_OPTION  */
    RIGHT_OPTION = 434             /* RIGHT_OPTION  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 42 "ptx.y"

  double double_value;
  float  float_value;
  int    int_value;
  char * string_value;
  void * ptr_value;

#line 251 "cuda-sim/ptx.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int ptx_parse (yyscan_t scanner, ptx_recognizer* recognizer);


#endif /* !YY_PTX_INSPIRE_QB_DEV_PROJECT_INTEGRATED_TRAINING_AND_PROMOTION_SYSTEM_LIYAQI_253108120131_HBF_SIM_ACCEL_SIM_HBF_GPU_SIMULATOR_GPGPU_SIM_BUILD_GCC_13_3_0_CUDA_12090_RELEASE_CUDA_SIM_PTX_TAB_H_INCLUDED  */
