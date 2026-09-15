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
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         ptx_parse
#define yylex           ptx_lex
#define yyerror         ptx_error
#define yydebug         ptx_debug
#define yynerrs         ptx_nerrs

/* First part of user prologue.  */
#line 30 "ptx.y"

typedef void * yyscan_t;
class ptx_recognizer;
#include "../../libcuda/gpgpu_context.h"

#line 82 "cuda-sim/ptx.tab.c"

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

#include "ptx.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_STRING = 3,                     /* STRING  */
  YYSYMBOL_OPCODE = 4,                     /* OPCODE  */
  YYSYMBOL_WMMA_DIRECTIVE = 5,             /* WMMA_DIRECTIVE  */
  YYSYMBOL_LAYOUT = 6,                     /* LAYOUT  */
  YYSYMBOL_CONFIGURATION = 7,              /* CONFIGURATION  */
  YYSYMBOL_ALIGN_DIRECTIVE = 8,            /* ALIGN_DIRECTIVE  */
  YYSYMBOL_BRANCHTARGETS_DIRECTIVE = 9,    /* BRANCHTARGETS_DIRECTIVE  */
  YYSYMBOL_BYTE_DIRECTIVE = 10,            /* BYTE_DIRECTIVE  */
  YYSYMBOL_CALLPROTOTYPE_DIRECTIVE = 11,   /* CALLPROTOTYPE_DIRECTIVE  */
  YYSYMBOL_CALLTARGETS_DIRECTIVE = 12,     /* CALLTARGETS_DIRECTIVE  */
  YYSYMBOL_CONST_DIRECTIVE = 13,           /* CONST_DIRECTIVE  */
  YYSYMBOL_CONSTPTR_DIRECTIVE = 14,        /* CONSTPTR_DIRECTIVE  */
  YYSYMBOL_PTR_DIRECTIVE = 15,             /* PTR_DIRECTIVE  */
  YYSYMBOL_ENTRY_DIRECTIVE = 16,           /* ENTRY_DIRECTIVE  */
  YYSYMBOL_EXTERN_DIRECTIVE = 17,          /* EXTERN_DIRECTIVE  */
  YYSYMBOL_FILE_DIRECTIVE = 18,            /* FILE_DIRECTIVE  */
  YYSYMBOL_FUNC_DIRECTIVE = 19,            /* FUNC_DIRECTIVE  */
  YYSYMBOL_GLOBAL_DIRECTIVE = 20,          /* GLOBAL_DIRECTIVE  */
  YYSYMBOL_LOCAL_DIRECTIVE = 21,           /* LOCAL_DIRECTIVE  */
  YYSYMBOL_LOC_DIRECTIVE = 22,             /* LOC_DIRECTIVE  */
  YYSYMBOL_MAXNCTAPERSM_DIRECTIVE = 23,    /* MAXNCTAPERSM_DIRECTIVE  */
  YYSYMBOL_MAXNNREG_DIRECTIVE = 24,        /* MAXNNREG_DIRECTIVE  */
  YYSYMBOL_MAXNTID_DIRECTIVE = 25,         /* MAXNTID_DIRECTIVE  */
  YYSYMBOL_MINNCTAPERSM_DIRECTIVE = 26,    /* MINNCTAPERSM_DIRECTIVE  */
  YYSYMBOL_PARAM_DIRECTIVE = 27,           /* PARAM_DIRECTIVE  */
  YYSYMBOL_PRAGMA_DIRECTIVE = 28,          /* PRAGMA_DIRECTIVE  */
  YYSYMBOL_REG_DIRECTIVE = 29,             /* REG_DIRECTIVE  */
  YYSYMBOL_REQNTID_DIRECTIVE = 30,         /* REQNTID_DIRECTIVE  */
  YYSYMBOL_SECTION_DIRECTIVE = 31,         /* SECTION_DIRECTIVE  */
  YYSYMBOL_SHARED_DIRECTIVE = 32,          /* SHARED_DIRECTIVE  */
  YYSYMBOL_SREG_DIRECTIVE = 33,            /* SREG_DIRECTIVE  */
  YYSYMBOL_SSTARR_DIRECTIVE = 34,          /* SSTARR_DIRECTIVE  */
  YYSYMBOL_STRUCT_DIRECTIVE = 35,          /* STRUCT_DIRECTIVE  */
  YYSYMBOL_SURF_DIRECTIVE = 36,            /* SURF_DIRECTIVE  */
  YYSYMBOL_TARGET_DIRECTIVE = 37,          /* TARGET_DIRECTIVE  */
  YYSYMBOL_TEX_DIRECTIVE = 38,             /* TEX_DIRECTIVE  */
  YYSYMBOL_UNION_DIRECTIVE = 39,           /* UNION_DIRECTIVE  */
  YYSYMBOL_VERSION_DIRECTIVE = 40,         /* VERSION_DIRECTIVE  */
  YYSYMBOL_ADDRESS_SIZE_DIRECTIVE = 41,    /* ADDRESS_SIZE_DIRECTIVE  */
  YYSYMBOL_VISIBLE_DIRECTIVE = 42,         /* VISIBLE_DIRECTIVE  */
  YYSYMBOL_WEAK_DIRECTIVE = 43,            /* WEAK_DIRECTIVE  */
  YYSYMBOL_IDENTIFIER = 44,                /* IDENTIFIER  */
  YYSYMBOL_INT_OPERAND = 45,               /* INT_OPERAND  */
  YYSYMBOL_FLOAT_OPERAND = 46,             /* FLOAT_OPERAND  */
  YYSYMBOL_DOUBLE_OPERAND = 47,            /* DOUBLE_OPERAND  */
  YYSYMBOL_S8_TYPE = 48,                   /* S8_TYPE  */
  YYSYMBOL_S16_TYPE = 49,                  /* S16_TYPE  */
  YYSYMBOL_S32_TYPE = 50,                  /* S32_TYPE  */
  YYSYMBOL_S64_TYPE = 51,                  /* S64_TYPE  */
  YYSYMBOL_U8_TYPE = 52,                   /* U8_TYPE  */
  YYSYMBOL_U16_TYPE = 53,                  /* U16_TYPE  */
  YYSYMBOL_U32_TYPE = 54,                  /* U32_TYPE  */
  YYSYMBOL_U64_TYPE = 55,                  /* U64_TYPE  */
  YYSYMBOL_F16_TYPE = 56,                  /* F16_TYPE  */
  YYSYMBOL_F32_TYPE = 57,                  /* F32_TYPE  */
  YYSYMBOL_F64_TYPE = 58,                  /* F64_TYPE  */
  YYSYMBOL_FF64_TYPE = 59,                 /* FF64_TYPE  */
  YYSYMBOL_B8_TYPE = 60,                   /* B8_TYPE  */
  YYSYMBOL_B16_TYPE = 61,                  /* B16_TYPE  */
  YYSYMBOL_B32_TYPE = 62,                  /* B32_TYPE  */
  YYSYMBOL_B64_TYPE = 63,                  /* B64_TYPE  */
  YYSYMBOL_BB64_TYPE = 64,                 /* BB64_TYPE  */
  YYSYMBOL_BB128_TYPE = 65,                /* BB128_TYPE  */
  YYSYMBOL_PRED_TYPE = 66,                 /* PRED_TYPE  */
  YYSYMBOL_TEXREF_TYPE = 67,               /* TEXREF_TYPE  */
  YYSYMBOL_SAMPLERREF_TYPE = 68,           /* SAMPLERREF_TYPE  */
  YYSYMBOL_SURFREF_TYPE = 69,              /* SURFREF_TYPE  */
  YYSYMBOL_V2_TYPE = 70,                   /* V2_TYPE  */
  YYSYMBOL_V3_TYPE = 71,                   /* V3_TYPE  */
  YYSYMBOL_V4_TYPE = 72,                   /* V4_TYPE  */
  YYSYMBOL_COMMA = 73,                     /* COMMA  */
  YYSYMBOL_PRED = 74,                      /* PRED  */
  YYSYMBOL_HALF_OPTION = 75,               /* HALF_OPTION  */
  YYSYMBOL_EXTP_OPTION = 76,               /* EXTP_OPTION  */
  YYSYMBOL_EQ_OPTION = 77,                 /* EQ_OPTION  */
  YYSYMBOL_NE_OPTION = 78,                 /* NE_OPTION  */
  YYSYMBOL_LT_OPTION = 79,                 /* LT_OPTION  */
  YYSYMBOL_LE_OPTION = 80,                 /* LE_OPTION  */
  YYSYMBOL_GT_OPTION = 81,                 /* GT_OPTION  */
  YYSYMBOL_GE_OPTION = 82,                 /* GE_OPTION  */
  YYSYMBOL_LO_OPTION = 83,                 /* LO_OPTION  */
  YYSYMBOL_LS_OPTION = 84,                 /* LS_OPTION  */
  YYSYMBOL_HI_OPTION = 85,                 /* HI_OPTION  */
  YYSYMBOL_HS_OPTION = 86,                 /* HS_OPTION  */
  YYSYMBOL_EQU_OPTION = 87,                /* EQU_OPTION  */
  YYSYMBOL_NEU_OPTION = 88,                /* NEU_OPTION  */
  YYSYMBOL_LTU_OPTION = 89,                /* LTU_OPTION  */
  YYSYMBOL_LEU_OPTION = 90,                /* LEU_OPTION  */
  YYSYMBOL_GTU_OPTION = 91,                /* GTU_OPTION  */
  YYSYMBOL_GEU_OPTION = 92,                /* GEU_OPTION  */
  YYSYMBOL_NUM_OPTION = 93,                /* NUM_OPTION  */
  YYSYMBOL_NAN_OPTION = 94,                /* NAN_OPTION  */
  YYSYMBOL_CF_OPTION = 95,                 /* CF_OPTION  */
  YYSYMBOL_SF_OPTION = 96,                 /* SF_OPTION  */
  YYSYMBOL_NSF_OPTION = 97,                /* NSF_OPTION  */
  YYSYMBOL_LEFT_SQUARE_BRACKET = 98,       /* LEFT_SQUARE_BRACKET  */
  YYSYMBOL_RIGHT_SQUARE_BRACKET = 99,      /* RIGHT_SQUARE_BRACKET  */
  YYSYMBOL_WIDE_OPTION = 100,              /* WIDE_OPTION  */
  YYSYMBOL_SPECIAL_REGISTER = 101,         /* SPECIAL_REGISTER  */
  YYSYMBOL_MINUS = 102,                    /* MINUS  */
  YYSYMBOL_PLUS = 103,                     /* PLUS  */
  YYSYMBOL_COLON = 104,                    /* COLON  */
  YYSYMBOL_SEMI_COLON = 105,               /* SEMI_COLON  */
  YYSYMBOL_EXCLAMATION = 106,              /* EXCLAMATION  */
  YYSYMBOL_PIPE = 107,                     /* PIPE  */
  YYSYMBOL_RIGHT_BRACE = 108,              /* RIGHT_BRACE  */
  YYSYMBOL_LEFT_BRACE = 109,               /* LEFT_BRACE  */
  YYSYMBOL_EQUALS = 110,                   /* EQUALS  */
  YYSYMBOL_PERIOD = 111,                   /* PERIOD  */
  YYSYMBOL_BACKSLASH = 112,                /* BACKSLASH  */
  YYSYMBOL_DIMENSION_MODIFIER = 113,       /* DIMENSION_MODIFIER  */
  YYSYMBOL_RN_OPTION = 114,                /* RN_OPTION  */
  YYSYMBOL_RZ_OPTION = 115,                /* RZ_OPTION  */
  YYSYMBOL_RM_OPTION = 116,                /* RM_OPTION  */
  YYSYMBOL_RP_OPTION = 117,                /* RP_OPTION  */
  YYSYMBOL_RNI_OPTION = 118,               /* RNI_OPTION  */
  YYSYMBOL_RZI_OPTION = 119,               /* RZI_OPTION  */
  YYSYMBOL_RMI_OPTION = 120,               /* RMI_OPTION  */
  YYSYMBOL_RPI_OPTION = 121,               /* RPI_OPTION  */
  YYSYMBOL_UNI_OPTION = 122,               /* UNI_OPTION  */
  YYSYMBOL_GEOM_MODIFIER_1D = 123,         /* GEOM_MODIFIER_1D  */
  YYSYMBOL_GEOM_MODIFIER_2D = 124,         /* GEOM_MODIFIER_2D  */
  YYSYMBOL_GEOM_MODIFIER_3D = 125,         /* GEOM_MODIFIER_3D  */
  YYSYMBOL_SAT_OPTION = 126,               /* SAT_OPTION  */
  YYSYMBOL_FTZ_OPTION = 127,               /* FTZ_OPTION  */
  YYSYMBOL_NEG_OPTION = 128,               /* NEG_OPTION  */
  YYSYMBOL_SYNC_OPTION = 129,              /* SYNC_OPTION  */
  YYSYMBOL_RED_OPTION = 130,               /* RED_OPTION  */
  YYSYMBOL_ARRIVE_OPTION = 131,            /* ARRIVE_OPTION  */
  YYSYMBOL_ATOMIC_POPC = 132,              /* ATOMIC_POPC  */
  YYSYMBOL_ATOMIC_AND = 133,               /* ATOMIC_AND  */
  YYSYMBOL_ATOMIC_OR = 134,                /* ATOMIC_OR  */
  YYSYMBOL_ATOMIC_XOR = 135,               /* ATOMIC_XOR  */
  YYSYMBOL_ATOMIC_CAS = 136,               /* ATOMIC_CAS  */
  YYSYMBOL_ATOMIC_EXCH = 137,              /* ATOMIC_EXCH  */
  YYSYMBOL_ATOMIC_ADD = 138,               /* ATOMIC_ADD  */
  YYSYMBOL_ATOMIC_INC = 139,               /* ATOMIC_INC  */
  YYSYMBOL_ATOMIC_DEC = 140,               /* ATOMIC_DEC  */
  YYSYMBOL_ATOMIC_MIN = 141,               /* ATOMIC_MIN  */
  YYSYMBOL_ATOMIC_MAX = 142,               /* ATOMIC_MAX  */
  YYSYMBOL_LEFT_ANGLE_BRACKET = 143,       /* LEFT_ANGLE_BRACKET  */
  YYSYMBOL_RIGHT_ANGLE_BRACKET = 144,      /* RIGHT_ANGLE_BRACKET  */
  YYSYMBOL_LEFT_PAREN = 145,               /* LEFT_PAREN  */
  YYSYMBOL_RIGHT_PAREN = 146,              /* RIGHT_PAREN  */
  YYSYMBOL_APPROX_OPTION = 147,            /* APPROX_OPTION  */
  YYSYMBOL_FULL_OPTION = 148,              /* FULL_OPTION  */
  YYSYMBOL_ANY_OPTION = 149,               /* ANY_OPTION  */
  YYSYMBOL_ALL_OPTION = 150,               /* ALL_OPTION  */
  YYSYMBOL_BALLOT_OPTION = 151,            /* BALLOT_OPTION  */
  YYSYMBOL_GLOBAL_OPTION = 152,            /* GLOBAL_OPTION  */
  YYSYMBOL_CTA_OPTION = 153,               /* CTA_OPTION  */
  YYSYMBOL_SYS_OPTION = 154,               /* SYS_OPTION  */
  YYSYMBOL_EXIT_OPTION = 155,              /* EXIT_OPTION  */
  YYSYMBOL_ABS_OPTION = 156,               /* ABS_OPTION  */
  YYSYMBOL_TO_OPTION = 157,                /* TO_OPTION  */
  YYSYMBOL_CA_OPTION = 158,                /* CA_OPTION  */
  YYSYMBOL_CG_OPTION = 159,                /* CG_OPTION  */
  YYSYMBOL_CS_OPTION = 160,                /* CS_OPTION  */
  YYSYMBOL_LU_OPTION = 161,                /* LU_OPTION  */
  YYSYMBOL_CV_OPTION = 162,                /* CV_OPTION  */
  YYSYMBOL_WB_OPTION = 163,                /* WB_OPTION  */
  YYSYMBOL_WT_OPTION = 164,                /* WT_OPTION  */
  YYSYMBOL_NC_OPTION = 165,                /* NC_OPTION  */
  YYSYMBOL_UP_OPTION = 166,                /* UP_OPTION  */
  YYSYMBOL_DOWN_OPTION = 167,              /* DOWN_OPTION  */
  YYSYMBOL_BFLY_OPTION = 168,              /* BFLY_OPTION  */
  YYSYMBOL_IDX_OPTION = 169,               /* IDX_OPTION  */
  YYSYMBOL_PRMT_F4E_MODE = 170,            /* PRMT_F4E_MODE  */
  YYSYMBOL_PRMT_B4E_MODE = 171,            /* PRMT_B4E_MODE  */
  YYSYMBOL_PRMT_RC8_MODE = 172,            /* PRMT_RC8_MODE  */
  YYSYMBOL_PRMT_RC16_MODE = 173,           /* PRMT_RC16_MODE  */
  YYSYMBOL_PRMT_ECL_MODE = 174,            /* PRMT_ECL_MODE  */
  YYSYMBOL_PRMT_ECR_MODE = 175,            /* PRMT_ECR_MODE  */
  YYSYMBOL_WRAP_OPTION = 176,              /* WRAP_OPTION  */
  YYSYMBOL_CLAMP_OPTION = 177,             /* CLAMP_OPTION  */
  YYSYMBOL_LEFT_OPTION = 178,              /* LEFT_OPTION  */
  YYSYMBOL_RIGHT_OPTION = 179,             /* RIGHT_OPTION  */
  YYSYMBOL_YYACCEPT = 180,                 /* $accept  */
  YYSYMBOL_input = 181,                    /* input  */
  YYSYMBOL_function_defn = 182,            /* function_defn  */
  YYSYMBOL_183_1 = 183,                    /* $@1  */
  YYSYMBOL_184_2 = 184,                    /* $@2  */
  YYSYMBOL_185_3 = 185,                    /* $@3  */
  YYSYMBOL_block_spec = 186,               /* block_spec  */
  YYSYMBOL_block_spec_list = 187,          /* block_spec_list  */
  YYSYMBOL_function_decl = 188,            /* function_decl  */
  YYSYMBOL_189_4 = 189,                    /* $@4  */
  YYSYMBOL_190_5 = 190,                    /* $@5  */
  YYSYMBOL_191_6 = 191,                    /* $@6  */
  YYSYMBOL_function_ident_param = 192,     /* function_ident_param  */
  YYSYMBOL_193_7 = 193,                    /* $@7  */
  YYSYMBOL_194_8 = 194,                    /* $@8  */
  YYSYMBOL_function_decl_header = 195,     /* function_decl_header  */
  YYSYMBOL_param_list = 196,               /* param_list  */
  YYSYMBOL_197_9 = 197,                    /* $@9  */
  YYSYMBOL_param_entry = 198,              /* param_entry  */
  YYSYMBOL_199_10 = 199,                   /* $@10  */
  YYSYMBOL_200_11 = 200,                   /* $@11  */
  YYSYMBOL_ptr_spec = 201,                 /* ptr_spec  */
  YYSYMBOL_ptr_space_spec = 202,           /* ptr_space_spec  */
  YYSYMBOL_ptr_align_spec = 203,           /* ptr_align_spec  */
  YYSYMBOL_statement_block = 204,          /* statement_block  */
  YYSYMBOL_statement_list = 205,           /* statement_list  */
  YYSYMBOL_206_12 = 206,                   /* $@12  */
  YYSYMBOL_207_13 = 207,                   /* $@13  */
  YYSYMBOL_directive_statement = 208,      /* directive_statement  */
  YYSYMBOL_variable_declaration = 209,     /* variable_declaration  */
  YYSYMBOL_variable_spec = 210,            /* variable_spec  */
  YYSYMBOL_identifier_list = 211,          /* identifier_list  */
  YYSYMBOL_identifier_spec = 212,          /* identifier_spec  */
  YYSYMBOL_var_spec_list = 213,            /* var_spec_list  */
  YYSYMBOL_var_spec = 214,                 /* var_spec  */
  YYSYMBOL_align_spec = 215,               /* align_spec  */
  YYSYMBOL_space_spec = 216,               /* space_spec  */
  YYSYMBOL_addressable_spec = 217,         /* addressable_spec  */
  YYSYMBOL_type_spec = 218,                /* type_spec  */
  YYSYMBOL_vector_spec = 219,              /* vector_spec  */
  YYSYMBOL_scalar_type = 220,              /* scalar_type  */
  YYSYMBOL_initializer_list = 221,         /* initializer_list  */
  YYSYMBOL_literal_list = 222,             /* literal_list  */
  YYSYMBOL_prototype_block = 223,          /* prototype_block  */
  YYSYMBOL_prototype_decl = 224,           /* prototype_decl  */
  YYSYMBOL_prototype_call = 225,           /* prototype_call  */
  YYSYMBOL_prototype_param = 226,          /* prototype_param  */
  YYSYMBOL_instruction_statement = 227,    /* instruction_statement  */
  YYSYMBOL_instruction = 228,              /* instruction  */
  YYSYMBOL_229_14 = 229,                   /* $@14  */
  YYSYMBOL_opcode_spec = 230,              /* opcode_spec  */
  YYSYMBOL_231_15 = 231,                   /* $@15  */
  YYSYMBOL_pred_spec = 232,                /* pred_spec  */
  YYSYMBOL_option_list = 233,              /* option_list  */
  YYSYMBOL_option = 234,                   /* option  */
  YYSYMBOL_atomic_operation_spec = 235,    /* atomic_operation_spec  */
  YYSYMBOL_rounding_mode = 236,            /* rounding_mode  */
  YYSYMBOL_floating_point_rounding_mode = 237, /* floating_point_rounding_mode  */
  YYSYMBOL_integer_rounding_mode = 238,    /* integer_rounding_mode  */
  YYSYMBOL_compare_spec = 239,             /* compare_spec  */
  YYSYMBOL_prmt_spec = 240,                /* prmt_spec  */
  YYSYMBOL_wmma_spec = 241,                /* wmma_spec  */
  YYSYMBOL_operand_list = 242,             /* operand_list  */
  YYSYMBOL_operand = 243,                  /* operand  */
  YYSYMBOL_vector_operand = 244,           /* vector_operand  */
  YYSYMBOL_tex_operand = 245,              /* tex_operand  */
  YYSYMBOL_246_16 = 246,                   /* $@16  */
  YYSYMBOL_builtin_operand = 247,          /* builtin_operand  */
  YYSYMBOL_memory_operand = 248,           /* memory_operand  */
  YYSYMBOL_twin_operand = 249,             /* twin_operand  */
  YYSYMBOL_literal_operand = 250,          /* literal_operand  */
  YYSYMBOL_address_expression = 251        /* address_expression  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */
#line 231 "ptx.y"

  	#include "ptx_parser.h"
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	void syntax_not_implemented(yyscan_t yyscanner, ptx_recognizer* recognizer);
	int ptx_lex(YYSTYPE * yylval_param, yyscan_t yyscanner, ptx_recognizer* recognizer);
	int ptx_error( yyscan_t yyscanner, ptx_recognizer* recognizer, const char *s );

#line 377 "cuda-sim/ptx.tab.c"


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
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   651

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  180
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  72
/* YYNRULES -- Number of rules.  */
#define YYNRULES  312
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  464

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   434


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
     175,   176,   177,   178,   179
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   243,   243,   244,   245,   246,   249,   249,   250,   250,
     250,   253,   257,   258,   261,   262,   265,   265,   265,   266,
     266,   267,   270,   270,   270,   271,   274,   275,   276,   277,
     278,   279,   280,   281,   284,   285,   286,   286,   288,   288,
     289,   289,   291,   292,   293,   295,   296,   297,   298,   300,
     302,   304,   305,   306,   307,   308,   309,   309,   310,   310,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   327,   328,   329,   330,   333,   335,   336,   338,
     339,   351,   352,   355,   356,   358,   359,   360,   361,   362,
     363,   366,   368,   369,   370,   373,   374,   375,   376,   377,
     378,   379,   380,   383,   384,   387,   388,   389,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   401,   402,   403,
     404,   405,   406,   407,   408,   409,   410,   411,   412,   413,
     416,   417,   419,   420,   428,   430,   432,   433,   435,   436,
     437,   439,   440,   441,   443,   443,   444,   445,   446,   447,
     450,   450,   451,   453,   454,   455,   456,   457,   458,   459,
     460,   461,   462,   463,   464,   465,   468,   469,   471,   472,
     473,   474,   475,   476,   477,   478,   479,   480,   481,   482,
     483,   484,   485,   486,   487,   488,   489,   490,   491,   492,
     493,   494,   495,   496,   497,   498,   499,   500,   501,   502,
     503,   504,   505,   506,   507,   508,   509,   510,   511,   512,
     513,   514,   515,   516,   517,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   533,   534,   537,   538,
     539,   540,   543,   544,   545,   546,   549,   550,   551,   552,
     553,   554,   555,   556,   557,   558,   559,   560,   561,   562,
     563,   564,   565,   566,   569,   570,   571,   572,   573,   574,
     577,   578,   587,   588,   590,   591,   592,   593,   594,   595,
     596,   597,   598,   599,   600,   601,   602,   603,   604,   605,
     606,   607,   608,   609,   612,   613,   614,   615,   616,   619,
     619,   624,   625,   628,   629,   630,   631,   632,   635,   636,
     637,   638,   639,   640,   641,   644,   645,   646,   649,   650,
     651,   652,   653
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
  "\"end of file\"", "error", "\"invalid token\"", "STRING", "OPCODE",
  "WMMA_DIRECTIVE", "LAYOUT", "CONFIGURATION", "ALIGN_DIRECTIVE",
  "BRANCHTARGETS_DIRECTIVE", "BYTE_DIRECTIVE", "CALLPROTOTYPE_DIRECTIVE",
  "CALLTARGETS_DIRECTIVE", "CONST_DIRECTIVE", "CONSTPTR_DIRECTIVE",
  "PTR_DIRECTIVE", "ENTRY_DIRECTIVE", "EXTERN_DIRECTIVE", "FILE_DIRECTIVE",
  "FUNC_DIRECTIVE", "GLOBAL_DIRECTIVE", "LOCAL_DIRECTIVE", "LOC_DIRECTIVE",
  "MAXNCTAPERSM_DIRECTIVE", "MAXNNREG_DIRECTIVE", "MAXNTID_DIRECTIVE",
  "MINNCTAPERSM_DIRECTIVE", "PARAM_DIRECTIVE", "PRAGMA_DIRECTIVE",
  "REG_DIRECTIVE", "REQNTID_DIRECTIVE", "SECTION_DIRECTIVE",
  "SHARED_DIRECTIVE", "SREG_DIRECTIVE", "SSTARR_DIRECTIVE",
  "STRUCT_DIRECTIVE", "SURF_DIRECTIVE", "TARGET_DIRECTIVE",
  "TEX_DIRECTIVE", "UNION_DIRECTIVE", "VERSION_DIRECTIVE",
  "ADDRESS_SIZE_DIRECTIVE", "VISIBLE_DIRECTIVE", "WEAK_DIRECTIVE",
  "IDENTIFIER", "INT_OPERAND", "FLOAT_OPERAND", "DOUBLE_OPERAND",
  "S8_TYPE", "S16_TYPE", "S32_TYPE", "S64_TYPE", "U8_TYPE", "U16_TYPE",
  "U32_TYPE", "U64_TYPE", "F16_TYPE", "F32_TYPE", "F64_TYPE", "FF64_TYPE",
  "B8_TYPE", "B16_TYPE", "B32_TYPE", "B64_TYPE", "BB64_TYPE", "BB128_TYPE",
  "PRED_TYPE", "TEXREF_TYPE", "SAMPLERREF_TYPE", "SURFREF_TYPE", "V2_TYPE",
  "V3_TYPE", "V4_TYPE", "COMMA", "PRED", "HALF_OPTION", "EXTP_OPTION",
  "EQ_OPTION", "NE_OPTION", "LT_OPTION", "LE_OPTION", "GT_OPTION",
  "GE_OPTION", "LO_OPTION", "LS_OPTION", "HI_OPTION", "HS_OPTION",
  "EQU_OPTION", "NEU_OPTION", "LTU_OPTION", "LEU_OPTION", "GTU_OPTION",
  "GEU_OPTION", "NUM_OPTION", "NAN_OPTION", "CF_OPTION", "SF_OPTION",
  "NSF_OPTION", "LEFT_SQUARE_BRACKET", "RIGHT_SQUARE_BRACKET",
  "WIDE_OPTION", "SPECIAL_REGISTER", "MINUS", "PLUS", "COLON",
  "SEMI_COLON", "EXCLAMATION", "PIPE", "RIGHT_BRACE", "LEFT_BRACE",
  "EQUALS", "PERIOD", "BACKSLASH", "DIMENSION_MODIFIER", "RN_OPTION",
  "RZ_OPTION", "RM_OPTION", "RP_OPTION", "RNI_OPTION", "RZI_OPTION",
  "RMI_OPTION", "RPI_OPTION", "UNI_OPTION", "GEOM_MODIFIER_1D",
  "GEOM_MODIFIER_2D", "GEOM_MODIFIER_3D", "SAT_OPTION", "FTZ_OPTION",
  "NEG_OPTION", "SYNC_OPTION", "RED_OPTION", "ARRIVE_OPTION",
  "ATOMIC_POPC", "ATOMIC_AND", "ATOMIC_OR", "ATOMIC_XOR", "ATOMIC_CAS",
  "ATOMIC_EXCH", "ATOMIC_ADD", "ATOMIC_INC", "ATOMIC_DEC", "ATOMIC_MIN",
  "ATOMIC_MAX", "LEFT_ANGLE_BRACKET", "RIGHT_ANGLE_BRACKET", "LEFT_PAREN",
  "RIGHT_PAREN", "APPROX_OPTION", "FULL_OPTION", "ANY_OPTION",
  "ALL_OPTION", "BALLOT_OPTION", "GLOBAL_OPTION", "CTA_OPTION",
  "SYS_OPTION", "EXIT_OPTION", "ABS_OPTION", "TO_OPTION", "CA_OPTION",
  "CG_OPTION", "CS_OPTION", "LU_OPTION", "CV_OPTION", "WB_OPTION",
  "WT_OPTION", "NC_OPTION", "UP_OPTION", "DOWN_OPTION", "BFLY_OPTION",
  "IDX_OPTION", "PRMT_F4E_MODE", "PRMT_B4E_MODE", "PRMT_RC8_MODE",
  "PRMT_RC16_MODE", "PRMT_ECL_MODE", "PRMT_ECR_MODE", "WRAP_OPTION",
  "CLAMP_OPTION", "LEFT_OPTION", "RIGHT_OPTION", "$accept", "input",
  "function_defn", "$@1", "$@2", "$@3", "block_spec", "block_spec_list",
  "function_decl", "$@4", "$@5", "$@6", "function_ident_param", "$@7",
  "$@8", "function_decl_header", "param_list", "$@9", "param_entry",
  "$@10", "$@11", "ptr_spec", "ptr_space_spec", "ptr_align_spec",
  "statement_block", "statement_list", "$@12", "$@13",
  "directive_statement", "variable_declaration", "variable_spec",
  "identifier_list", "identifier_spec", "var_spec_list", "var_spec",
  "align_spec", "space_spec", "addressable_spec", "type_spec",
  "vector_spec", "scalar_type", "initializer_list", "literal_list",
  "prototype_block", "prototype_decl", "prototype_call", "prototype_param",
  "instruction_statement", "instruction", "$@14", "opcode_spec", "$@15",
  "pred_spec", "option_list", "option", "atomic_operation_spec",
  "rounding_mode", "floating_point_rounding_mode", "integer_rounding_mode",
  "compare_spec", "prmt_spec", "wmma_spec", "operand_list", "operand",
  "vector_operand", "tex_operand", "$@16", "builtin_operand",
  "memory_operand", "twin_operand", "literal_operand",
  "address_expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-317)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-153)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -317,   392,  -317,   -19,  -317,   -14,  -317,    29,    60,  -317,
    -317,  -317,   139,  -317,   190,  -317,  -317,  -317,  -317,  -317,
     152,  -317,   165,   200,    15,   167,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,    -7,   -39,  -317,   136,   254,   526,  -317,  -317,
    -317,  -317,  -317,   581,  -317,  -317,   232,  -317,   310,   281,
     216,   255,   224,  -317,  -317,  -317,  -317,  -317,  -317,   220,
      84,  -317,   286,  -317,    49,   258,   222,  -317,  -317,  -317,
    -317,  -317,   289,   261,   290,  -317,   292,  -317,   461,  -317,
     293,   296,   299,  -317,    84,    11,   192,  -317,     0,   300,
     254,   -34,   273,   304,  -317,   280,   250,   257,    -3,   264,
     206,   220,  -317,  -317,   267,   256,   362,  -317,   294,  -317,
     220,  -317,  -317,  -317,   229,   231,   278,  -317,   235,  -317,
    -317,  -317,  -317,   -34,  -317,  -317,   335,   309,   339,     1,
    -317,   525,   340,   282,  -317,   220,  -317,  -317,   381,  -317,
    -317,  -317,   516,   160,   275,     2,   345,   346,   272,  -317,
     318,  -317,  -317,  -317,  -317,  -317,   288,   349,  -317,   526,
     526,  -317,  -317,  -317,  -317,   295,   109,  -317,  -317,   351,
    -317,   391,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,     1,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,   387,  -317,
     -37,  -317,  -317,  -317,   144,   354,   358,   360,   237,  -317,
     316,  -317,   102,   207,   101,  -317,  -317,  -317,   121,   270,
     262,  -317,   344,   403,   254,   286,    11,  -317,    67,  -317,
    -317,   276,  -317,   277,   350,   383,   -60,  -317,   332,   337,
     338,  -317,   123,   126,  -317,  -317,  -317,   393,  -317,  -317,
    -317,   -56,   341,   422,  -317,  -317,   241,  -317,   394,   423,
     291,   254,  -317,  -317,   -49,  -317,  -317,   463,  -317,   444,
     327,   330,   -36,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
     364,  -317,   122,   411,  -317,   361,   272,  -317,   440,  -317,
    -317,  -317,  -317,   478,  -317,  -317,  -317,  -317,  -317,   227,
     390,   443,   418,   130,   247,   397,   448,  -317,   272,  -317,
    -317,  -317,    11,   456,   462,   464,   395,   272,  -317,  -317,
     196,  -317,  -317,   128,   465,  -317,  -317,  -317,   399,   467,
     469,  -317,  -317,   493,  -317,   400,   444,   504,   404,   129,
     272,   405,   445,   508,   510,  -317,   410,   452,  -317,   415,
     490,  -317,  -317,   492,   522,   523,   497,   466,   528,  -317,
     500,   556,   498,  -317
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,     0,    95,     0,    26,    89,     0,    29,
      96,    97,     0,    98,     0,    92,    99,    93,   100,   101,
       0,   102,     0,     0,    88,    90,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   105,   106,
     107,     4,     5,    21,     3,     0,     0,    76,    83,    87,
      85,    94,    86,     0,   103,    91,     0,    32,     0,     0,
       0,    66,    61,    63,    27,    30,    28,    31,    71,     0,
       0,    16,     0,    60,    79,    72,    77,    89,    88,    90,
      84,   104,     0,    67,     0,    70,     0,    62,    58,     7,
       0,     0,     0,    14,     9,     0,    25,    20,     0,     0,
       0,     0,     0,     0,    69,    64,   150,     0,     0,     0,
      56,     0,    51,    53,     0,   149,     0,    13,     0,    12,
       0,    15,    38,    40,     0,     0,     0,    81,     0,    78,
     305,   306,   307,     0,    73,    74,     0,     0,     0,     0,
     142,   153,     0,     0,    50,     0,    54,    52,     0,    55,
      59,   141,   264,     0,   292,     0,     0,     0,     0,   148,
     262,   270,   272,   269,   267,   268,     0,     0,    10,     0,
       0,    17,    23,    82,    80,     0,     0,   132,    75,     0,
      65,     0,   197,   198,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   178,   228,   229,   230,   231,   232,   233,   234,
     235,   177,   185,   186,   187,   188,   189,   190,   174,   176,
     175,   216,   215,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   191,   192,   179,   180,   181,   182,   183,   184,
     193,   194,   196,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   254,   255,   256,   257,   258,
     259,   211,   212,   213,   214,   170,   168,   151,   166,   195,
     171,   226,   227,   169,   173,   172,   156,   158,   155,   157,
     159,   160,   162,   161,   163,   164,   165,   154,   142,    57,
       0,   134,   274,   276,     0,     0,     0,     0,   308,   312,
       0,   291,   266,     0,     0,   271,   297,   265,     0,     0,
       0,   143,     0,    42,     0,     0,    34,   131,     0,   130,
      68,     0,   167,     0,     0,     0,   308,   305,     0,     0,
       0,   273,   278,   281,   289,   309,   310,     0,   293,   275,
     277,   308,     0,     0,   288,   144,     0,   263,   262,     0,
       0,     0,    41,    18,     0,    35,   133,     0,   260,   138,
       0,     0,     0,   296,   295,   294,   279,   280,   282,   283,
       0,   311,     0,     0,   147,     0,     0,    11,     0,    48,
      45,    46,    47,     0,    44,    39,    36,    24,   261,     0,
       0,     0,     0,   298,     0,     0,     0,   284,     0,   146,
      49,    43,     0,     0,     0,     0,     0,     0,   299,   300,
     301,   304,   290,     0,     0,    37,   140,   139,     0,     0,
       0,   302,   303,     0,   285,     0,   138,     0,     0,     0,
       0,     0,     0,     0,     0,   286,     0,     0,   137,     0,
       0,   145,   135,     0,     0,     0,     0,     0,     0,   136,
       0,     0,     0,   287
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -317,  -317,  -317,  -317,  -317,  -317,   505,  -317,   607,  -317,
    -317,  -317,   285,  -317,  -317,  -317,  -317,  -317,  -316,  -317,
    -317,  -317,  -317,   218,    77,  -317,  -317,  -317,   -78,  -317,
     145,  -317,  -108,  -317,   558,  -317,  -317,  -134,  -132,  -317,
     554,   475,  -317,  -317,  -317,  -317,   188,   506,   499,  -317,
    -317,  -317,  -317,   373,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,  -317,  -125,  -124,  -162,  -317,  -317,  -317,  -133,  -317,
    -107,   323
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,    51,    79,    80,   130,   103,   104,   119,   105,
     325,    82,   107,   135,   326,    53,   364,   412,   134,   179,
     180,   361,   393,   394,    99,   120,   155,   121,    54,    55,
      56,    85,    86,    57,    58,    59,    60,    61,    62,    63,
      64,   144,   186,   157,   158,   301,   400,   123,   124,   383,
     125,   149,   126,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   357,   358,   171,   172,   380,   173,   174,   338,
     175,   310
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     169,   170,   139,   315,   145,   -19,   191,   334,   403,   381,
     365,   140,   141,   142,     4,   275,    -8,   276,    -8,    -8,
     122,    10,    11,   345,   396,   346,    65,   345,    13,   346,
      66,    74,   316,    16,    75,    18,   187,    19,   132,    21,
     133,   151,   156,   372,   319,   136,   312,   347,    67,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,   404,   143,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   425,   397,    78,   137,
     313,   212,    -6,   152,   314,    68,    81,   100,   335,   101,
     102,   167,   140,   141,   142,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   275,   352,   276,   108,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   316,   328,    76,    69,   349,    77,   350,   336,   337,
     141,   142,   109,    70,   353,   406,    71,   339,   160,   313,
     304,   433,   444,   314,   308,   309,   376,   178,   377,   378,
     116,   379,    72,   418,     3,   419,   362,   329,   405,     4,
       5,   366,     6,     7,     8,     9,    10,    11,    12,   354,
     407,   385,   299,    13,    14,    15,   434,   445,    16,    17,
      18,    83,    19,    20,    21,    73,    22,    23,    24,    25,
     153,   351,   309,   395,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,   431,
     118,   432,   367,   368,   424,   162,   140,   141,   142,   413,
     414,   420,   421,   430,  -152,  -152,  -152,  -152,    84,   388,
     162,   140,   141,   142,   389,    92,   162,   140,   141,   142,
     344,   390,   391,    93,   154,   446,   162,   140,   141,   142,
     345,    95,   346,   392,   323,   324,    94,    97,    96,    98,
     106,   110,   111,   112,   113,   114,   115,   -22,   127,   163,
     347,   128,   164,   165,   129,   138,   146,   166,  -152,   147,
     167,  -152,  -152,   148,   163,  -152,  -152,   164,   165,  -152,
     163,   150,   166,   164,   165,   167,   116,   177,   166,    78,
     163,   167,   161,   164,   165,   181,   182,   183,   166,   184,
     188,   167,   189,   190,   297,   300,   298,   384,   311,   317,
     318,   320,     2,   321,   322,  -152,   330,   331,   333,   341,
       3,   168,   342,   327,   343,     4,     5,   356,     6,     7,
       8,     9,    10,    11,    12,   348,   355,   359,   360,    13,
      14,    15,   369,   370,    16,    17,    18,   371,    19,    20,
      21,   373,    22,    23,    24,    25,   374,   375,   381,   304,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,   116,   382,   386,   387,     3,
     398,   399,   401,   167,     4,     5,   402,     6,     7,     8,
       9,    10,    11,    12,   408,   410,   388,   416,    13,    14,
      15,   417,   423,    16,    17,    18,   422,    19,    20,    21,
     426,    22,    23,    24,    25,   117,   427,   409,   428,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,     3,   118,   415,   439,   435,     4,
     437,   429,   438,    87,   436,   440,    10,    11,   442,   443,
     448,   447,   449,    13,   450,    15,   451,   452,    16,    17,
      18,   453,    19,   454,    21,   455,   456,   457,    88,    89,
     458,   459,   460,   461,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,   302,
     462,   303,   286,   287,   288,   289,   463,   290,    52,   131,
     363,   411,   291,   292,   304,    90,   293,    91,   185,   305,
     294,   295,   296,   306,   441,   176,   159,   340,   307,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,   332
};

static const yytype_int16 yycheck[] =
{
     125,   125,   110,   165,   111,    44,     5,    44,    44,    45,
     326,    45,    46,    47,    13,   149,    23,   149,    25,    26,
      98,    20,    21,    83,    73,    85,    45,    83,    27,    85,
      44,    16,   165,    32,    19,    34,   143,    36,    27,    38,
      29,    44,   120,   103,   168,    45,    44,   103,    19,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,   110,   109,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,   412,   146,   105,    99,
      98,   100,   109,   106,   102,    45,   145,    23,   145,    25,
      26,   109,    45,    46,    47,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   278,    44,   278,    98,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   314,    73,    16,    45,    83,    19,    85,    44,    45,
      46,    47,   143,     3,    73,    73,    44,   304,   121,    98,
      98,    73,    73,   102,    44,    45,    83,   130,    85,    83,
       4,    85,    47,    83,     8,    85,   324,   108,   380,    13,
      14,   328,    16,    17,    18,    19,    20,    21,    22,   108,
     108,   356,   155,    27,    28,    29,   108,   108,    32,    33,
      34,   105,    36,    37,    38,    45,    40,    41,    42,    43,
      44,    44,    45,   361,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    83,
      74,    85,     6,     7,   408,    44,    45,    46,    47,    62,
      63,    44,    45,   417,    44,    45,    46,    47,    44,     8,
      44,    45,    46,    47,    13,    73,    44,    45,    46,    47,
      73,    20,    21,     3,   108,   440,    44,    45,    46,    47,
      83,   105,    85,    32,   179,   180,    45,   103,    73,   109,
      44,    73,   110,    44,    73,    45,    44,   145,    45,    98,
     103,    45,   101,   102,    45,    45,    73,   106,    98,    45,
     109,   101,   102,    73,    98,   105,   106,   101,   102,   109,
      98,   104,   106,   101,   102,   109,     4,    73,   106,   105,
      98,   109,   105,   101,   102,   146,   145,    99,   106,   144,
      45,   109,    73,    44,    44,     4,   104,   146,   113,    44,
      44,    73,     0,   105,    45,   145,    45,     6,    11,    45,
       8,   145,    44,   108,    44,    13,    14,   145,    16,    17,
      18,    19,    20,    21,    22,    99,   146,    73,    15,    27,
      28,    29,   145,    73,    32,    33,    34,    44,    36,    37,
      38,    99,    40,    41,    42,    43,    99,    99,    45,    98,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,     4,    44,    73,    45,     8,
       7,    27,   145,   109,    13,    14,   146,    16,    17,    18,
      19,    20,    21,    22,    73,    45,     8,    44,    27,    28,
      29,    73,    44,    32,    33,    34,    99,    36,    37,    38,
      44,    40,    41,    42,    43,    44,    44,   146,    44,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,     8,    74,   146,    44,    73,    13,
      73,   146,    73,    17,   145,   145,    20,    21,    44,   145,
     105,   146,    44,    27,    44,    29,   146,   105,    32,    33,
      34,   146,    36,    73,    38,    73,    44,    44,    42,    43,
      73,   105,    44,    73,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    83,
      44,    85,    77,    78,    79,    80,   108,    82,     1,   104,
     325,   393,    87,    88,    98,    57,    91,    63,   143,   103,
      95,    96,    97,   107,   436,   126,   120,   304,   112,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,   278
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   181,     0,     8,    13,    14,    16,    17,    18,    19,
      20,    21,    22,    27,    28,    29,    32,    33,    34,    36,
      37,    38,    40,    41,    42,    43,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,   182,   188,   195,   208,   209,   210,   213,   214,   215,
     216,   217,   218,   219,   220,    45,    44,    19,    45,    45,
       3,    44,    47,    45,    16,    19,    16,    19,   105,   183,
     184,   145,   191,   105,    44,   211,   212,    17,    42,    43,
     214,   220,    73,     3,    45,   105,    73,   103,   109,   204,
      23,    25,    26,   186,   187,   189,    44,   192,    98,   143,
      73,   110,    44,    73,    45,    44,     4,    44,    74,   188,
     205,   207,   208,   227,   228,   230,   232,    45,    45,    45,
     185,   186,    27,    29,   198,   193,    45,    99,    45,   212,
      45,    46,    47,   109,   221,   250,    73,    45,    73,   231,
     104,    44,   106,    44,   108,   206,   208,   223,   224,   227,
     204,   105,    44,    98,   101,   102,   106,   109,   145,   242,
     243,   244,   245,   247,   248,   250,   228,    73,   204,   199,
     200,   146,   145,    99,   144,   221,   222,   250,    45,    73,
      44,     5,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,   100,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   217,   218,   233,   234,   235,
     236,   237,   238,   239,   240,   241,    77,    78,    79,    80,
      82,    87,    88,    91,    95,    96,    97,    44,   104,   204,
       4,   225,    83,    85,    98,   103,   107,   112,    44,    45,
     251,   113,    44,    98,   102,   244,   248,    44,    44,   243,
      73,   105,    45,   210,   210,   190,   194,   108,    73,   108,
      45,     6,   233,    11,    44,   145,    44,    45,   249,   250,
     251,    45,    44,    44,    73,    83,    85,   103,    99,    83,
      85,    44,    44,    73,   108,   146,   145,   242,   243,    73,
      15,   201,   212,   192,   196,   198,   250,     6,     7,   145,
      73,    44,   103,    99,    99,    99,    83,    85,    83,    85,
     246,    45,    44,   229,   146,   242,    73,    45,     8,    13,
      20,    21,    32,   202,   203,   212,    73,   146,     7,    27,
     226,   145,   146,    44,   110,   244,    73,   108,    73,   146,
      45,   203,   197,    62,    63,   146,    44,    73,    83,    85,
      44,    45,    99,    44,   243,   198,    44,    44,    44,   146,
     243,    83,    85,    73,   108,    73,   145,    73,    73,    44,
     145,   226,    44,   145,    73,   108,   242,   146,   105,    44,
      44,   146,   105,   146,    73,    73,    44,    44,    73,   105,
      44,    73,    44,   108
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   180,   181,   181,   181,   181,   183,   182,   184,   185,
     182,   186,   186,   186,   187,   187,   189,   190,   188,   191,
     188,   188,   193,   194,   192,   192,   195,   195,   195,   195,
     195,   195,   195,   195,   196,   196,   197,   196,   199,   198,
     200,   198,   201,   201,   201,   202,   202,   202,   202,   203,
     204,   205,   205,   205,   205,   205,   206,   205,   207,   205,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   209,   209,   209,   209,   210,   211,   211,   212,
     212,   212,   212,   213,   213,   214,   214,   214,   214,   214,
     214,   215,   216,   216,   216,   217,   217,   217,   217,   217,
     217,   217,   217,   218,   218,   219,   219,   219,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     221,   221,   222,   222,   223,   224,   225,   225,   226,   226,
     226,   227,   227,   227,   229,   228,   228,   228,   228,   228,
     231,   230,   230,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   233,   233,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   235,   235,   235,   235,   235,
     235,   235,   235,   235,   235,   235,   236,   236,   237,   237,
     237,   237,   238,   238,   238,   238,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   240,   240,   240,   240,   240,   240,
     241,   241,   242,   242,   243,   243,   243,   243,   243,   243,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   243,
     243,   243,   243,   243,   244,   244,   244,   244,   244,   246,
     245,   247,   247,   248,   248,   248,   248,   248,   249,   249,
     249,   249,   249,   249,   249,   250,   250,   250,   251,   251,
     251,   251,   251
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     0,     3,     0,     0,
       5,     6,     2,     2,     1,     2,     0,     0,     7,     0,
       3,     1,     0,     0,     6,     1,     1,     2,     2,     1,
       2,     2,     2,     2,     0,     1,     0,     4,     0,     5,
       0,     4,     0,     3,     2,     1,     1,     1,     1,     2,
       3,     1,     2,     1,     2,     2,     0,     3,     0,     2,
       2,     2,     3,     2,     4,     6,     2,     3,     7,     4,
       3,     2,     2,     4,     4,     6,     1,     1,     3,     1,
       4,     3,     4,     1,     2,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     1,     3,     2,    11,    13,     9,     0,     3,
       3,     2,     2,     3,     0,    11,     6,     5,     2,     1,
       0,     3,     1,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     4,     1,     3,     1,     2,     2,     1,     1,     1,
       1,     2,     1,     3,     2,     3,     2,     3,     3,     4,
       4,     3,     4,     4,     5,     7,     9,    17,     3,     0,
       6,     2,     1,     3,     4,     4,     4,     2,     3,     4,
       4,     4,     5,     5,     4,     1,     1,     1,     1,     2,
       2,     3,     1
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
        yyerror (scanner, recognizer, YY_("syntax error: cannot back up")); \
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
                  Kind, Value, scanner, recognizer); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, yyscan_t scanner, ptx_recognizer* recognizer)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (scanner);
  YY_USE (recognizer);
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
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, yyscan_t scanner, ptx_recognizer* recognizer)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner, recognizer);
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
                 int yyrule, yyscan_t scanner, ptx_recognizer* recognizer)
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
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner, recognizer);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner, recognizer); \
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
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, yyscan_t scanner, ptx_recognizer* recognizer)
{
  YY_USE (yyvaluep);
  YY_USE (scanner);
  YY_USE (recognizer);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (yyscan_t scanner, ptx_recognizer* recognizer)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

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
      yychar = yylex (&yylval, scanner, recognizer);
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
  case 6: /* $@1: %empty  */
#line 249 "ptx.y"
                             { recognizer->set_symtab((yyvsp[0].ptr_value)); recognizer->func_header(".skip"); }
#line 1778 "cuda-sim/ptx.tab.c"
    break;

  case 7: /* function_defn: function_decl $@1 statement_block  */
#line 249 "ptx.y"
                                                                                                               { recognizer->end_function(); }
#line 1784 "cuda-sim/ptx.tab.c"
    break;

  case 8: /* $@2: %empty  */
#line 250 "ptx.y"
                        { recognizer->set_symtab((yyvsp[0].ptr_value)); }
#line 1790 "cuda-sim/ptx.tab.c"
    break;

  case 9: /* $@3: %empty  */
#line 250 "ptx.y"
                                                                        { recognizer->func_header(".skip"); }
#line 1796 "cuda-sim/ptx.tab.c"
    break;

  case 10: /* function_defn: function_decl $@2 block_spec_list $@3 statement_block  */
#line 250 "ptx.y"
                                                                                                                              { recognizer->end_function(); }
#line 1802 "cuda-sim/ptx.tab.c"
    break;

  case 11: /* block_spec: MAXNTID_DIRECTIVE INT_OPERAND COMMA INT_OPERAND COMMA INT_OPERAND  */
#line 253 "ptx.y"
                                                                              {recognizer->func_header_info_int(".maxntid", (yyvsp[-4].int_value));
										recognizer->func_header_info_int(",", (yyvsp[-2].int_value));
										recognizer->func_header_info_int(",", (yyvsp[0].int_value));
                                                                                recognizer->maxnt_id((yyvsp[-4].int_value), (yyvsp[-2].int_value), (yyvsp[0].int_value));}
#line 1811 "cuda-sim/ptx.tab.c"
    break;

  case 12: /* block_spec: MINNCTAPERSM_DIRECTIVE INT_OPERAND  */
#line 257 "ptx.y"
                                             { recognizer->func_header_info_int(".minnctapersm", (yyvsp[0].int_value)); printf("GPGPU-Sim: Warning: .minnctapersm ignored. \n"); }
#line 1817 "cuda-sim/ptx.tab.c"
    break;

  case 13: /* block_spec: MAXNCTAPERSM_DIRECTIVE INT_OPERAND  */
#line 258 "ptx.y"
                                             { recognizer->func_header_info_int(".maxnctapersm", (yyvsp[0].int_value)); printf("GPGPU-Sim: Warning: .maxnctapersm ignored. \n"); }
#line 1823 "cuda-sim/ptx.tab.c"
    break;

  case 16: /* $@4: %empty  */
#line 265 "ptx.y"
                                               { recognizer->start_function((yyvsp[-1].int_value)); recognizer->func_header_info("(");}
#line 1829 "cuda-sim/ptx.tab.c"
    break;

  case 17: /* $@5: %empty  */
#line 265 "ptx.y"
                                                                                                                                             {recognizer->func_header_info(")");}
#line 1835 "cuda-sim/ptx.tab.c"
    break;

  case 18: /* function_decl: function_decl_header LEFT_PAREN $@4 param_entry RIGHT_PAREN $@5 function_ident_param  */
#line 265 "ptx.y"
                                                                                                                                                                                                       { (yyval.ptr_value) = recognizer->reset_symtab(); }
#line 1841 "cuda-sim/ptx.tab.c"
    break;

  case 19: /* $@6: %empty  */
#line 266 "ptx.y"
                               { recognizer->start_function((yyvsp[0].int_value)); }
#line 1847 "cuda-sim/ptx.tab.c"
    break;

  case 20: /* function_decl: function_decl_header $@6 function_ident_param  */
#line 266 "ptx.y"
                                                                                        { (yyval.ptr_value) = recognizer->reset_symtab(); }
#line 1853 "cuda-sim/ptx.tab.c"
    break;

  case 21: /* function_decl: function_decl_header  */
#line 267 "ptx.y"
                               { recognizer->start_function((yyvsp[0].int_value)); recognizer->add_function_name(""); recognizer->g_func_decl=0; (yyval.ptr_value) = recognizer->reset_symtab(); }
#line 1859 "cuda-sim/ptx.tab.c"
    break;

  case 22: /* $@7: %empty  */
#line 270 "ptx.y"
                                 { recognizer->add_function_name((yyvsp[0].string_value)); }
#line 1865 "cuda-sim/ptx.tab.c"
    break;

  case 23: /* $@8: %empty  */
#line 270 "ptx.y"
                                                                                   {recognizer->func_header_info("(");}
#line 1871 "cuda-sim/ptx.tab.c"
    break;

  case 24: /* function_ident_param: IDENTIFIER $@7 LEFT_PAREN $@8 param_list RIGHT_PAREN  */
#line 270 "ptx.y"
                                                                                                                                               { recognizer->g_func_decl=0; recognizer->func_header_info(")"); }
#line 1877 "cuda-sim/ptx.tab.c"
    break;

  case 25: /* function_ident_param: IDENTIFIER  */
#line 271 "ptx.y"
                     { recognizer->add_function_name((yyvsp[0].string_value)); recognizer->g_func_decl=0; }
#line 1883 "cuda-sim/ptx.tab.c"
    break;

  case 26: /* function_decl_header: ENTRY_DIRECTIVE  */
#line 274 "ptx.y"
                                      { (yyval.int_value) = 1; recognizer->g_func_decl=1; recognizer->func_header(".entry"); }
#line 1889 "cuda-sim/ptx.tab.c"
    break;

  case 27: /* function_decl_header: VISIBLE_DIRECTIVE ENTRY_DIRECTIVE  */
#line 275 "ptx.y"
                                            { (yyval.int_value) = 1; recognizer->g_func_decl=1; recognizer->func_header(".entry"); }
#line 1895 "cuda-sim/ptx.tab.c"
    break;

  case 28: /* function_decl_header: WEAK_DIRECTIVE ENTRY_DIRECTIVE  */
#line 276 "ptx.y"
                                         { (yyval.int_value) = 1; recognizer->g_func_decl=1; recognizer->func_header(".entry"); }
#line 1901 "cuda-sim/ptx.tab.c"
    break;

  case 29: /* function_decl_header: FUNC_DIRECTIVE  */
#line 277 "ptx.y"
                         { (yyval.int_value) = 0; recognizer->g_func_decl=1; recognizer->func_header(".func"); }
#line 1907 "cuda-sim/ptx.tab.c"
    break;

  case 30: /* function_decl_header: VISIBLE_DIRECTIVE FUNC_DIRECTIVE  */
#line 278 "ptx.y"
                                           { (yyval.int_value) = 0; recognizer->g_func_decl=1; recognizer->func_header(".func"); }
#line 1913 "cuda-sim/ptx.tab.c"
    break;

  case 31: /* function_decl_header: WEAK_DIRECTIVE FUNC_DIRECTIVE  */
#line 279 "ptx.y"
                                        { (yyval.int_value) = 0; recognizer->g_func_decl=1; recognizer->func_header(".func"); }
#line 1919 "cuda-sim/ptx.tab.c"
    break;

  case 32: /* function_decl_header: EXTERN_DIRECTIVE FUNC_DIRECTIVE  */
#line 280 "ptx.y"
                                          { (yyval.int_value) = 2; recognizer->g_func_decl=1; recognizer->func_header(".func"); }
#line 1925 "cuda-sim/ptx.tab.c"
    break;

  case 33: /* function_decl_header: WEAK_DIRECTIVE FUNC_DIRECTIVE  */
#line 281 "ptx.y"
                                        { (yyval.int_value) = 0; recognizer->g_func_decl=1; recognizer->func_header(".func"); }
#line 1931 "cuda-sim/ptx.tab.c"
    break;

  case 35: /* param_list: param_entry  */
#line 285 "ptx.y"
                      { recognizer->add_directive(); }
#line 1937 "cuda-sim/ptx.tab.c"
    break;

  case 36: /* $@9: %empty  */
#line 286 "ptx.y"
                           {recognizer->func_header_info(",");}
#line 1943 "cuda-sim/ptx.tab.c"
    break;

  case 37: /* param_list: param_list COMMA $@9 param_entry  */
#line 286 "ptx.y"
                                                                            { recognizer->add_directive(); }
#line 1949 "cuda-sim/ptx.tab.c"
    break;

  case 38: /* $@10: %empty  */
#line 288 "ptx.y"
                             { recognizer->add_space_spec(param_space_unclassified,0); }
#line 1955 "cuda-sim/ptx.tab.c"
    break;

  case 39: /* param_entry: PARAM_DIRECTIVE $@10 variable_spec ptr_spec identifier_spec  */
#line 288 "ptx.y"
                                                                                                                                { recognizer->add_function_arg(); }
#line 1961 "cuda-sim/ptx.tab.c"
    break;

  case 40: /* $@11: %empty  */
#line 289 "ptx.y"
                        { recognizer->add_space_spec(reg_space,0); }
#line 1967 "cuda-sim/ptx.tab.c"
    break;

  case 41: /* param_entry: REG_DIRECTIVE $@11 variable_spec identifier_spec  */
#line 289 "ptx.y"
                                                                                                   { recognizer->add_function_arg(); }
#line 1973 "cuda-sim/ptx.tab.c"
    break;

  case 45: /* ptr_space_spec: GLOBAL_DIRECTIVE  */
#line 295 "ptx.y"
                                 { recognizer->add_ptr_spec(global_space); }
#line 1979 "cuda-sim/ptx.tab.c"
    break;

  case 46: /* ptr_space_spec: LOCAL_DIRECTIVE  */
#line 296 "ptx.y"
                                 { recognizer->add_ptr_spec(local_space); }
#line 1985 "cuda-sim/ptx.tab.c"
    break;

  case 47: /* ptr_space_spec: SHARED_DIRECTIVE  */
#line 297 "ptx.y"
                                 { recognizer->add_ptr_spec(shared_space); }
#line 1991 "cuda-sim/ptx.tab.c"
    break;

  case 48: /* ptr_space_spec: CONST_DIRECTIVE  */
#line 298 "ptx.y"
                                            { recognizer->add_ptr_spec(global_space); }
#line 1997 "cuda-sim/ptx.tab.c"
    break;

  case 51: /* statement_list: directive_statement  */
#line 304 "ptx.y"
                                    { recognizer->add_directive(); }
#line 2003 "cuda-sim/ptx.tab.c"
    break;

  case 52: /* statement_list: statement_list prototype_block  */
#line 305 "ptx.y"
                                     {printf("Prototype statement detected. WARNING: this is not supported yet on GPGPU-SIM\n"); }
#line 2009 "cuda-sim/ptx.tab.c"
    break;

  case 53: /* statement_list: instruction_statement  */
#line 306 "ptx.y"
                                { recognizer->add_instruction(); }
#line 2015 "cuda-sim/ptx.tab.c"
    break;

  case 54: /* statement_list: statement_list directive_statement  */
#line 307 "ptx.y"
                                             { recognizer->add_directive(); }
#line 2021 "cuda-sim/ptx.tab.c"
    break;

  case 55: /* statement_list: statement_list instruction_statement  */
#line 308 "ptx.y"
                                               { recognizer->add_instruction(); }
#line 2027 "cuda-sim/ptx.tab.c"
    break;

  case 56: /* $@12: %empty  */
#line 309 "ptx.y"
                         {recognizer->start_inst_group();}
#line 2033 "cuda-sim/ptx.tab.c"
    break;

  case 57: /* statement_list: statement_list $@12 statement_block  */
#line 309 "ptx.y"
                                                                           {recognizer->end_inst_group();}
#line 2039 "cuda-sim/ptx.tab.c"
    break;

  case 58: /* $@13: %empty  */
#line 310 "ptx.y"
          {recognizer->start_inst_group();}
#line 2045 "cuda-sim/ptx.tab.c"
    break;

  case 59: /* statement_list: $@13 statement_block  */
#line 310 "ptx.y"
                                                            {recognizer->end_inst_group();}
#line 2051 "cuda-sim/ptx.tab.c"
    break;

  case 61: /* directive_statement: VERSION_DIRECTIVE DOUBLE_OPERAND  */
#line 314 "ptx.y"
                                           { recognizer->add_version_info((yyvsp[0].double_value), 0); }
#line 2057 "cuda-sim/ptx.tab.c"
    break;

  case 62: /* directive_statement: VERSION_DIRECTIVE DOUBLE_OPERAND PLUS  */
#line 315 "ptx.y"
                                                { recognizer->add_version_info((yyvsp[-1].double_value),1); }
#line 2063 "cuda-sim/ptx.tab.c"
    break;

  case 63: /* directive_statement: ADDRESS_SIZE_DIRECTIVE INT_OPERAND  */
#line 316 "ptx.y"
                                             {/*Do nothing*/}
#line 2069 "cuda-sim/ptx.tab.c"
    break;

  case 64: /* directive_statement: TARGET_DIRECTIVE IDENTIFIER COMMA IDENTIFIER  */
#line 317 "ptx.y"
                                                       { recognizer->target_header2((yyvsp[-2].string_value),(yyvsp[0].string_value)); }
#line 2075 "cuda-sim/ptx.tab.c"
    break;

  case 65: /* directive_statement: TARGET_DIRECTIVE IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER  */
#line 318 "ptx.y"
                                                                        { recognizer->target_header3((yyvsp[-4].string_value),(yyvsp[-2].string_value),(yyvsp[0].string_value)); }
#line 2081 "cuda-sim/ptx.tab.c"
    break;

  case 66: /* directive_statement: TARGET_DIRECTIVE IDENTIFIER  */
#line 319 "ptx.y"
                                      { recognizer->target_header((yyvsp[0].string_value)); }
#line 2087 "cuda-sim/ptx.tab.c"
    break;

  case 67: /* directive_statement: FILE_DIRECTIVE INT_OPERAND STRING  */
#line 320 "ptx.y"
                                            { recognizer->add_file((yyvsp[-1].int_value),(yyvsp[0].string_value)); }
#line 2093 "cuda-sim/ptx.tab.c"
    break;

  case 68: /* directive_statement: FILE_DIRECTIVE INT_OPERAND STRING COMMA INT_OPERAND COMMA INT_OPERAND  */
#line 321 "ptx.y"
                                                                                { recognizer->add_file((yyvsp[-5].int_value),(yyvsp[-4].string_value)); }
#line 2099 "cuda-sim/ptx.tab.c"
    break;

  case 70: /* directive_statement: PRAGMA_DIRECTIVE STRING SEMI_COLON  */
#line 323 "ptx.y"
                                             { recognizer->add_pragma((yyvsp[-1].string_value)); }
#line 2105 "cuda-sim/ptx.tab.c"
    break;

  case 71: /* directive_statement: function_decl SEMI_COLON  */
#line 324 "ptx.y"
                                   {/*Do nothing*/}
#line 2111 "cuda-sim/ptx.tab.c"
    break;

  case 72: /* variable_declaration: variable_spec identifier_list  */
#line 327 "ptx.y"
                                                    { recognizer->add_variables(); }
#line 2117 "cuda-sim/ptx.tab.c"
    break;

  case 73: /* variable_declaration: variable_spec identifier_spec EQUALS initializer_list  */
#line 328 "ptx.y"
                                                                { recognizer->add_variables(); }
#line 2123 "cuda-sim/ptx.tab.c"
    break;

  case 74: /* variable_declaration: variable_spec identifier_spec EQUALS literal_operand  */
#line 329 "ptx.y"
                                                               { recognizer->add_variables(); }
#line 2129 "cuda-sim/ptx.tab.c"
    break;

  case 75: /* variable_declaration: CONSTPTR_DIRECTIVE IDENTIFIER COMMA IDENTIFIER COMMA INT_OPERAND  */
#line 330 "ptx.y"
                                                                           { recognizer->add_constptr((yyvsp[-4].string_value), (yyvsp[-2].string_value), (yyvsp[0].int_value)); }
#line 2135 "cuda-sim/ptx.tab.c"
    break;

  case 76: /* variable_spec: var_spec_list  */
#line 333 "ptx.y"
                             { recognizer->set_variable_type(); }
#line 2141 "cuda-sim/ptx.tab.c"
    break;

  case 79: /* identifier_spec: IDENTIFIER  */
#line 338 "ptx.y"
                            { recognizer->add_identifier((yyvsp[0].string_value),0,NON_ARRAY_IDENTIFIER); recognizer->func_header_info((yyvsp[0].string_value));}
#line 2147 "cuda-sim/ptx.tab.c"
    break;

  case 80: /* identifier_spec: IDENTIFIER LEFT_ANGLE_BRACKET INT_OPERAND RIGHT_ANGLE_BRACKET  */
#line 339 "ptx.y"
                                                                        { recognizer->func_header_info((yyvsp[-3].string_value)); recognizer->func_header_info_int("<", (yyvsp[-1].int_value)); recognizer->func_header_info(">");
		int i,lbase,l;
		char *id = NULL;
		lbase = strlen((yyvsp[-3].string_value));
		for( i=0; i < (yyvsp[-1].int_value); i++ ) { 
			l = lbase + (int)log10(i+1)+10;
			id = (char*) malloc(l);
			snprintf(id,l,"%s%u",(yyvsp[-3].string_value),i);
			recognizer->add_identifier(id,0,NON_ARRAY_IDENTIFIER);
		}
		free((yyvsp[-3].string_value));
	}
#line 2164 "cuda-sim/ptx.tab.c"
    break;

  case 81: /* identifier_spec: IDENTIFIER LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET  */
#line 351 "ptx.y"
                                                              { recognizer->add_identifier((yyvsp[-2].string_value),0,ARRAY_IDENTIFIER_NO_DIM); recognizer->func_header_info((yyvsp[-2].string_value)); recognizer->func_header_info("["); recognizer->func_header_info("]");}
#line 2170 "cuda-sim/ptx.tab.c"
    break;

  case 82: /* identifier_spec: IDENTIFIER LEFT_SQUARE_BRACKET INT_OPERAND RIGHT_SQUARE_BRACKET  */
#line 352 "ptx.y"
                                                                          { recognizer->add_identifier((yyvsp[-3].string_value),(yyvsp[-1].int_value),ARRAY_IDENTIFIER); recognizer->func_header_info((yyvsp[-3].string_value)); recognizer->func_header_info_int("[",(yyvsp[-1].int_value)); recognizer->func_header_info("]");}
#line 2176 "cuda-sim/ptx.tab.c"
    break;

  case 89: /* var_spec: EXTERN_DIRECTIVE  */
#line 362 "ptx.y"
                           { recognizer->add_extern_spec(); }
#line 2182 "cuda-sim/ptx.tab.c"
    break;

  case 91: /* align_spec: ALIGN_DIRECTIVE INT_OPERAND  */
#line 366 "ptx.y"
                                        { recognizer->add_alignment_spec((yyvsp[0].int_value)); }
#line 2188 "cuda-sim/ptx.tab.c"
    break;

  case 92: /* space_spec: REG_DIRECTIVE  */
#line 368 "ptx.y"
                          {  recognizer->add_space_spec(reg_space,0); }
#line 2194 "cuda-sim/ptx.tab.c"
    break;

  case 93: /* space_spec: SREG_DIRECTIVE  */
#line 369 "ptx.y"
                          {  recognizer->add_space_spec(reg_space,0); }
#line 2200 "cuda-sim/ptx.tab.c"
    break;

  case 95: /* addressable_spec: CONST_DIRECTIVE  */
#line 373 "ptx.y"
                                  {  recognizer->add_space_spec(const_space,(yyvsp[0].int_value)); }
#line 2206 "cuda-sim/ptx.tab.c"
    break;

  case 96: /* addressable_spec: GLOBAL_DIRECTIVE  */
#line 374 "ptx.y"
                                  {  recognizer->add_space_spec(global_space,0); }
#line 2212 "cuda-sim/ptx.tab.c"
    break;

  case 97: /* addressable_spec: LOCAL_DIRECTIVE  */
#line 375 "ptx.y"
                                  {  recognizer->add_space_spec(local_space,0); }
#line 2218 "cuda-sim/ptx.tab.c"
    break;

  case 98: /* addressable_spec: PARAM_DIRECTIVE  */
#line 376 "ptx.y"
                                  {  recognizer->add_space_spec(param_space_unclassified,0); }
#line 2224 "cuda-sim/ptx.tab.c"
    break;

  case 99: /* addressable_spec: SHARED_DIRECTIVE  */
#line 377 "ptx.y"
                                  {  recognizer->add_space_spec(shared_space,0); }
#line 2230 "cuda-sim/ptx.tab.c"
    break;

  case 100: /* addressable_spec: SSTARR_DIRECTIVE  */
#line 378 "ptx.y"
                              {  recognizer->add_space_spec(sstarr_space,0); }
#line 2236 "cuda-sim/ptx.tab.c"
    break;

  case 101: /* addressable_spec: SURF_DIRECTIVE  */
#line 379 "ptx.y"
                                  {  recognizer->add_space_spec(surf_space,0); }
#line 2242 "cuda-sim/ptx.tab.c"
    break;

  case 102: /* addressable_spec: TEX_DIRECTIVE  */
#line 380 "ptx.y"
                                  {  recognizer->add_space_spec(tex_space,0); }
#line 2248 "cuda-sim/ptx.tab.c"
    break;

  case 105: /* vector_spec: V2_TYPE  */
#line 387 "ptx.y"
                      {  recognizer->add_option(V2_TYPE); recognizer->func_header_info(".v2");}
#line 2254 "cuda-sim/ptx.tab.c"
    break;

  case 106: /* vector_spec: V3_TYPE  */
#line 388 "ptx.y"
                      {  recognizer->add_option(V3_TYPE); recognizer->func_header_info(".v3");}
#line 2260 "cuda-sim/ptx.tab.c"
    break;

  case 107: /* vector_spec: V4_TYPE  */
#line 389 "ptx.y"
                      {  recognizer->add_option(V4_TYPE); recognizer->func_header_info(".v4");}
#line 2266 "cuda-sim/ptx.tab.c"
    break;

  case 108: /* scalar_type: S8_TYPE  */
#line 392 "ptx.y"
                     { recognizer->add_scalar_type_spec( S8_TYPE ); }
#line 2272 "cuda-sim/ptx.tab.c"
    break;

  case 109: /* scalar_type: S16_TYPE  */
#line 393 "ptx.y"
                     { recognizer->add_scalar_type_spec( S16_TYPE ); }
#line 2278 "cuda-sim/ptx.tab.c"
    break;

  case 110: /* scalar_type: S32_TYPE  */
#line 394 "ptx.y"
                     { recognizer->add_scalar_type_spec( S32_TYPE ); }
#line 2284 "cuda-sim/ptx.tab.c"
    break;

  case 111: /* scalar_type: S64_TYPE  */
#line 395 "ptx.y"
                     { recognizer->add_scalar_type_spec( S64_TYPE ); }
#line 2290 "cuda-sim/ptx.tab.c"
    break;

  case 112: /* scalar_type: U8_TYPE  */
#line 396 "ptx.y"
                     { recognizer->add_scalar_type_spec( U8_TYPE ); }
#line 2296 "cuda-sim/ptx.tab.c"
    break;

  case 113: /* scalar_type: U16_TYPE  */
#line 397 "ptx.y"
                     { recognizer->add_scalar_type_spec( U16_TYPE ); }
#line 2302 "cuda-sim/ptx.tab.c"
    break;

  case 114: /* scalar_type: U32_TYPE  */
#line 398 "ptx.y"
                     { recognizer->add_scalar_type_spec( U32_TYPE ); }
#line 2308 "cuda-sim/ptx.tab.c"
    break;

  case 115: /* scalar_type: U64_TYPE  */
#line 399 "ptx.y"
                     { recognizer->add_scalar_type_spec( U64_TYPE ); }
#line 2314 "cuda-sim/ptx.tab.c"
    break;

  case 116: /* scalar_type: F16_TYPE  */
#line 400 "ptx.y"
                     { recognizer->add_scalar_type_spec( F16_TYPE ); }
#line 2320 "cuda-sim/ptx.tab.c"
    break;

  case 117: /* scalar_type: F32_TYPE  */
#line 401 "ptx.y"
                     { recognizer->add_scalar_type_spec( F32_TYPE ); }
#line 2326 "cuda-sim/ptx.tab.c"
    break;

  case 118: /* scalar_type: F64_TYPE  */
#line 402 "ptx.y"
                     { recognizer->add_scalar_type_spec( F64_TYPE ); }
#line 2332 "cuda-sim/ptx.tab.c"
    break;

  case 119: /* scalar_type: FF64_TYPE  */
#line 403 "ptx.y"
                      { recognizer->add_scalar_type_spec( FF64_TYPE ); }
#line 2338 "cuda-sim/ptx.tab.c"
    break;

  case 120: /* scalar_type: B8_TYPE  */
#line 404 "ptx.y"
                     { recognizer->add_scalar_type_spec( B8_TYPE );  }
#line 2344 "cuda-sim/ptx.tab.c"
    break;

  case 121: /* scalar_type: B16_TYPE  */
#line 405 "ptx.y"
                     { recognizer->add_scalar_type_spec( B16_TYPE ); }
#line 2350 "cuda-sim/ptx.tab.c"
    break;

  case 122: /* scalar_type: B32_TYPE  */
#line 406 "ptx.y"
                     { recognizer->add_scalar_type_spec( B32_TYPE ); }
#line 2356 "cuda-sim/ptx.tab.c"
    break;

  case 123: /* scalar_type: B64_TYPE  */
#line 407 "ptx.y"
                     { recognizer->add_scalar_type_spec( B64_TYPE ); }
#line 2362 "cuda-sim/ptx.tab.c"
    break;

  case 124: /* scalar_type: BB64_TYPE  */
#line 408 "ptx.y"
                      { recognizer->add_scalar_type_spec( BB64_TYPE ); }
#line 2368 "cuda-sim/ptx.tab.c"
    break;

  case 125: /* scalar_type: BB128_TYPE  */
#line 409 "ptx.y"
                       { recognizer->add_scalar_type_spec( BB128_TYPE ); }
#line 2374 "cuda-sim/ptx.tab.c"
    break;

  case 126: /* scalar_type: PRED_TYPE  */
#line 410 "ptx.y"
                     { recognizer->add_scalar_type_spec( PRED_TYPE ); }
#line 2380 "cuda-sim/ptx.tab.c"
    break;

  case 127: /* scalar_type: TEXREF_TYPE  */
#line 411 "ptx.y"
                       { recognizer->add_scalar_type_spec( TEXREF_TYPE ); }
#line 2386 "cuda-sim/ptx.tab.c"
    break;

  case 128: /* scalar_type: SAMPLERREF_TYPE  */
#line 412 "ptx.y"
                           { recognizer->add_scalar_type_spec( SAMPLERREF_TYPE ); }
#line 2392 "cuda-sim/ptx.tab.c"
    break;

  case 129: /* scalar_type: SURFREF_TYPE  */
#line 413 "ptx.y"
                        { recognizer->add_scalar_type_spec( SURFREF_TYPE ); }
#line 2398 "cuda-sim/ptx.tab.c"
    break;

  case 130: /* initializer_list: LEFT_BRACE literal_list RIGHT_BRACE  */
#line 416 "ptx.y"
                                                      { recognizer->add_array_initializer(); }
#line 2404 "cuda-sim/ptx.tab.c"
    break;

  case 131: /* initializer_list: LEFT_BRACE initializer_list RIGHT_BRACE  */
#line 417 "ptx.y"
                                                  { syntax_not_implemented(scanner, recognizer); }
#line 2410 "cuda-sim/ptx.tab.c"
    break;

  case 142: /* instruction_statement: IDENTIFIER COLON  */
#line 440 "ptx.y"
                           { recognizer->add_label((yyvsp[-1].string_value)); }
#line 2416 "cuda-sim/ptx.tab.c"
    break;

  case 144: /* $@14: %empty  */
#line 443 "ptx.y"
                                                        { recognizer->set_return(); }
#line 2422 "cuda-sim/ptx.tab.c"
    break;

  case 150: /* $@15: %empty  */
#line 450 "ptx.y"
                    { recognizer->add_opcode((yyvsp[0].int_value)); }
#line 2428 "cuda-sim/ptx.tab.c"
    break;

  case 152: /* opcode_spec: OPCODE  */
#line 451 "ptx.y"
                 { recognizer->add_opcode((yyvsp[0].int_value)); }
#line 2434 "cuda-sim/ptx.tab.c"
    break;

  case 153: /* pred_spec: PRED IDENTIFIER  */
#line 453 "ptx.y"
                            { recognizer->add_pred((yyvsp[0].string_value),0, -1); }
#line 2440 "cuda-sim/ptx.tab.c"
    break;

  case 154: /* pred_spec: PRED EXCLAMATION IDENTIFIER  */
#line 454 "ptx.y"
                                      { recognizer->add_pred((yyvsp[0].string_value),1, -1); }
#line 2446 "cuda-sim/ptx.tab.c"
    break;

  case 155: /* pred_spec: PRED IDENTIFIER LT_OPTION  */
#line 455 "ptx.y"
                                     { recognizer->add_pred((yyvsp[-1].string_value),0,1); }
#line 2452 "cuda-sim/ptx.tab.c"
    break;

  case 156: /* pred_spec: PRED IDENTIFIER EQ_OPTION  */
#line 456 "ptx.y"
                                     { recognizer->add_pred((yyvsp[-1].string_value),0,2); }
#line 2458 "cuda-sim/ptx.tab.c"
    break;

  case 157: /* pred_spec: PRED IDENTIFIER LE_OPTION  */
#line 457 "ptx.y"
                                     { recognizer->add_pred((yyvsp[-1].string_value),0,3); }
#line 2464 "cuda-sim/ptx.tab.c"
    break;

  case 158: /* pred_spec: PRED IDENTIFIER NE_OPTION  */
#line 458 "ptx.y"
                                     { recognizer->add_pred((yyvsp[-1].string_value),0,5); }
#line 2470 "cuda-sim/ptx.tab.c"
    break;

  case 159: /* pred_spec: PRED IDENTIFIER GE_OPTION  */
#line 459 "ptx.y"
                                     { recognizer->add_pred((yyvsp[-1].string_value),0,6); }
#line 2476 "cuda-sim/ptx.tab.c"
    break;

  case 160: /* pred_spec: PRED IDENTIFIER EQU_OPTION  */
#line 460 "ptx.y"
                                      { recognizer->add_pred((yyvsp[-1].string_value),0,10); }
#line 2482 "cuda-sim/ptx.tab.c"
    break;

  case 161: /* pred_spec: PRED IDENTIFIER GTU_OPTION  */
#line 461 "ptx.y"
                                      { recognizer->add_pred((yyvsp[-1].string_value),0,12); }
#line 2488 "cuda-sim/ptx.tab.c"
    break;

  case 162: /* pred_spec: PRED IDENTIFIER NEU_OPTION  */
#line 462 "ptx.y"
                                      { recognizer->add_pred((yyvsp[-1].string_value),0,13); }
#line 2494 "cuda-sim/ptx.tab.c"
    break;

  case 163: /* pred_spec: PRED IDENTIFIER CF_OPTION  */
#line 463 "ptx.y"
                                     { recognizer->add_pred((yyvsp[-1].string_value),0,17); }
#line 2500 "cuda-sim/ptx.tab.c"
    break;

  case 164: /* pred_spec: PRED IDENTIFIER SF_OPTION  */
#line 464 "ptx.y"
                                     { recognizer->add_pred((yyvsp[-1].string_value),0,19); }
#line 2506 "cuda-sim/ptx.tab.c"
    break;

  case 165: /* pred_spec: PRED IDENTIFIER NSF_OPTION  */
#line 465 "ptx.y"
                                      { recognizer->add_pred((yyvsp[-1].string_value),0,28); }
#line 2512 "cuda-sim/ptx.tab.c"
    break;

  case 174: /* option: SYNC_OPTION  */
#line 477 "ptx.y"
                      { recognizer->add_option(SYNC_OPTION); }
#line 2518 "cuda-sim/ptx.tab.c"
    break;

  case 175: /* option: ARRIVE_OPTION  */
#line 478 "ptx.y"
                        { recognizer->add_option(ARRIVE_OPTION); }
#line 2524 "cuda-sim/ptx.tab.c"
    break;

  case 176: /* option: RED_OPTION  */
#line 479 "ptx.y"
                     { recognizer->add_option(RED_OPTION); }
#line 2530 "cuda-sim/ptx.tab.c"
    break;

  case 177: /* option: UNI_OPTION  */
#line 480 "ptx.y"
                     { recognizer->add_option(UNI_OPTION); }
#line 2536 "cuda-sim/ptx.tab.c"
    break;

  case 178: /* option: WIDE_OPTION  */
#line 481 "ptx.y"
                      { recognizer->add_option(WIDE_OPTION); }
#line 2542 "cuda-sim/ptx.tab.c"
    break;

  case 179: /* option: ANY_OPTION  */
#line 482 "ptx.y"
                     { recognizer->add_option(ANY_OPTION); }
#line 2548 "cuda-sim/ptx.tab.c"
    break;

  case 180: /* option: ALL_OPTION  */
#line 483 "ptx.y"
                     { recognizer->add_option(ALL_OPTION); }
#line 2554 "cuda-sim/ptx.tab.c"
    break;

  case 181: /* option: BALLOT_OPTION  */
#line 484 "ptx.y"
                        { recognizer->add_option(BALLOT_OPTION); }
#line 2560 "cuda-sim/ptx.tab.c"
    break;

  case 182: /* option: GLOBAL_OPTION  */
#line 485 "ptx.y"
                        { recognizer->add_option(GLOBAL_OPTION); }
#line 2566 "cuda-sim/ptx.tab.c"
    break;

  case 183: /* option: CTA_OPTION  */
#line 486 "ptx.y"
                     { recognizer->add_option(CTA_OPTION); }
#line 2572 "cuda-sim/ptx.tab.c"
    break;

  case 184: /* option: SYS_OPTION  */
#line 487 "ptx.y"
                     { recognizer->add_option(SYS_OPTION); }
#line 2578 "cuda-sim/ptx.tab.c"
    break;

  case 185: /* option: GEOM_MODIFIER_1D  */
#line 488 "ptx.y"
                           { recognizer->add_option(GEOM_MODIFIER_1D); }
#line 2584 "cuda-sim/ptx.tab.c"
    break;

  case 186: /* option: GEOM_MODIFIER_2D  */
#line 489 "ptx.y"
                           { recognizer->add_option(GEOM_MODIFIER_2D); }
#line 2590 "cuda-sim/ptx.tab.c"
    break;

  case 187: /* option: GEOM_MODIFIER_3D  */
#line 490 "ptx.y"
                           { recognizer->add_option(GEOM_MODIFIER_3D); }
#line 2596 "cuda-sim/ptx.tab.c"
    break;

  case 188: /* option: SAT_OPTION  */
#line 491 "ptx.y"
                     { recognizer->add_option(SAT_OPTION); }
#line 2602 "cuda-sim/ptx.tab.c"
    break;

  case 189: /* option: FTZ_OPTION  */
#line 492 "ptx.y"
                     { recognizer->add_option(FTZ_OPTION); }
#line 2608 "cuda-sim/ptx.tab.c"
    break;

  case 190: /* option: NEG_OPTION  */
#line 493 "ptx.y"
                     { recognizer->add_option(NEG_OPTION); }
#line 2614 "cuda-sim/ptx.tab.c"
    break;

  case 191: /* option: APPROX_OPTION  */
#line 494 "ptx.y"
                        { recognizer->add_option(APPROX_OPTION); }
#line 2620 "cuda-sim/ptx.tab.c"
    break;

  case 192: /* option: FULL_OPTION  */
#line 495 "ptx.y"
                      { recognizer->add_option(FULL_OPTION); }
#line 2626 "cuda-sim/ptx.tab.c"
    break;

  case 193: /* option: EXIT_OPTION  */
#line 496 "ptx.y"
                      { recognizer->add_option(EXIT_OPTION); }
#line 2632 "cuda-sim/ptx.tab.c"
    break;

  case 194: /* option: ABS_OPTION  */
#line 497 "ptx.y"
                     { recognizer->add_option(ABS_OPTION); }
#line 2638 "cuda-sim/ptx.tab.c"
    break;

  case 196: /* option: TO_OPTION  */
#line 499 "ptx.y"
                    { recognizer->add_option(TO_OPTION); }
#line 2644 "cuda-sim/ptx.tab.c"
    break;

  case 197: /* option: HALF_OPTION  */
#line 500 "ptx.y"
                      { recognizer->add_option(HALF_OPTION); }
#line 2650 "cuda-sim/ptx.tab.c"
    break;

  case 198: /* option: EXTP_OPTION  */
#line 501 "ptx.y"
                      { recognizer->add_option(EXTP_OPTION); }
#line 2656 "cuda-sim/ptx.tab.c"
    break;

  case 199: /* option: CA_OPTION  */
#line 502 "ptx.y"
                    { recognizer->add_option(CA_OPTION); }
#line 2662 "cuda-sim/ptx.tab.c"
    break;

  case 200: /* option: CG_OPTION  */
#line 503 "ptx.y"
                    { recognizer->add_option(CG_OPTION); }
#line 2668 "cuda-sim/ptx.tab.c"
    break;

  case 201: /* option: CS_OPTION  */
#line 504 "ptx.y"
                    { recognizer->add_option(CS_OPTION); }
#line 2674 "cuda-sim/ptx.tab.c"
    break;

  case 202: /* option: LU_OPTION  */
#line 505 "ptx.y"
                    { recognizer->add_option(LU_OPTION); }
#line 2680 "cuda-sim/ptx.tab.c"
    break;

  case 203: /* option: CV_OPTION  */
#line 506 "ptx.y"
                    { recognizer->add_option(CV_OPTION); }
#line 2686 "cuda-sim/ptx.tab.c"
    break;

  case 204: /* option: WB_OPTION  */
#line 507 "ptx.y"
                    { recognizer->add_option(WB_OPTION); }
#line 2692 "cuda-sim/ptx.tab.c"
    break;

  case 205: /* option: WT_OPTION  */
#line 508 "ptx.y"
                    { recognizer->add_option(WT_OPTION); }
#line 2698 "cuda-sim/ptx.tab.c"
    break;

  case 206: /* option: NC_OPTION  */
#line 509 "ptx.y"
                    { recognizer->add_option(NC_OPTION); }
#line 2704 "cuda-sim/ptx.tab.c"
    break;

  case 207: /* option: UP_OPTION  */
#line 510 "ptx.y"
                    { recognizer->add_option(UP_OPTION); }
#line 2710 "cuda-sim/ptx.tab.c"
    break;

  case 208: /* option: DOWN_OPTION  */
#line 511 "ptx.y"
                      { recognizer->add_option(DOWN_OPTION); }
#line 2716 "cuda-sim/ptx.tab.c"
    break;

  case 209: /* option: BFLY_OPTION  */
#line 512 "ptx.y"
                      { recognizer->add_option(BFLY_OPTION); }
#line 2722 "cuda-sim/ptx.tab.c"
    break;

  case 210: /* option: IDX_OPTION  */
#line 513 "ptx.y"
                     { recognizer->add_option(IDX_OPTION); }
#line 2728 "cuda-sim/ptx.tab.c"
    break;

  case 211: /* option: WRAP_OPTION  */
#line 514 "ptx.y"
                      { recognizer->add_option(WRAP_OPTION); }
#line 2734 "cuda-sim/ptx.tab.c"
    break;

  case 212: /* option: CLAMP_OPTION  */
#line 515 "ptx.y"
                       { recognizer->add_option(CLAMP_OPTION); }
#line 2740 "cuda-sim/ptx.tab.c"
    break;

  case 213: /* option: LEFT_OPTION  */
#line 516 "ptx.y"
                      { recognizer->add_option(LEFT_OPTION); }
#line 2746 "cuda-sim/ptx.tab.c"
    break;

  case 214: /* option: RIGHT_OPTION  */
#line 517 "ptx.y"
                       { recognizer->add_option(RIGHT_OPTION); }
#line 2752 "cuda-sim/ptx.tab.c"
    break;

  case 215: /* atomic_operation_spec: ATOMIC_AND  */
#line 520 "ptx.y"
                                  { recognizer->add_option(ATOMIC_AND); }
#line 2758 "cuda-sim/ptx.tab.c"
    break;

  case 216: /* atomic_operation_spec: ATOMIC_POPC  */
#line 521 "ptx.y"
                      { recognizer->add_option(ATOMIC_POPC); }
#line 2764 "cuda-sim/ptx.tab.c"
    break;

  case 217: /* atomic_operation_spec: ATOMIC_OR  */
#line 522 "ptx.y"
                    { recognizer->add_option(ATOMIC_OR); }
#line 2770 "cuda-sim/ptx.tab.c"
    break;

  case 218: /* atomic_operation_spec: ATOMIC_XOR  */
#line 523 "ptx.y"
                     { recognizer->add_option(ATOMIC_XOR); }
#line 2776 "cuda-sim/ptx.tab.c"
    break;

  case 219: /* atomic_operation_spec: ATOMIC_CAS  */
#line 524 "ptx.y"
                     { recognizer->add_option(ATOMIC_CAS); }
#line 2782 "cuda-sim/ptx.tab.c"
    break;

  case 220: /* atomic_operation_spec: ATOMIC_EXCH  */
#line 525 "ptx.y"
                      { recognizer->add_option(ATOMIC_EXCH); }
#line 2788 "cuda-sim/ptx.tab.c"
    break;

  case 221: /* atomic_operation_spec: ATOMIC_ADD  */
#line 526 "ptx.y"
                     { recognizer->add_option(ATOMIC_ADD); }
#line 2794 "cuda-sim/ptx.tab.c"
    break;

  case 222: /* atomic_operation_spec: ATOMIC_INC  */
#line 527 "ptx.y"
                     { recognizer->add_option(ATOMIC_INC); }
#line 2800 "cuda-sim/ptx.tab.c"
    break;

  case 223: /* atomic_operation_spec: ATOMIC_DEC  */
#line 528 "ptx.y"
                     { recognizer->add_option(ATOMIC_DEC); }
#line 2806 "cuda-sim/ptx.tab.c"
    break;

  case 224: /* atomic_operation_spec: ATOMIC_MIN  */
#line 529 "ptx.y"
                     { recognizer->add_option(ATOMIC_MIN); }
#line 2812 "cuda-sim/ptx.tab.c"
    break;

  case 225: /* atomic_operation_spec: ATOMIC_MAX  */
#line 530 "ptx.y"
                     { recognizer->add_option(ATOMIC_MAX); }
#line 2818 "cuda-sim/ptx.tab.c"
    break;

  case 228: /* floating_point_rounding_mode: RN_OPTION  */
#line 537 "ptx.y"
                                        { recognizer->add_option(RN_OPTION); }
#line 2824 "cuda-sim/ptx.tab.c"
    break;

  case 229: /* floating_point_rounding_mode: RZ_OPTION  */
#line 538 "ptx.y"
                    { recognizer->add_option(RZ_OPTION); }
#line 2830 "cuda-sim/ptx.tab.c"
    break;

  case 230: /* floating_point_rounding_mode: RM_OPTION  */
#line 539 "ptx.y"
                    { recognizer->add_option(RM_OPTION); }
#line 2836 "cuda-sim/ptx.tab.c"
    break;

  case 231: /* floating_point_rounding_mode: RP_OPTION  */
#line 540 "ptx.y"
                    { recognizer->add_option(RP_OPTION); }
#line 2842 "cuda-sim/ptx.tab.c"
    break;

  case 232: /* integer_rounding_mode: RNI_OPTION  */
#line 543 "ptx.y"
                                  { recognizer->add_option(RNI_OPTION); }
#line 2848 "cuda-sim/ptx.tab.c"
    break;

  case 233: /* integer_rounding_mode: RZI_OPTION  */
#line 544 "ptx.y"
                     { recognizer->add_option(RZI_OPTION); }
#line 2854 "cuda-sim/ptx.tab.c"
    break;

  case 234: /* integer_rounding_mode: RMI_OPTION  */
#line 545 "ptx.y"
                     { recognizer->add_option(RMI_OPTION); }
#line 2860 "cuda-sim/ptx.tab.c"
    break;

  case 235: /* integer_rounding_mode: RPI_OPTION  */
#line 546 "ptx.y"
                     { recognizer->add_option(RPI_OPTION); }
#line 2866 "cuda-sim/ptx.tab.c"
    break;

  case 236: /* compare_spec: EQ_OPTION  */
#line 549 "ptx.y"
                       { recognizer->add_option(EQ_OPTION); }
#line 2872 "cuda-sim/ptx.tab.c"
    break;

  case 237: /* compare_spec: NE_OPTION  */
#line 550 "ptx.y"
                    { recognizer->add_option(NE_OPTION); }
#line 2878 "cuda-sim/ptx.tab.c"
    break;

  case 238: /* compare_spec: LT_OPTION  */
#line 551 "ptx.y"
                    { recognizer->add_option(LT_OPTION); }
#line 2884 "cuda-sim/ptx.tab.c"
    break;

  case 239: /* compare_spec: LE_OPTION  */
#line 552 "ptx.y"
                    { recognizer->add_option(LE_OPTION); }
#line 2890 "cuda-sim/ptx.tab.c"
    break;

  case 240: /* compare_spec: GT_OPTION  */
#line 553 "ptx.y"
                    { recognizer->add_option(GT_OPTION); }
#line 2896 "cuda-sim/ptx.tab.c"
    break;

  case 241: /* compare_spec: GE_OPTION  */
#line 554 "ptx.y"
                    { recognizer->add_option(GE_OPTION); }
#line 2902 "cuda-sim/ptx.tab.c"
    break;

  case 242: /* compare_spec: LO_OPTION  */
#line 555 "ptx.y"
                    { recognizer->add_option(LO_OPTION); }
#line 2908 "cuda-sim/ptx.tab.c"
    break;

  case 243: /* compare_spec: LS_OPTION  */
#line 556 "ptx.y"
                    { recognizer->add_option(LS_OPTION); }
#line 2914 "cuda-sim/ptx.tab.c"
    break;

  case 244: /* compare_spec: HI_OPTION  */
#line 557 "ptx.y"
                    { recognizer->add_option(HI_OPTION); }
#line 2920 "cuda-sim/ptx.tab.c"
    break;

  case 245: /* compare_spec: HS_OPTION  */
#line 558 "ptx.y"
                     { recognizer->add_option(HS_OPTION); }
#line 2926 "cuda-sim/ptx.tab.c"
    break;

  case 246: /* compare_spec: EQU_OPTION  */
#line 559 "ptx.y"
                     { recognizer->add_option(EQU_OPTION); }
#line 2932 "cuda-sim/ptx.tab.c"
    break;

  case 247: /* compare_spec: NEU_OPTION  */
#line 560 "ptx.y"
                     { recognizer->add_option(NEU_OPTION); }
#line 2938 "cuda-sim/ptx.tab.c"
    break;

  case 248: /* compare_spec: LTU_OPTION  */
#line 561 "ptx.y"
                     { recognizer->add_option(LTU_OPTION); }
#line 2944 "cuda-sim/ptx.tab.c"
    break;

  case 249: /* compare_spec: LEU_OPTION  */
#line 562 "ptx.y"
                     { recognizer->add_option(LEU_OPTION); }
#line 2950 "cuda-sim/ptx.tab.c"
    break;

  case 250: /* compare_spec: GTU_OPTION  */
#line 563 "ptx.y"
                     { recognizer->add_option(GTU_OPTION); }
#line 2956 "cuda-sim/ptx.tab.c"
    break;

  case 251: /* compare_spec: GEU_OPTION  */
#line 564 "ptx.y"
                     { recognizer->add_option(GEU_OPTION); }
#line 2962 "cuda-sim/ptx.tab.c"
    break;

  case 252: /* compare_spec: NUM_OPTION  */
#line 565 "ptx.y"
                     { recognizer->add_option(NUM_OPTION); }
#line 2968 "cuda-sim/ptx.tab.c"
    break;

  case 253: /* compare_spec: NAN_OPTION  */
#line 566 "ptx.y"
                     { recognizer->add_option(NAN_OPTION); }
#line 2974 "cuda-sim/ptx.tab.c"
    break;

  case 254: /* prmt_spec: PRMT_F4E_MODE  */
#line 569 "ptx.y"
                         { recognizer->add_option( PRMT_F4E_MODE); }
#line 2980 "cuda-sim/ptx.tab.c"
    break;

  case 255: /* prmt_spec: PRMT_B4E_MODE  */
#line 570 "ptx.y"
                         { recognizer->add_option( PRMT_B4E_MODE); }
#line 2986 "cuda-sim/ptx.tab.c"
    break;

  case 256: /* prmt_spec: PRMT_RC8_MODE  */
#line 571 "ptx.y"
                         { recognizer->add_option( PRMT_RC8_MODE); }
#line 2992 "cuda-sim/ptx.tab.c"
    break;

  case 257: /* prmt_spec: PRMT_RC16_MODE  */
#line 572 "ptx.y"
                         { recognizer->add_option( PRMT_RC16_MODE);}
#line 2998 "cuda-sim/ptx.tab.c"
    break;

  case 258: /* prmt_spec: PRMT_ECL_MODE  */
#line 573 "ptx.y"
                         { recognizer->add_option( PRMT_ECL_MODE); }
#line 3004 "cuda-sim/ptx.tab.c"
    break;

  case 259: /* prmt_spec: PRMT_ECR_MODE  */
#line 574 "ptx.y"
                         { recognizer->add_option( PRMT_ECR_MODE); }
#line 3010 "cuda-sim/ptx.tab.c"
    break;

  case 260: /* wmma_spec: WMMA_DIRECTIVE LAYOUT CONFIGURATION  */
#line 577 "ptx.y"
                                              {recognizer->add_space_spec(global_space,0);recognizer->add_ptr_spec(global_space); recognizer->add_wmma_option((yyvsp[-2].int_value));recognizer->add_wmma_option((yyvsp[-1].int_value));recognizer->add_wmma_option((yyvsp[0].int_value));}
#line 3016 "cuda-sim/ptx.tab.c"
    break;

  case 261: /* wmma_spec: WMMA_DIRECTIVE LAYOUT LAYOUT CONFIGURATION  */
#line 578 "ptx.y"
                                                    {recognizer->add_wmma_option((yyvsp[-3].int_value));recognizer->add_wmma_option((yyvsp[-2].int_value));recognizer->add_wmma_option((yyvsp[-1].int_value));recognizer->add_wmma_option((yyvsp[0].int_value));}
#line 3022 "cuda-sim/ptx.tab.c"
    break;

  case 264: /* operand: IDENTIFIER  */
#line 590 "ptx.y"
                     { recognizer->add_scalar_operand( (yyvsp[0].string_value) ); }
#line 3028 "cuda-sim/ptx.tab.c"
    break;

  case 265: /* operand: EXCLAMATION IDENTIFIER  */
#line 591 "ptx.y"
                                 { recognizer->add_neg_pred_operand( (yyvsp[0].string_value) ); }
#line 3034 "cuda-sim/ptx.tab.c"
    break;

  case 266: /* operand: MINUS IDENTIFIER  */
#line 592 "ptx.y"
                            { recognizer->add_scalar_operand( (yyvsp[0].string_value) ); recognizer->change_operand_neg(); }
#line 3040 "cuda-sim/ptx.tab.c"
    break;

  case 271: /* operand: MINUS vector_operand  */
#line 597 "ptx.y"
                               { recognizer->change_operand_neg(); }
#line 3046 "cuda-sim/ptx.tab.c"
    break;

  case 273: /* operand: IDENTIFIER PLUS INT_OPERAND  */
#line 599 "ptx.y"
                                      { recognizer->add_address_operand((yyvsp[-2].string_value),(yyvsp[0].int_value)); }
#line 3052 "cuda-sim/ptx.tab.c"
    break;

  case 274: /* operand: IDENTIFIER LO_OPTION  */
#line 600 "ptx.y"
                               { recognizer->add_scalar_operand( (yyvsp[-1].string_value) ); recognizer->change_operand_lohi(1);}
#line 3058 "cuda-sim/ptx.tab.c"
    break;

  case 275: /* operand: MINUS IDENTIFIER LO_OPTION  */
#line 601 "ptx.y"
                                     { recognizer->add_scalar_operand( (yyvsp[-1].string_value) ); recognizer->change_operand_lohi(1); recognizer->change_operand_neg();}
#line 3064 "cuda-sim/ptx.tab.c"
    break;

  case 276: /* operand: IDENTIFIER HI_OPTION  */
#line 602 "ptx.y"
                               { recognizer->add_scalar_operand( (yyvsp[-1].string_value) ); recognizer->change_operand_lohi(2);}
#line 3070 "cuda-sim/ptx.tab.c"
    break;

  case 277: /* operand: MINUS IDENTIFIER HI_OPTION  */
#line 603 "ptx.y"
                                     { recognizer->add_scalar_operand( (yyvsp[-1].string_value) ); recognizer->change_operand_lohi(2); recognizer->change_operand_neg();}
#line 3076 "cuda-sim/ptx.tab.c"
    break;

  case 278: /* operand: IDENTIFIER PIPE IDENTIFIER  */
#line 604 "ptx.y"
                                     { recognizer->add_2vector_operand((yyvsp[-2].string_value),(yyvsp[0].string_value)); recognizer->change_double_operand_type(-1);}
#line 3082 "cuda-sim/ptx.tab.c"
    break;

  case 279: /* operand: IDENTIFIER PIPE IDENTIFIER LO_OPTION  */
#line 605 "ptx.y"
                                               { recognizer->add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); recognizer->change_double_operand_type(-1); recognizer->change_operand_lohi(1);}
#line 3088 "cuda-sim/ptx.tab.c"
    break;

  case 280: /* operand: IDENTIFIER PIPE IDENTIFIER HI_OPTION  */
#line 606 "ptx.y"
                                               { recognizer->add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); recognizer->change_double_operand_type(-1); recognizer->change_operand_lohi(2);}
#line 3094 "cuda-sim/ptx.tab.c"
    break;

  case 281: /* operand: IDENTIFIER BACKSLASH IDENTIFIER  */
#line 607 "ptx.y"
                                          { recognizer->add_2vector_operand((yyvsp[-2].string_value),(yyvsp[0].string_value)); recognizer->change_double_operand_type(-3);}
#line 3100 "cuda-sim/ptx.tab.c"
    break;

  case 282: /* operand: IDENTIFIER BACKSLASH IDENTIFIER LO_OPTION  */
#line 608 "ptx.y"
                                                    { recognizer->add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); recognizer->change_double_operand_type(-3); recognizer->change_operand_lohi(1);}
#line 3106 "cuda-sim/ptx.tab.c"
    break;

  case 283: /* operand: IDENTIFIER BACKSLASH IDENTIFIER HI_OPTION  */
#line 609 "ptx.y"
                                                    { recognizer->add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); recognizer->change_double_operand_type(-3); recognizer->change_operand_lohi(2);}
#line 3112 "cuda-sim/ptx.tab.c"
    break;

  case 284: /* vector_operand: LEFT_BRACE IDENTIFIER COMMA IDENTIFIER RIGHT_BRACE  */
#line 612 "ptx.y"
                                                                   { recognizer->add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); }
#line 3118 "cuda-sim/ptx.tab.c"
    break;

  case 285: /* vector_operand: LEFT_BRACE IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER RIGHT_BRACE  */
#line 613 "ptx.y"
                                                                                      { recognizer->add_3vector_operand((yyvsp[-5].string_value),(yyvsp[-3].string_value),(yyvsp[-1].string_value)); }
#line 3124 "cuda-sim/ptx.tab.c"
    break;

  case 286: /* vector_operand: LEFT_BRACE IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER RIGHT_BRACE  */
#line 614 "ptx.y"
                                                                                                       { recognizer->add_4vector_operand((yyvsp[-7].string_value),(yyvsp[-5].string_value),(yyvsp[-3].string_value),(yyvsp[-1].string_value)); }
#line 3130 "cuda-sim/ptx.tab.c"
    break;

  case 287: /* vector_operand: LEFT_BRACE IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER RIGHT_BRACE  */
#line 615 "ptx.y"
                                                                                                                                                                           { recognizer->add_8vector_operand((yyvsp[-15].string_value),(yyvsp[-13].string_value),(yyvsp[-11].string_value),(yyvsp[-9].string_value),(yyvsp[-7].string_value),(yyvsp[-5].string_value),(yyvsp[-3].string_value),(yyvsp[-1].string_value)); }
#line 3136 "cuda-sim/ptx.tab.c"
    break;

  case 288: /* vector_operand: LEFT_BRACE IDENTIFIER RIGHT_BRACE  */
#line 616 "ptx.y"
                                                    { recognizer->add_1vector_operand((yyvsp[-1].string_value)); }
#line 3142 "cuda-sim/ptx.tab.c"
    break;

  case 289: /* $@16: %empty  */
#line 619 "ptx.y"
                                                  { recognizer->add_scalar_operand((yyvsp[-1].string_value)); }
#line 3148 "cuda-sim/ptx.tab.c"
    break;

  case 291: /* builtin_operand: SPECIAL_REGISTER DIMENSION_MODIFIER  */
#line 624 "ptx.y"
                                                     { recognizer->add_builtin_operand((yyvsp[-1].int_value),(yyvsp[0].int_value)); }
#line 3154 "cuda-sim/ptx.tab.c"
    break;

  case 292: /* builtin_operand: SPECIAL_REGISTER  */
#line 625 "ptx.y"
                           { recognizer->add_builtin_operand((yyvsp[0].int_value),-1); }
#line 3160 "cuda-sim/ptx.tab.c"
    break;

  case 293: /* memory_operand: LEFT_SQUARE_BRACKET address_expression RIGHT_SQUARE_BRACKET  */
#line 628 "ptx.y"
                                                                             { recognizer->add_memory_operand(); }
#line 3166 "cuda-sim/ptx.tab.c"
    break;

  case 294: /* memory_operand: IDENTIFIER LEFT_SQUARE_BRACKET address_expression RIGHT_SQUARE_BRACKET  */
#line 629 "ptx.y"
                                                                                 { recognizer->add_memory_operand(); recognizer->change_memory_addr_space((yyvsp[-3].string_value)); }
#line 3172 "cuda-sim/ptx.tab.c"
    break;

  case 295: /* memory_operand: IDENTIFIER LEFT_SQUARE_BRACKET literal_operand RIGHT_SQUARE_BRACKET  */
#line 630 "ptx.y"
                                                                              { recognizer->change_memory_addr_space((yyvsp[-3].string_value)); }
#line 3178 "cuda-sim/ptx.tab.c"
    break;

  case 296: /* memory_operand: IDENTIFIER LEFT_SQUARE_BRACKET twin_operand RIGHT_SQUARE_BRACKET  */
#line 631 "ptx.y"
                                                                           { recognizer->change_memory_addr_space((yyvsp[-3].string_value)); recognizer->add_memory_operand();}
#line 3184 "cuda-sim/ptx.tab.c"
    break;

  case 297: /* memory_operand: MINUS memory_operand  */
#line 632 "ptx.y"
                               { recognizer->change_operand_neg(); }
#line 3190 "cuda-sim/ptx.tab.c"
    break;

  case 298: /* twin_operand: IDENTIFIER PLUS IDENTIFIER  */
#line 635 "ptx.y"
                                          { recognizer->add_double_operand((yyvsp[-2].string_value),(yyvsp[0].string_value)); recognizer->change_double_operand_type(1); }
#line 3196 "cuda-sim/ptx.tab.c"
    break;

  case 299: /* twin_operand: IDENTIFIER PLUS IDENTIFIER LO_OPTION  */
#line 636 "ptx.y"
                                               { recognizer->add_double_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); recognizer->change_double_operand_type(1); recognizer->change_operand_lohi(1); }
#line 3202 "cuda-sim/ptx.tab.c"
    break;

  case 300: /* twin_operand: IDENTIFIER PLUS IDENTIFIER HI_OPTION  */
#line 637 "ptx.y"
                                               { recognizer->add_double_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); recognizer->change_double_operand_type(1); recognizer->change_operand_lohi(2); }
#line 3208 "cuda-sim/ptx.tab.c"
    break;

  case 301: /* twin_operand: IDENTIFIER PLUS EQUALS IDENTIFIER  */
#line 638 "ptx.y"
                                             { recognizer->add_double_operand((yyvsp[-3].string_value),(yyvsp[0].string_value)); recognizer->change_double_operand_type(2); }
#line 3214 "cuda-sim/ptx.tab.c"
    break;

  case 302: /* twin_operand: IDENTIFIER PLUS EQUALS IDENTIFIER LO_OPTION  */
#line 639 "ptx.y"
                                                      { recognizer->add_double_operand((yyvsp[-4].string_value),(yyvsp[-1].string_value)); recognizer->change_double_operand_type(2); recognizer->change_operand_lohi(1); }
#line 3220 "cuda-sim/ptx.tab.c"
    break;

  case 303: /* twin_operand: IDENTIFIER PLUS EQUALS IDENTIFIER HI_OPTION  */
#line 640 "ptx.y"
                                                      { recognizer->add_double_operand((yyvsp[-4].string_value),(yyvsp[-1].string_value)); recognizer->change_double_operand_type(2); recognizer->change_operand_lohi(2); }
#line 3226 "cuda-sim/ptx.tab.c"
    break;

  case 304: /* twin_operand: IDENTIFIER PLUS EQUALS INT_OPERAND  */
#line 641 "ptx.y"
                                              { recognizer->add_address_operand((yyvsp[-3].string_value),(yyvsp[0].int_value)); recognizer->change_double_operand_type(3); }
#line 3232 "cuda-sim/ptx.tab.c"
    break;

  case 305: /* literal_operand: INT_OPERAND  */
#line 644 "ptx.y"
                              { recognizer->add_literal_int((yyvsp[0].int_value)); }
#line 3238 "cuda-sim/ptx.tab.c"
    break;

  case 306: /* literal_operand: FLOAT_OPERAND  */
#line 645 "ptx.y"
                        { recognizer->add_literal_float((yyvsp[0].float_value)); }
#line 3244 "cuda-sim/ptx.tab.c"
    break;

  case 307: /* literal_operand: DOUBLE_OPERAND  */
#line 646 "ptx.y"
                         { recognizer->add_literal_double((yyvsp[0].double_value)); }
#line 3250 "cuda-sim/ptx.tab.c"
    break;

  case 308: /* address_expression: IDENTIFIER  */
#line 649 "ptx.y"
                               { recognizer->add_address_operand((yyvsp[0].string_value),0); }
#line 3256 "cuda-sim/ptx.tab.c"
    break;

  case 309: /* address_expression: IDENTIFIER LO_OPTION  */
#line 650 "ptx.y"
                               { recognizer->add_address_operand((yyvsp[-1].string_value),0); recognizer->change_operand_lohi(1);}
#line 3262 "cuda-sim/ptx.tab.c"
    break;

  case 310: /* address_expression: IDENTIFIER HI_OPTION  */
#line 651 "ptx.y"
                               { recognizer->add_address_operand((yyvsp[-1].string_value),0); recognizer->change_operand_lohi(2); }
#line 3268 "cuda-sim/ptx.tab.c"
    break;

  case 311: /* address_expression: IDENTIFIER PLUS INT_OPERAND  */
#line 652 "ptx.y"
                                      { recognizer->add_address_operand((yyvsp[-2].string_value),(yyvsp[0].int_value)); }
#line 3274 "cuda-sim/ptx.tab.c"
    break;

  case 312: /* address_expression: INT_OPERAND  */
#line 653 "ptx.y"
                      { recognizer->add_address_operand2((yyvsp[0].int_value)); }
#line 3280 "cuda-sim/ptx.tab.c"
    break;


#line 3284 "cuda-sim/ptx.tab.c"

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
      yyerror (scanner, recognizer, YY_("syntax error"));
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
                      yytoken, &yylval, scanner, recognizer);
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner, recognizer);
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
  yyerror (scanner, recognizer, YY_("memory exhausted"));
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
                  yytoken, &yylval, scanner, recognizer);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner, recognizer);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 656 "ptx.y"


void syntax_not_implemented(yyscan_t yyscanner, ptx_recognizer* recognizer)
{
	printf("Parse error (%s): this syntax is not (yet) implemented:\n", recognizer->gpgpu_ctx->g_filename);
	ptx_error(yyscanner, recognizer, NULL);
	abort();
}
