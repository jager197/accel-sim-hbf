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
#define yyparse         ptx_parse
#define yylex           ptx_lex
#define yyerror         ptx_error
#define yydebug         ptx_debug
#define yynerrs         ptx_nerrs
#define yylval          ptx_lval
#define yychar          ptx_char


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
  YYSYMBOL_YYACCEPT = 176,                 /* $accept  */
  YYSYMBOL_input = 177,                    /* input  */
  YYSYMBOL_function_defn = 178,            /* function_defn  */
  YYSYMBOL_179_1 = 179,                    /* $@1  */
  YYSYMBOL_180_2 = 180,                    /* $@2  */
  YYSYMBOL_181_3 = 181,                    /* $@3  */
  YYSYMBOL_block_spec = 182,               /* block_spec  */
  YYSYMBOL_block_spec_list = 183,          /* block_spec_list  */
  YYSYMBOL_function_decl = 184,            /* function_decl  */
  YYSYMBOL_185_4 = 185,                    /* $@4  */
  YYSYMBOL_186_5 = 186,                    /* $@5  */
  YYSYMBOL_187_6 = 187,                    /* $@6  */
  YYSYMBOL_function_ident_param = 188,     /* function_ident_param  */
  YYSYMBOL_189_7 = 189,                    /* $@7  */
  YYSYMBOL_190_8 = 190,                    /* $@8  */
  YYSYMBOL_function_decl_header = 191,     /* function_decl_header  */
  YYSYMBOL_param_list = 192,               /* param_list  */
  YYSYMBOL_193_9 = 193,                    /* $@9  */
  YYSYMBOL_param_entry = 194,              /* param_entry  */
  YYSYMBOL_195_10 = 195,                   /* $@10  */
  YYSYMBOL_196_11 = 196,                   /* $@11  */
  YYSYMBOL_ptr_spec = 197,                 /* ptr_spec  */
  YYSYMBOL_ptr_space_spec = 198,           /* ptr_space_spec  */
  YYSYMBOL_ptr_align_spec = 199,           /* ptr_align_spec  */
  YYSYMBOL_statement_block = 200,          /* statement_block  */
  YYSYMBOL_statement_list = 201,           /* statement_list  */
  YYSYMBOL_202_12 = 202,                   /* $@12  */
  YYSYMBOL_203_13 = 203,                   /* $@13  */
  YYSYMBOL_directive_statement = 204,      /* directive_statement  */
  YYSYMBOL_variable_declaration = 205,     /* variable_declaration  */
  YYSYMBOL_variable_spec = 206,            /* variable_spec  */
  YYSYMBOL_identifier_list = 207,          /* identifier_list  */
  YYSYMBOL_identifier_spec = 208,          /* identifier_spec  */
  YYSYMBOL_var_spec_list = 209,            /* var_spec_list  */
  YYSYMBOL_var_spec = 210,                 /* var_spec  */
  YYSYMBOL_align_spec = 211,               /* align_spec  */
  YYSYMBOL_space_spec = 212,               /* space_spec  */
  YYSYMBOL_addressable_spec = 213,         /* addressable_spec  */
  YYSYMBOL_type_spec = 214,                /* type_spec  */
  YYSYMBOL_vector_spec = 215,              /* vector_spec  */
  YYSYMBOL_scalar_type = 216,              /* scalar_type  */
  YYSYMBOL_initializer_list = 217,         /* initializer_list  */
  YYSYMBOL_literal_list = 218,             /* literal_list  */
  YYSYMBOL_instruction_statement = 219,    /* instruction_statement  */
  YYSYMBOL_instruction = 220,              /* instruction  */
  YYSYMBOL_221_14 = 221,                   /* $@14  */
  YYSYMBOL_opcode_spec = 222,              /* opcode_spec  */
  YYSYMBOL_223_15 = 223,                   /* $@15  */
  YYSYMBOL_pred_spec = 224,                /* pred_spec  */
  YYSYMBOL_option_list = 225,              /* option_list  */
  YYSYMBOL_option = 226,                   /* option  */
  YYSYMBOL_atomic_operation_spec = 227,    /* atomic_operation_spec  */
  YYSYMBOL_rounding_mode = 228,            /* rounding_mode  */
  YYSYMBOL_floating_point_rounding_mode = 229, /* floating_point_rounding_mode  */
  YYSYMBOL_integer_rounding_mode = 230,    /* integer_rounding_mode  */
  YYSYMBOL_compare_spec = 231,             /* compare_spec  */
  YYSYMBOL_prmt_spec = 232,                /* prmt_spec  */
  YYSYMBOL_wmma_spec = 233,                /* wmma_spec  */
  YYSYMBOL_operand_list = 234,             /* operand_list  */
  YYSYMBOL_operand = 235,                  /* operand  */
  YYSYMBOL_vector_operand = 236,           /* vector_operand  */
  YYSYMBOL_tex_operand = 237,              /* tex_operand  */
  YYSYMBOL_238_16 = 238,                   /* $@16  */
  YYSYMBOL_builtin_operand = 239,          /* builtin_operand  */
  YYSYMBOL_memory_operand = 240,           /* memory_operand  */
  YYSYMBOL_twin_operand = 241,             /* twin_operand  */
  YYSYMBOL_literal_operand = 242,          /* literal_operand  */
  YYSYMBOL_address_expression = 243        /* address_expression  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */
#line 215 "ptx.y"

  	#include "ptx_parser.h"
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	void syntax_not_implemented();
	extern int g_func_decl;
	int ptx_lex(void);
	int ptx_error(const char *);

#line 364 "cuobjdump_to_ptxplus/ptx.tab.c"


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
#define YYLAST   615

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  176
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  68
/* YYNRULES -- Number of rules.  */
#define YYNRULES  300
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  420

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   430


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
     175
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   228,   228,   229,   230,   231,   234,   234,   235,   235,
     235,   238,   242,   243,   246,   247,   250,   250,   250,   251,
     251,   252,   255,   255,   255,   256,   259,   260,   261,   262,
     263,   264,   265,   266,   269,   270,   271,   271,   273,   273,
     274,   274,   276,   277,   278,   280,   281,   282,   283,   285,
     287,   289,   290,   291,   292,   293,   293,   294,   294,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   311,   312,   313,   314,   317,   319,   320,   322,   323,
     335,   336,   339,   340,   342,   343,   344,   345,   346,   347,
     350,   352,   353,   354,   357,   358,   359,   360,   361,   362,
     363,   364,   367,   368,   371,   372,   373,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   395,   396,   397,   400,
     401,   403,   404,   406,   407,   408,   410,   410,   411,   412,
     413,   414,   417,   417,   418,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   435,   436,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   463,   464,   465,   466,   467,
     468,   469,   470,   471,   472,   473,   474,   475,   476,   477,
     478,   479,   480,   483,   484,   485,   486,   487,   488,   489,
     490,   491,   492,   493,   496,   497,   500,   501,   502,   503,
     506,   507,   508,   509,   512,   513,   514,   515,   516,   517,
     518,   519,   520,   521,   522,   523,   524,   525,   526,   527,
     528,   529,   532,   533,   534,   535,   536,   537,   540,   541,
     550,   551,   553,   554,   555,   556,   557,   558,   559,   560,
     561,   562,   563,   564,   565,   566,   567,   568,   569,   570,
     571,   572,   575,   576,   577,   578,   579,   582,   582,   587,
     588,   591,   592,   593,   594,   595,   598,   599,   600,   601,
     602,   603,   604,   607,   608,   609,   612,   613,   614,   615,
     616
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
  "PRMT_RC16_MODE", "PRMT_ECL_MODE", "PRMT_ECR_MODE", "$accept", "input",
  "function_defn", "$@1", "$@2", "$@3", "block_spec", "block_spec_list",
  "function_decl", "$@4", "$@5", "$@6", "function_ident_param", "$@7",
  "$@8", "function_decl_header", "param_list", "$@9", "param_entry",
  "$@10", "$@11", "ptr_spec", "ptr_space_spec", "ptr_align_spec",
  "statement_block", "statement_list", "$@12", "$@13",
  "directive_statement", "variable_declaration", "variable_spec",
  "identifier_list", "identifier_spec", "var_spec_list", "var_spec",
  "align_spec", "space_spec", "addressable_spec", "type_spec",
  "vector_spec", "scalar_type", "initializer_list", "literal_list",
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

#define YYPACT_NINF (-290)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-145)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -290,   382,  -290,   -28,  -290,   -24,  -290,    15,    69,  -290,
    -290,  -290,   150,  -290,   198,  -290,  -290,  -290,  -290,  -290,
     165,  -290,   183,   180,    -4,   197,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,    -7,   -37,  -290,   128,   192,   521,  -290,  -290,
    -290,  -290,  -290,   546,  -290,  -290,   185,  -290,   264,   217,
     182,   210,   195,  -290,  -290,  -290,  -290,  -290,  -290,   216,
       6,  -290,   289,  -290,    86,   267,   234,  -290,  -290,  -290,
    -290,  -290,   307,   280,   312,  -290,   314,  -290,   451,  -290,
     316,   317,   319,  -290,     6,    11,   220,  -290,   144,   321,
     192,   136,   286,   322,  -290,   295,   133,   265,     3,   266,
     252,   216,  -290,  -290,   268,   146,   366,  -290,   299,  -290,
     216,  -290,  -290,  -290,   228,   230,   277,  -290,   235,  -290,
    -290,  -290,  -290,   136,  -290,  -290,   332,   305,   336,     1,
    -290,   259,   339,  -290,   216,  -290,  -290,  -290,  -290,   134,
     -19,   273,   233,   343,   344,   430,  -290,   318,  -290,  -290,
    -290,  -290,  -290,   288,   349,  -290,   521,   521,  -290,  -290,
    -290,  -290,   297,   -62,  -290,  -290,   352,  -290,   400,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,     1,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,    66,   362,   364,   368,   125,  -290,
     327,  -290,   114,    61,   102,  -290,  -290,  -290,   -60,   271,
     283,  -290,   340,   406,   192,   289,    11,  -290,   204,  -290,
    -290,   247,  -290,   249,  -290,   328,   330,   357,  -290,   139,
     174,  -290,  -290,  -290,   412,  -290,  -290,  -290,   260,   360,
     416,  -290,  -290,    -2,  -290,   388,   417,   194,   192,  -290,
    -290,   -49,  -290,  -290,   456,  -290,   -35,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,   372,  -290,   112,   393,  -290,   350,
     430,  -290,   437,  -290,  -290,  -290,  -290,   478,  -290,  -290,
    -290,  -290,  -290,   178,   231,   391,   453,  -290,   430,  -290,
    -290,  -290,    11,  -290,  -290,   214,  -290,  -290,   113,   425,
    -290,  -290,  -290,   480,  -290,   381,   115,   430,   483,  -290,
     384,   460,  -290,   491,   464,   496,   470,   500,   438,  -290
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,     0,    94,     0,    26,    88,     0,    29,
      95,    96,     0,    97,     0,    91,    98,    92,    99,   100,
       0,   101,     0,     0,    87,    89,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   104,   105,
     106,     4,     5,    21,     3,     0,     0,    75,    82,    86,
      84,    93,    85,     0,   102,    90,     0,    32,     0,     0,
       0,    65,    60,    62,    27,    30,    28,    31,    70,     0,
       0,    16,     0,    59,    78,    71,    76,    88,    87,    89,
      83,   103,     0,    66,     0,    69,     0,    61,    57,     7,
       0,     0,     0,    14,     9,     0,    25,    20,     0,     0,
       0,     0,     0,     0,    68,    63,   142,     0,     0,     0,
      55,     0,    51,    52,     0,   141,     0,    13,     0,    12,
       0,    15,    38,    40,     0,     0,     0,    80,     0,    77,
     293,   294,   295,     0,    72,    73,     0,     0,     0,     0,
     134,   145,     0,    50,     0,    53,    54,    58,   133,   252,
       0,   280,     0,     0,     0,     0,   140,   250,   258,   260,
     257,   255,   256,     0,     0,    10,     0,     0,    17,    23,
      81,    79,     0,     0,   131,    74,     0,    64,     0,   189,
     190,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   170,
     216,   217,   218,   219,   220,   221,   222,   223,   169,   177,
     178,   179,   180,   181,   182,   166,   168,   167,   204,   203,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   183,
     184,   171,   172,   173,   174,   175,   176,   185,   186,   188,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   242,   243,   244,   245,   246,   247,   162,   160,
     143,   158,   187,   163,   214,   215,   161,   165,   164,   148,
     150,   147,   149,   151,   152,   154,   153,   155,   156,   157,
     146,    56,   262,   264,     0,     0,     0,     0,   296,   300,
       0,   279,   254,     0,     0,   259,   285,   253,     0,     0,
       0,   135,     0,    42,     0,     0,    34,   130,     0,   129,
      67,     0,   159,   296,   293,     0,     0,     0,   261,   266,
     269,   277,   297,   298,     0,   281,   263,   265,   296,     0,
       0,   276,   136,     0,   251,   250,     0,     0,     0,    41,
      18,     0,    35,   132,     0,   248,     0,   284,   283,   282,
     267,   268,   270,   271,     0,   299,     0,     0,   139,     0,
       0,    11,     0,    48,    45,    46,    47,     0,    44,    39,
      36,    24,   249,   286,     0,     0,     0,   272,     0,   138,
      49,    43,     0,   287,   288,   289,   292,   278,     0,     0,
      37,   290,   291,     0,   273,     0,     0,     0,     0,   274,
       0,     0,   137,     0,     0,     0,     0,     0,     0,   275
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -290,  -290,  -290,  -290,  -290,  -290,   441,  -290,   548,  -290,
    -290,  -290,   232,  -290,  -290,  -290,  -290,  -290,  -289,  -290,
    -290,  -290,  -290,   175,    73,  -290,  -290,  -290,   -90,  -290,
     172,  -290,  -108,  -290,   494,  -290,  -290,  -126,   -75,  -290,
     493,   415,  -290,   440,   435,  -290,  -290,  -290,  -290,   291,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -125,  -124,
    -159,  -290,  -290,  -290,  -157,  -290,  -107,   272
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,    51,    79,    80,   130,   103,   104,   119,   105,
     315,    82,   107,   135,   316,    53,   351,   392,   134,   176,
     177,   348,   377,   378,    99,   120,   154,   121,    54,    55,
      56,    85,    86,    57,    58,    59,    60,    61,    62,    63,
      64,   144,   183,   123,   124,   367,   125,   149,   126,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   344,   345,
     168,   169,   364,   170,   171,   325,   172,   300
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     166,   167,   139,   305,   145,   306,   188,   -19,   122,   383,
     365,   318,    74,   340,     4,    75,    -8,    65,    -8,    -8,
      66,    10,    11,   268,   380,   298,   299,   352,    13,   100,
     155,   101,   102,    16,    67,    18,   184,    19,   132,    21,
     133,   309,   159,   140,   141,   142,   319,   151,   341,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,   269,   384,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   160,   381,    78,   161,
     162,   209,    -6,   400,   163,   338,   299,   164,    81,   152,
     323,   324,   141,   142,    68,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   368,   268,   339,   306,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,  -144,  -144,  -144,
    -144,   140,   141,   142,   108,   386,   403,   326,   408,   136,
     159,   140,   141,   142,   157,    69,   269,   336,   331,   337,
     303,    70,   372,   175,   304,   385,   349,   373,   332,    71,
     333,   353,   294,    76,   374,   375,    77,   292,   369,   293,
     387,   404,   360,   409,   361,    73,   376,   291,   334,   109,
      72,  -144,   294,    83,  -144,  -144,    84,   295,  -144,  -144,
     379,   296,  -144,   137,   160,   143,   297,   161,   162,   140,
     141,   142,   163,   354,   355,   164,   116,   362,    92,   363,
       3,   393,    94,   394,   399,     4,     5,    93,     6,     7,
       8,     9,    10,    11,    12,   395,   396,   302,  -144,    13,
      14,    15,   410,    96,    16,    17,    18,    95,    19,    20,
      21,   165,    22,    23,    24,    25,   117,   401,    97,   402,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    98,   118,   159,   140,   141,
     142,   303,   332,   106,   333,   304,   279,   280,   281,   282,
     110,   283,   164,   332,   111,   333,   284,   285,   313,   314,
     286,   112,   356,   113,   287,   288,   289,   114,   115,   146,
     153,   127,   128,   334,   129,   -22,   138,   147,   148,   150,
     116,    78,   174,   158,   178,   179,   180,   185,   186,   181,
     187,   160,     2,   290,   161,   162,   301,   307,   308,   163,
       3,   310,   164,   311,   312,     4,     5,   320,     6,     7,
       8,     9,    10,    11,    12,   317,   321,   328,   329,    13,
      14,    15,   330,   346,    16,    17,    18,   342,    19,    20,
      21,   347,    22,    23,    24,    25,   335,   357,   343,   358,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,   116,   359,   365,   294,     3,
     366,   370,   371,   382,     4,     5,   388,     6,     7,     8,
       9,    10,    11,    12,   159,   140,   141,   142,    13,    14,
      15,   164,   390,    16,    17,    18,   372,    19,    20,    21,
     397,    22,    23,    24,    25,   117,   389,   398,   405,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,   406,   118,   407,   411,   160,     3,
     412,   161,   162,   413,     4,   414,   163,   415,    87,   164,
     416,    10,    11,   417,   418,   131,   419,   350,    13,    52,
      15,    90,   391,    16,    17,    18,    91,    19,   182,    21,
     156,   173,   322,    88,    89,     0,   327,     0,     0,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47
};

static const yytype_int16 yycheck[] =
{
     125,   125,   110,   162,   111,   162,     5,    44,    98,    44,
      45,    73,    16,    73,    13,    19,    23,    45,    25,    26,
      44,    20,    21,   149,    73,    44,    45,   316,    27,    23,
     120,    25,    26,    32,    19,    34,   143,    36,    27,    38,
      29,   165,    44,    45,    46,    47,   108,    44,   108,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,   149,   110,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    98,   146,   105,   101,
     102,   100,   109,   392,   106,    44,    45,   109,   145,   106,
      44,    45,    46,    47,    45,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   146,   271,    44,   304,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,    44,    45,    46,
      47,    45,    46,    47,    98,    73,    73,   294,    73,    45,
      44,    45,    46,    47,   121,    45,   271,    83,    73,    85,
      98,     3,     8,   130,   102,   364,   314,    13,    83,    44,
      85,   318,    98,    16,    20,    21,    19,    83,   343,    85,
     108,   108,    83,   108,    85,    45,    32,   154,   103,   143,
      47,    98,    98,   105,   101,   102,    44,   103,   105,   106,
     348,   107,   109,    99,    98,   109,   112,   101,   102,    45,
      46,    47,   106,     6,     7,   109,     4,    83,    73,    85,
       8,    83,    45,    85,   388,    13,    14,     3,    16,    17,
      18,    19,    20,    21,    22,    44,    45,    44,   145,    27,
      28,    29,   407,    73,    32,    33,    34,   105,    36,    37,
      38,   145,    40,    41,    42,    43,    44,    83,   103,    85,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,   109,    74,    44,    45,    46,
      47,    98,    83,    44,    85,   102,    77,    78,    79,    80,
      73,    82,   109,    83,   110,    85,    87,    88,   176,   177,
      91,    44,   103,    73,    95,    96,    97,    45,    44,    73,
     108,    45,    45,   103,    45,   145,    45,    45,    73,   104,
       4,   105,    73,   105,   146,   145,    99,    45,    73,   144,
      44,    98,     0,    44,   101,   102,   113,    44,    44,   106,
       8,    73,   109,   105,    45,    13,    14,    45,    16,    17,
      18,    19,    20,    21,    22,   108,     6,    45,    44,    27,
      28,    29,    44,    73,    32,    33,    34,   146,    36,    37,
      38,    15,    40,    41,    42,    43,    99,    99,   145,    99,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,     4,    99,    45,    98,     8,
      44,    73,    45,     7,    13,    14,    73,    16,    17,    18,
      19,    20,    21,    22,    44,    45,    46,    47,    27,    28,
      29,   109,    45,    32,    33,    34,     8,    36,    37,    38,
      99,    40,    41,    42,    43,    44,   146,    44,    73,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    44,    74,   145,    44,    98,     8,
     146,   101,   102,    73,    13,    44,   106,    73,    17,   109,
      44,    20,    21,    73,    44,   104,   108,   315,    27,     1,
      29,    57,   377,    32,    33,    34,    63,    36,   143,    38,
     120,   126,   271,    42,    43,    -1,   294,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   177,     0,     8,    13,    14,    16,    17,    18,    19,
      20,    21,    22,    27,    28,    29,    32,    33,    34,    36,
      37,    38,    40,    41,    42,    43,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,   178,   184,   191,   204,   205,   206,   209,   210,   211,
     212,   213,   214,   215,   216,    45,    44,    19,    45,    45,
       3,    44,    47,    45,    16,    19,    16,    19,   105,   179,
     180,   145,   187,   105,    44,   207,   208,    17,    42,    43,
     210,   216,    73,     3,    45,   105,    73,   103,   109,   200,
      23,    25,    26,   182,   183,   185,    44,   188,    98,   143,
      73,   110,    44,    73,    45,    44,     4,    44,    74,   184,
     201,   203,   204,   219,   220,   222,   224,    45,    45,    45,
     181,   182,    27,    29,   194,   189,    45,    99,    45,   208,
      45,    46,    47,   109,   217,   242,    73,    45,    73,   223,
     104,    44,   106,   108,   202,   204,   219,   200,   105,    44,
      98,   101,   102,   106,   109,   145,   234,   235,   236,   237,
     239,   240,   242,   220,    73,   200,   195,   196,   146,   145,
      99,   144,   217,   218,   242,    45,    73,    44,     5,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,   100,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   213,   214,
     225,   226,   227,   228,   229,   230,   231,   232,   233,    77,
      78,    79,    80,    82,    87,    88,    91,    95,    96,    97,
      44,   200,    83,    85,    98,   103,   107,   112,    44,    45,
     243,   113,    44,    98,   102,   236,   240,    44,    44,   235,
      73,   105,    45,   206,   206,   186,   190,   108,    73,   108,
      45,     6,   225,    44,    45,   241,   242,   243,    45,    44,
      44,    73,    83,    85,   103,    99,    83,    85,    44,    44,
      73,   108,   146,   145,   234,   235,    73,    15,   197,   208,
     188,   192,   194,   242,     6,     7,   103,    99,    99,    99,
      83,    85,    83,    85,   238,    45,    44,   221,   146,   234,
      73,    45,     8,    13,    20,    21,    32,   198,   199,   208,
      73,   146,     7,    44,   110,   236,    73,   108,    73,   146,
      45,   199,   193,    83,    85,    44,    45,    99,    44,   235,
     194,    83,    85,    73,   108,    73,    44,   145,    73,   108,
     234,    44,   146,    73,    44,    73,    44,    73,    44,   108
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   176,   177,   177,   177,   177,   179,   178,   180,   181,
     178,   182,   182,   182,   183,   183,   185,   186,   184,   187,
     184,   184,   189,   190,   188,   188,   191,   191,   191,   191,
     191,   191,   191,   191,   192,   192,   193,   192,   195,   194,
     196,   194,   197,   197,   197,   198,   198,   198,   198,   199,
     200,   201,   201,   201,   201,   202,   201,   203,   201,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   205,   205,   205,   205,   206,   207,   207,   208,   208,
     208,   208,   209,   209,   210,   210,   210,   210,   210,   210,
     211,   212,   212,   212,   213,   213,   213,   213,   213,   213,
     213,   213,   214,   214,   215,   215,   215,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   217,
     217,   218,   218,   219,   219,   219,   221,   220,   220,   220,
     220,   220,   223,   222,   222,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   225,   225,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   227,   227,   227,   227,   227,   227,   227,
     227,   227,   227,   227,   228,   228,   229,   229,   229,   229,
     230,   230,   230,   230,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   232,   232,   232,   232,   232,   232,   233,   233,
     234,   234,   235,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   236,   236,   236,   236,   236,   238,   237,   239,
     239,   240,   240,   240,   240,   240,   241,   241,   241,   241,
     241,   241,   241,   242,   242,   242,   243,   243,   243,   243,
     243
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     0,     3,     0,     0,
       5,     6,     2,     2,     1,     2,     0,     0,     7,     0,
       3,     1,     0,     0,     6,     1,     1,     2,     2,     1,
       2,     2,     2,     2,     0,     1,     0,     4,     0,     5,
       0,     4,     0,     3,     2,     1,     1,     1,     1,     2,
       3,     1,     1,     2,     2,     0,     3,     0,     2,     2,
       2,     3,     2,     4,     6,     2,     3,     7,     4,     3,
       2,     2,     4,     4,     6,     1,     1,     3,     1,     4,
       3,     4,     1,     2,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     3,     2,     2,     3,     0,    11,     6,     5,
       2,     1,     0,     3,     1,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     4,
       1,     3,     1,     2,     2,     1,     1,     1,     1,     2,
       1,     3,     2,     3,     2,     3,     3,     4,     4,     3,
       4,     4,     5,     7,     9,    17,     3,     0,     6,     2,
       1,     3,     4,     4,     4,     2,     3,     4,     4,     4,
       5,     5,     4,     1,     1,     1,     1,     2,     2,     3,
       1
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
  case 6: /* $@1: %empty  */
#line 234 "ptx.y"
                             { set_symtab((yyvsp[0].ptr_value)); func_header(".skip"); }
#line 1725 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 7: /* function_defn: function_decl $@1 statement_block  */
#line 234 "ptx.y"
                                                                                       { end_function(); }
#line 1731 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 8: /* $@2: %empty  */
#line 235 "ptx.y"
                        { set_symtab((yyvsp[0].ptr_value)); }
#line 1737 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 9: /* $@3: %empty  */
#line 235 "ptx.y"
                                                            { func_header(".skip"); }
#line 1743 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 10: /* function_defn: function_decl $@2 block_spec_list $@3 statement_block  */
#line 235 "ptx.y"
                                                                                                      { end_function(); }
#line 1749 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 11: /* block_spec: MAXNTID_DIRECTIVE INT_OPERAND COMMA INT_OPERAND COMMA INT_OPERAND  */
#line 238 "ptx.y"
                                                                              {func_header_info_int(".maxntid", (yyvsp[-4].int_value));
										func_header_info_int(",", (yyvsp[-2].int_value));
										func_header_info_int(",", (yyvsp[0].int_value)); 
                                                                                maxnt_id((yyvsp[-4].int_value), (yyvsp[-2].int_value), (yyvsp[0].int_value));}
#line 1758 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 12: /* block_spec: MINNCTAPERSM_DIRECTIVE INT_OPERAND  */
#line 242 "ptx.y"
                                             { func_header_info_int(".minnctapersm", (yyvsp[0].int_value)); printf("GPGPU-Sim: Warning: .minnctapersm ignored. \n"); }
#line 1764 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 13: /* block_spec: MAXNCTAPERSM_DIRECTIVE INT_OPERAND  */
#line 243 "ptx.y"
                                             { func_header_info_int(".maxnctapersm", (yyvsp[0].int_value)); printf("GPGPU-Sim: Warning: .maxnctapersm ignored. \n"); }
#line 1770 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 16: /* $@4: %empty  */
#line 250 "ptx.y"
                                               { start_function((yyvsp[-1].int_value)); func_header_info("(");}
#line 1776 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 17: /* $@5: %empty  */
#line 250 "ptx.y"
                                                                                                                     {func_header_info(")");}
#line 1782 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 18: /* function_decl: function_decl_header LEFT_PAREN $@4 param_entry RIGHT_PAREN $@5 function_ident_param  */
#line 250 "ptx.y"
                                                                                                                                                                   { (yyval.ptr_value) = reset_symtab(); }
#line 1788 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 19: /* $@6: %empty  */
#line 251 "ptx.y"
                               { start_function((yyvsp[0].int_value)); }
#line 1794 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 20: /* function_decl: function_decl_header $@6 function_ident_param  */
#line 251 "ptx.y"
                                                                            { (yyval.ptr_value) = reset_symtab(); }
#line 1800 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 21: /* function_decl: function_decl_header  */
#line 252 "ptx.y"
                               { start_function((yyvsp[0].int_value)); add_function_name(""); g_func_decl=0; (yyval.ptr_value) = reset_symtab(); }
#line 1806 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 22: /* $@7: %empty  */
#line 255 "ptx.y"
                                 { add_function_name((yyvsp[0].string_value)); }
#line 1812 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 23: /* $@8: %empty  */
#line 255 "ptx.y"
                                                                       {func_header_info("(");}
#line 1818 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 24: /* function_ident_param: IDENTIFIER $@7 LEFT_PAREN $@8 param_list RIGHT_PAREN  */
#line 255 "ptx.y"
                                                                                                                       { g_func_decl=0; func_header_info(")"); }
#line 1824 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 25: /* function_ident_param: IDENTIFIER  */
#line 256 "ptx.y"
                     { add_function_name((yyvsp[0].string_value)); g_func_decl=0; }
#line 1830 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 26: /* function_decl_header: ENTRY_DIRECTIVE  */
#line 259 "ptx.y"
                                      { (yyval.int_value) = 1; g_func_decl=1; func_header(".entry"); }
#line 1836 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 27: /* function_decl_header: VISIBLE_DIRECTIVE ENTRY_DIRECTIVE  */
#line 260 "ptx.y"
                                            { (yyval.int_value) = 1; g_func_decl=1; func_header(".entry"); }
#line 1842 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 28: /* function_decl_header: WEAK_DIRECTIVE ENTRY_DIRECTIVE  */
#line 261 "ptx.y"
                                         { (yyval.int_value) = 1; g_func_decl=1; func_header(".entry"); }
#line 1848 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 29: /* function_decl_header: FUNC_DIRECTIVE  */
#line 262 "ptx.y"
                         { (yyval.int_value) = 0; g_func_decl=1; func_header(".func"); }
#line 1854 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 30: /* function_decl_header: VISIBLE_DIRECTIVE FUNC_DIRECTIVE  */
#line 263 "ptx.y"
                                           { (yyval.int_value) = 0; g_func_decl=1; func_header(".func"); }
#line 1860 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 31: /* function_decl_header: WEAK_DIRECTIVE FUNC_DIRECTIVE  */
#line 264 "ptx.y"
                                        { (yyval.int_value) = 0; g_func_decl=1; func_header(".func"); }
#line 1866 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 32: /* function_decl_header: EXTERN_DIRECTIVE FUNC_DIRECTIVE  */
#line 265 "ptx.y"
                                          { (yyval.int_value) = 2; g_func_decl=1; func_header(".func"); }
#line 1872 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 33: /* function_decl_header: WEAK_DIRECTIVE FUNC_DIRECTIVE  */
#line 266 "ptx.y"
                                        { (yyval.int_value) = 0; g_func_decl=1; func_header(".func"); }
#line 1878 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 35: /* param_list: param_entry  */
#line 270 "ptx.y"
                      { add_directive(); }
#line 1884 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 36: /* $@9: %empty  */
#line 271 "ptx.y"
                           {func_header_info(",");}
#line 1890 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 37: /* param_list: param_list COMMA $@9 param_entry  */
#line 271 "ptx.y"
                                                                { add_directive(); }
#line 1896 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 38: /* $@10: %empty  */
#line 273 "ptx.y"
                             { add_space_spec(param_space_unclassified,0); }
#line 1902 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 39: /* param_entry: PARAM_DIRECTIVE $@10 variable_spec ptr_spec identifier_spec  */
#line 273 "ptx.y"
                                                                                                                    { add_function_arg(); }
#line 1908 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 40: /* $@11: %empty  */
#line 274 "ptx.y"
                        { add_space_spec(reg_space,0); }
#line 1914 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 41: /* param_entry: REG_DIRECTIVE $@11 variable_spec identifier_spec  */
#line 274 "ptx.y"
                                                                                       { add_function_arg(); }
#line 1920 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 45: /* ptr_space_spec: GLOBAL_DIRECTIVE  */
#line 280 "ptx.y"
                                 { add_ptr_spec(global_space); }
#line 1926 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 46: /* ptr_space_spec: LOCAL_DIRECTIVE  */
#line 281 "ptx.y"
                                 { add_ptr_spec(local_space); }
#line 1932 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 47: /* ptr_space_spec: SHARED_DIRECTIVE  */
#line 282 "ptx.y"
                                 { add_ptr_spec(shared_space); }
#line 1938 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 48: /* ptr_space_spec: CONST_DIRECTIVE  */
#line 283 "ptx.y"
                                            { add_ptr_spec(global_space); }
#line 1944 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 51: /* statement_list: directive_statement  */
#line 289 "ptx.y"
                                    { add_directive(); }
#line 1950 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 52: /* statement_list: instruction_statement  */
#line 290 "ptx.y"
                                { add_instruction(); }
#line 1956 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 53: /* statement_list: statement_list directive_statement  */
#line 291 "ptx.y"
                                             { add_directive(); }
#line 1962 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 54: /* statement_list: statement_list instruction_statement  */
#line 292 "ptx.y"
                                               { add_instruction(); }
#line 1968 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 55: /* $@12: %empty  */
#line 293 "ptx.y"
                         {start_inst_group();}
#line 1974 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 56: /* statement_list: statement_list $@12 statement_block  */
#line 293 "ptx.y"
                                                               {end_inst_group();}
#line 1980 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 57: /* $@13: %empty  */
#line 294 "ptx.y"
          {start_inst_group();}
#line 1986 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 58: /* statement_list: $@13 statement_block  */
#line 294 "ptx.y"
                                                {end_inst_group();}
#line 1992 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 60: /* directive_statement: VERSION_DIRECTIVE DOUBLE_OPERAND  */
#line 298 "ptx.y"
                                           { add_version_info((yyvsp[0].double_value), 0); }
#line 1998 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 61: /* directive_statement: VERSION_DIRECTIVE DOUBLE_OPERAND PLUS  */
#line 299 "ptx.y"
                                                { add_version_info((yyvsp[-1].double_value),1); }
#line 2004 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 62: /* directive_statement: ADDRESS_SIZE_DIRECTIVE INT_OPERAND  */
#line 300 "ptx.y"
                                             {/*Do nothing*/}
#line 2010 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 63: /* directive_statement: TARGET_DIRECTIVE IDENTIFIER COMMA IDENTIFIER  */
#line 301 "ptx.y"
                                                       { target_header2((yyvsp[-2].string_value),(yyvsp[0].string_value)); }
#line 2016 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 64: /* directive_statement: TARGET_DIRECTIVE IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER  */
#line 302 "ptx.y"
                                                                        { target_header3((yyvsp[-4].string_value),(yyvsp[-2].string_value),(yyvsp[0].string_value)); }
#line 2022 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 65: /* directive_statement: TARGET_DIRECTIVE IDENTIFIER  */
#line 303 "ptx.y"
                                      { target_header((yyvsp[0].string_value)); }
#line 2028 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 66: /* directive_statement: FILE_DIRECTIVE INT_OPERAND STRING  */
#line 304 "ptx.y"
                                            { add_file((yyvsp[-1].int_value),(yyvsp[0].string_value)); }
#line 2034 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 67: /* directive_statement: FILE_DIRECTIVE INT_OPERAND STRING COMMA INT_OPERAND COMMA INT_OPERAND  */
#line 305 "ptx.y"
                                                                                { add_file((yyvsp[-5].int_value),(yyvsp[-4].string_value)); }
#line 2040 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 69: /* directive_statement: PRAGMA_DIRECTIVE STRING SEMI_COLON  */
#line 307 "ptx.y"
                                             { add_pragma((yyvsp[-1].string_value)); }
#line 2046 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 70: /* directive_statement: function_decl SEMI_COLON  */
#line 308 "ptx.y"
                                   {/*Do nothing*/}
#line 2052 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 71: /* variable_declaration: variable_spec identifier_list  */
#line 311 "ptx.y"
                                                    { add_variables(); }
#line 2058 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 72: /* variable_declaration: variable_spec identifier_spec EQUALS initializer_list  */
#line 312 "ptx.y"
                                                                { add_variables(); }
#line 2064 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 73: /* variable_declaration: variable_spec identifier_spec EQUALS literal_operand  */
#line 313 "ptx.y"
                                                               { add_variables(); }
#line 2070 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 74: /* variable_declaration: CONSTPTR_DIRECTIVE IDENTIFIER COMMA IDENTIFIER COMMA INT_OPERAND  */
#line 314 "ptx.y"
                                                                           { add_constptr((yyvsp[-4].string_value), (yyvsp[-2].string_value), (yyvsp[0].int_value)); }
#line 2076 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 75: /* variable_spec: var_spec_list  */
#line 317 "ptx.y"
                             { set_variable_type(); }
#line 2082 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 78: /* identifier_spec: IDENTIFIER  */
#line 322 "ptx.y"
                            { add_identifier((yyvsp[0].string_value),0,NON_ARRAY_IDENTIFIER); func_header_info((yyvsp[0].string_value));}
#line 2088 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 79: /* identifier_spec: IDENTIFIER LEFT_ANGLE_BRACKET INT_OPERAND RIGHT_ANGLE_BRACKET  */
#line 323 "ptx.y"
                                                                        { func_header_info((yyvsp[-3].string_value)); func_header_info_int("<", (yyvsp[-1].int_value)); func_header_info(">");
		int i,lbase,l;
		char *id = NULL;
		lbase = strlen((yyvsp[-3].string_value));
		for( i=0; i < (yyvsp[-1].int_value); i++ ) { 
			l = lbase + (int)log10(i+1)+10;
			id = (char*) malloc(l);
			snprintf(id,l,"%s%u",(yyvsp[-3].string_value),i);
			add_identifier(id,0,NON_ARRAY_IDENTIFIER); 
		}
		free((yyvsp[-3].string_value));
	}
#line 2105 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 80: /* identifier_spec: IDENTIFIER LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET  */
#line 335 "ptx.y"
                                                              { add_identifier((yyvsp[-2].string_value),0,ARRAY_IDENTIFIER_NO_DIM); func_header_info((yyvsp[-2].string_value)); func_header_info("["); func_header_info("]");}
#line 2111 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 81: /* identifier_spec: IDENTIFIER LEFT_SQUARE_BRACKET INT_OPERAND RIGHT_SQUARE_BRACKET  */
#line 336 "ptx.y"
                                                                          { add_identifier((yyvsp[-3].string_value),(yyvsp[-1].int_value),ARRAY_IDENTIFIER); func_header_info((yyvsp[-3].string_value)); func_header_info_int("[",(yyvsp[-1].int_value)); func_header_info("]");}
#line 2117 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 88: /* var_spec: EXTERN_DIRECTIVE  */
#line 346 "ptx.y"
                           { add_extern_spec(); }
#line 2123 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 90: /* align_spec: ALIGN_DIRECTIVE INT_OPERAND  */
#line 350 "ptx.y"
                                        { add_alignment_spec((yyvsp[0].int_value)); }
#line 2129 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 91: /* space_spec: REG_DIRECTIVE  */
#line 352 "ptx.y"
                          {  add_space_spec(reg_space,0); }
#line 2135 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 92: /* space_spec: SREG_DIRECTIVE  */
#line 353 "ptx.y"
                          {  add_space_spec(reg_space,0); }
#line 2141 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 94: /* addressable_spec: CONST_DIRECTIVE  */
#line 357 "ptx.y"
                                  {  add_space_spec(const_space,(yyvsp[0].int_value)); }
#line 2147 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 95: /* addressable_spec: GLOBAL_DIRECTIVE  */
#line 358 "ptx.y"
                                  {  add_space_spec(global_space,0); }
#line 2153 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 96: /* addressable_spec: LOCAL_DIRECTIVE  */
#line 359 "ptx.y"
                                  {  add_space_spec(local_space,0); }
#line 2159 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 97: /* addressable_spec: PARAM_DIRECTIVE  */
#line 360 "ptx.y"
                                  {  add_space_spec(param_space_unclassified,0); }
#line 2165 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 98: /* addressable_spec: SHARED_DIRECTIVE  */
#line 361 "ptx.y"
                                  {  add_space_spec(shared_space,0); }
#line 2171 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 99: /* addressable_spec: SSTARR_DIRECTIVE  */
#line 362 "ptx.y"
                              {  add_space_spec(sstarr_space,0); }
#line 2177 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 100: /* addressable_spec: SURF_DIRECTIVE  */
#line 363 "ptx.y"
                                  {  add_space_spec(surf_space,0); }
#line 2183 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 101: /* addressable_spec: TEX_DIRECTIVE  */
#line 364 "ptx.y"
                                  {  add_space_spec(tex_space,0); }
#line 2189 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 104: /* vector_spec: V2_TYPE  */
#line 371 "ptx.y"
                      {  add_option(V2_TYPE); func_header_info(".v2");}
#line 2195 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 105: /* vector_spec: V3_TYPE  */
#line 372 "ptx.y"
                      {  add_option(V3_TYPE); func_header_info(".v3");}
#line 2201 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 106: /* vector_spec: V4_TYPE  */
#line 373 "ptx.y"
                      {  add_option(V4_TYPE); func_header_info(".v4");}
#line 2207 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 107: /* scalar_type: S8_TYPE  */
#line 376 "ptx.y"
                     { add_scalar_type_spec( S8_TYPE ); }
#line 2213 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 108: /* scalar_type: S16_TYPE  */
#line 377 "ptx.y"
                     { add_scalar_type_spec( S16_TYPE ); }
#line 2219 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 109: /* scalar_type: S32_TYPE  */
#line 378 "ptx.y"
                     { add_scalar_type_spec( S32_TYPE ); }
#line 2225 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 110: /* scalar_type: S64_TYPE  */
#line 379 "ptx.y"
                     { add_scalar_type_spec( S64_TYPE ); }
#line 2231 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 111: /* scalar_type: U8_TYPE  */
#line 380 "ptx.y"
                     { add_scalar_type_spec( U8_TYPE ); }
#line 2237 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 112: /* scalar_type: U16_TYPE  */
#line 381 "ptx.y"
                     { add_scalar_type_spec( U16_TYPE ); }
#line 2243 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 113: /* scalar_type: U32_TYPE  */
#line 382 "ptx.y"
                     { add_scalar_type_spec( U32_TYPE ); }
#line 2249 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 114: /* scalar_type: U64_TYPE  */
#line 383 "ptx.y"
                     { add_scalar_type_spec( U64_TYPE ); }
#line 2255 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 115: /* scalar_type: F16_TYPE  */
#line 384 "ptx.y"
                     { add_scalar_type_spec( F16_TYPE ); }
#line 2261 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 116: /* scalar_type: F32_TYPE  */
#line 385 "ptx.y"
                     { add_scalar_type_spec( F32_TYPE ); }
#line 2267 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 117: /* scalar_type: F64_TYPE  */
#line 386 "ptx.y"
                     { add_scalar_type_spec( F64_TYPE ); }
#line 2273 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 118: /* scalar_type: FF64_TYPE  */
#line 387 "ptx.y"
                      { add_scalar_type_spec( FF64_TYPE ); }
#line 2279 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 119: /* scalar_type: B8_TYPE  */
#line 388 "ptx.y"
                     { add_scalar_type_spec( B8_TYPE );  }
#line 2285 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 120: /* scalar_type: B16_TYPE  */
#line 389 "ptx.y"
                     { add_scalar_type_spec( B16_TYPE ); }
#line 2291 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 121: /* scalar_type: B32_TYPE  */
#line 390 "ptx.y"
                     { add_scalar_type_spec( B32_TYPE ); }
#line 2297 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 122: /* scalar_type: B64_TYPE  */
#line 391 "ptx.y"
                     { add_scalar_type_spec( B64_TYPE ); }
#line 2303 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 123: /* scalar_type: BB64_TYPE  */
#line 392 "ptx.y"
                      { add_scalar_type_spec( BB64_TYPE ); }
#line 2309 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 124: /* scalar_type: BB128_TYPE  */
#line 393 "ptx.y"
                       { add_scalar_type_spec( BB128_TYPE ); }
#line 2315 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 125: /* scalar_type: PRED_TYPE  */
#line 394 "ptx.y"
                     { add_scalar_type_spec( PRED_TYPE ); }
#line 2321 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 126: /* scalar_type: TEXREF_TYPE  */
#line 395 "ptx.y"
                       { add_scalar_type_spec( TEXREF_TYPE ); }
#line 2327 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 127: /* scalar_type: SAMPLERREF_TYPE  */
#line 396 "ptx.y"
                           { add_scalar_type_spec( SAMPLERREF_TYPE ); }
#line 2333 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 128: /* scalar_type: SURFREF_TYPE  */
#line 397 "ptx.y"
                        { add_scalar_type_spec( SURFREF_TYPE ); }
#line 2339 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 129: /* initializer_list: LEFT_BRACE literal_list RIGHT_BRACE  */
#line 400 "ptx.y"
                                                      { add_array_initializer(); }
#line 2345 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 130: /* initializer_list: LEFT_BRACE initializer_list RIGHT_BRACE  */
#line 401 "ptx.y"
                                                  { syntax_not_implemented(); }
#line 2351 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 134: /* instruction_statement: IDENTIFIER COLON  */
#line 407 "ptx.y"
                           { add_label((yyvsp[-1].string_value)); }
#line 2357 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 136: /* $@14: %empty  */
#line 410 "ptx.y"
                                                        { set_return(); }
#line 2363 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 142: /* $@15: %empty  */
#line 417 "ptx.y"
                    { add_opcode((yyvsp[0].int_value)); }
#line 2369 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 144: /* opcode_spec: OPCODE  */
#line 418 "ptx.y"
                 { add_opcode((yyvsp[0].int_value)); }
#line 2375 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 145: /* pred_spec: PRED IDENTIFIER  */
#line 420 "ptx.y"
                            { add_pred((yyvsp[0].string_value),0, -1); }
#line 2381 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 146: /* pred_spec: PRED EXCLAMATION IDENTIFIER  */
#line 421 "ptx.y"
                                      { add_pred((yyvsp[0].string_value),1, -1); }
#line 2387 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 147: /* pred_spec: PRED IDENTIFIER LT_OPTION  */
#line 422 "ptx.y"
                                     { add_pred((yyvsp[-1].string_value),0,1); }
#line 2393 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 148: /* pred_spec: PRED IDENTIFIER EQ_OPTION  */
#line 423 "ptx.y"
                                     { add_pred((yyvsp[-1].string_value),0,2); }
#line 2399 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 149: /* pred_spec: PRED IDENTIFIER LE_OPTION  */
#line 424 "ptx.y"
                                     { add_pred((yyvsp[-1].string_value),0,3); }
#line 2405 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 150: /* pred_spec: PRED IDENTIFIER NE_OPTION  */
#line 425 "ptx.y"
                                     { add_pred((yyvsp[-1].string_value),0,5); }
#line 2411 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 151: /* pred_spec: PRED IDENTIFIER GE_OPTION  */
#line 426 "ptx.y"
                                     { add_pred((yyvsp[-1].string_value),0,6); }
#line 2417 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 152: /* pred_spec: PRED IDENTIFIER EQU_OPTION  */
#line 427 "ptx.y"
                                      { add_pred((yyvsp[-1].string_value),0,10); }
#line 2423 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 153: /* pred_spec: PRED IDENTIFIER GTU_OPTION  */
#line 428 "ptx.y"
                                      { add_pred((yyvsp[-1].string_value),0,12); }
#line 2429 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 154: /* pred_spec: PRED IDENTIFIER NEU_OPTION  */
#line 429 "ptx.y"
                                      { add_pred((yyvsp[-1].string_value),0,13); }
#line 2435 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 155: /* pred_spec: PRED IDENTIFIER CF_OPTION  */
#line 430 "ptx.y"
                                     { add_pred((yyvsp[-1].string_value),0,17); }
#line 2441 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 156: /* pred_spec: PRED IDENTIFIER SF_OPTION  */
#line 431 "ptx.y"
                                     { add_pred((yyvsp[-1].string_value),0,19); }
#line 2447 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 157: /* pred_spec: PRED IDENTIFIER NSF_OPTION  */
#line 432 "ptx.y"
                                      { add_pred((yyvsp[-1].string_value),0,28); }
#line 2453 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 166: /* option: SYNC_OPTION  */
#line 444 "ptx.y"
                      { add_option(SYNC_OPTION); }
#line 2459 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 167: /* option: ARRIVE_OPTION  */
#line 445 "ptx.y"
                        { add_option(ARRIVE_OPTION); }
#line 2465 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 168: /* option: RED_OPTION  */
#line 446 "ptx.y"
                     { add_option(RED_OPTION); }
#line 2471 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 169: /* option: UNI_OPTION  */
#line 447 "ptx.y"
                     { add_option(UNI_OPTION); }
#line 2477 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 170: /* option: WIDE_OPTION  */
#line 448 "ptx.y"
                      { add_option(WIDE_OPTION); }
#line 2483 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 171: /* option: ANY_OPTION  */
#line 449 "ptx.y"
                     { add_option(ANY_OPTION); }
#line 2489 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 172: /* option: ALL_OPTION  */
#line 450 "ptx.y"
                     { add_option(ALL_OPTION); }
#line 2495 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 173: /* option: BALLOT_OPTION  */
#line 451 "ptx.y"
                        { add_option(BALLOT_OPTION); }
#line 2501 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 174: /* option: GLOBAL_OPTION  */
#line 452 "ptx.y"
                        { add_option(GLOBAL_OPTION); }
#line 2507 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 175: /* option: CTA_OPTION  */
#line 453 "ptx.y"
                     { add_option(CTA_OPTION); }
#line 2513 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 176: /* option: SYS_OPTION  */
#line 454 "ptx.y"
                     { add_option(SYS_OPTION); }
#line 2519 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 177: /* option: GEOM_MODIFIER_1D  */
#line 455 "ptx.y"
                           { add_option(GEOM_MODIFIER_1D); }
#line 2525 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 178: /* option: GEOM_MODIFIER_2D  */
#line 456 "ptx.y"
                           { add_option(GEOM_MODIFIER_2D); }
#line 2531 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 179: /* option: GEOM_MODIFIER_3D  */
#line 457 "ptx.y"
                           { add_option(GEOM_MODIFIER_3D); }
#line 2537 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 180: /* option: SAT_OPTION  */
#line 458 "ptx.y"
                     { add_option(SAT_OPTION); }
#line 2543 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 181: /* option: FTZ_OPTION  */
#line 459 "ptx.y"
                     { add_option(FTZ_OPTION); }
#line 2549 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 182: /* option: NEG_OPTION  */
#line 460 "ptx.y"
                     { add_option(NEG_OPTION); }
#line 2555 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 183: /* option: APPROX_OPTION  */
#line 461 "ptx.y"
                        { add_option(APPROX_OPTION); }
#line 2561 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 184: /* option: FULL_OPTION  */
#line 462 "ptx.y"
                      { add_option(FULL_OPTION); }
#line 2567 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 185: /* option: EXIT_OPTION  */
#line 463 "ptx.y"
                      { add_option(EXIT_OPTION); }
#line 2573 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 186: /* option: ABS_OPTION  */
#line 464 "ptx.y"
                     { add_option(ABS_OPTION); }
#line 2579 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 188: /* option: TO_OPTION  */
#line 466 "ptx.y"
                    { add_option(TO_OPTION); }
#line 2585 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 189: /* option: HALF_OPTION  */
#line 467 "ptx.y"
                      { add_option(HALF_OPTION); }
#line 2591 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 190: /* option: EXTP_OPTION  */
#line 468 "ptx.y"
                      { add_option(EXTP_OPTION); }
#line 2597 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 191: /* option: CA_OPTION  */
#line 469 "ptx.y"
                    { add_option(CA_OPTION); }
#line 2603 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 192: /* option: CG_OPTION  */
#line 470 "ptx.y"
                    { add_option(CG_OPTION); }
#line 2609 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 193: /* option: CS_OPTION  */
#line 471 "ptx.y"
                    { add_option(CS_OPTION); }
#line 2615 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 194: /* option: LU_OPTION  */
#line 472 "ptx.y"
                    { add_option(LU_OPTION); }
#line 2621 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 195: /* option: CV_OPTION  */
#line 473 "ptx.y"
                    { add_option(CV_OPTION); }
#line 2627 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 196: /* option: WB_OPTION  */
#line 474 "ptx.y"
                    { add_option(WB_OPTION); }
#line 2633 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 197: /* option: WT_OPTION  */
#line 475 "ptx.y"
                    { add_option(WT_OPTION); }
#line 2639 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 198: /* option: NC_OPTION  */
#line 476 "ptx.y"
                    { add_option(NC_OPTION); }
#line 2645 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 199: /* option: UP_OPTION  */
#line 477 "ptx.y"
                    { add_option(UP_OPTION); }
#line 2651 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 200: /* option: DOWN_OPTION  */
#line 478 "ptx.y"
                      { add_option(DOWN_OPTION); }
#line 2657 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 201: /* option: BFLY_OPTION  */
#line 479 "ptx.y"
                      { add_option(BFLY_OPTION); }
#line 2663 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 202: /* option: IDX_OPTION  */
#line 480 "ptx.y"
                     { add_option(IDX_OPTION); }
#line 2669 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 203: /* atomic_operation_spec: ATOMIC_AND  */
#line 483 "ptx.y"
                                  { add_option(ATOMIC_AND); }
#line 2675 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 204: /* atomic_operation_spec: ATOMIC_POPC  */
#line 484 "ptx.y"
                      { add_option(ATOMIC_POPC); }
#line 2681 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 205: /* atomic_operation_spec: ATOMIC_OR  */
#line 485 "ptx.y"
                    { add_option(ATOMIC_OR); }
#line 2687 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 206: /* atomic_operation_spec: ATOMIC_XOR  */
#line 486 "ptx.y"
                     { add_option(ATOMIC_XOR); }
#line 2693 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 207: /* atomic_operation_spec: ATOMIC_CAS  */
#line 487 "ptx.y"
                     { add_option(ATOMIC_CAS); }
#line 2699 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 208: /* atomic_operation_spec: ATOMIC_EXCH  */
#line 488 "ptx.y"
                      { add_option(ATOMIC_EXCH); }
#line 2705 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 209: /* atomic_operation_spec: ATOMIC_ADD  */
#line 489 "ptx.y"
                     { add_option(ATOMIC_ADD); }
#line 2711 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 210: /* atomic_operation_spec: ATOMIC_INC  */
#line 490 "ptx.y"
                     { add_option(ATOMIC_INC); }
#line 2717 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 211: /* atomic_operation_spec: ATOMIC_DEC  */
#line 491 "ptx.y"
                     { add_option(ATOMIC_DEC); }
#line 2723 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 212: /* atomic_operation_spec: ATOMIC_MIN  */
#line 492 "ptx.y"
                     { add_option(ATOMIC_MIN); }
#line 2729 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 213: /* atomic_operation_spec: ATOMIC_MAX  */
#line 493 "ptx.y"
                     { add_option(ATOMIC_MAX); }
#line 2735 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 216: /* floating_point_rounding_mode: RN_OPTION  */
#line 500 "ptx.y"
                                        { add_option(RN_OPTION); }
#line 2741 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 217: /* floating_point_rounding_mode: RZ_OPTION  */
#line 501 "ptx.y"
                    { add_option(RZ_OPTION); }
#line 2747 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 218: /* floating_point_rounding_mode: RM_OPTION  */
#line 502 "ptx.y"
                    { add_option(RM_OPTION); }
#line 2753 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 219: /* floating_point_rounding_mode: RP_OPTION  */
#line 503 "ptx.y"
                    { add_option(RP_OPTION); }
#line 2759 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 220: /* integer_rounding_mode: RNI_OPTION  */
#line 506 "ptx.y"
                                  { add_option(RNI_OPTION); }
#line 2765 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 221: /* integer_rounding_mode: RZI_OPTION  */
#line 507 "ptx.y"
                     { add_option(RZI_OPTION); }
#line 2771 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 222: /* integer_rounding_mode: RMI_OPTION  */
#line 508 "ptx.y"
                     { add_option(RMI_OPTION); }
#line 2777 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 223: /* integer_rounding_mode: RPI_OPTION  */
#line 509 "ptx.y"
                     { add_option(RPI_OPTION); }
#line 2783 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 224: /* compare_spec: EQ_OPTION  */
#line 512 "ptx.y"
                       { add_option(EQ_OPTION); }
#line 2789 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 225: /* compare_spec: NE_OPTION  */
#line 513 "ptx.y"
                    { add_option(NE_OPTION); }
#line 2795 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 226: /* compare_spec: LT_OPTION  */
#line 514 "ptx.y"
                    { add_option(LT_OPTION); }
#line 2801 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 227: /* compare_spec: LE_OPTION  */
#line 515 "ptx.y"
                    { add_option(LE_OPTION); }
#line 2807 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 228: /* compare_spec: GT_OPTION  */
#line 516 "ptx.y"
                    { add_option(GT_OPTION); }
#line 2813 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 229: /* compare_spec: GE_OPTION  */
#line 517 "ptx.y"
                    { add_option(GE_OPTION); }
#line 2819 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 230: /* compare_spec: LO_OPTION  */
#line 518 "ptx.y"
                    { add_option(LO_OPTION); }
#line 2825 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 231: /* compare_spec: LS_OPTION  */
#line 519 "ptx.y"
                    { add_option(LS_OPTION); }
#line 2831 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 232: /* compare_spec: HI_OPTION  */
#line 520 "ptx.y"
                    { add_option(HI_OPTION); }
#line 2837 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 233: /* compare_spec: HS_OPTION  */
#line 521 "ptx.y"
                     { add_option(HS_OPTION); }
#line 2843 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 234: /* compare_spec: EQU_OPTION  */
#line 522 "ptx.y"
                     { add_option(EQU_OPTION); }
#line 2849 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 235: /* compare_spec: NEU_OPTION  */
#line 523 "ptx.y"
                     { add_option(NEU_OPTION); }
#line 2855 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 236: /* compare_spec: LTU_OPTION  */
#line 524 "ptx.y"
                     { add_option(LTU_OPTION); }
#line 2861 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 237: /* compare_spec: LEU_OPTION  */
#line 525 "ptx.y"
                     { add_option(LEU_OPTION); }
#line 2867 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 238: /* compare_spec: GTU_OPTION  */
#line 526 "ptx.y"
                     { add_option(GTU_OPTION); }
#line 2873 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 239: /* compare_spec: GEU_OPTION  */
#line 527 "ptx.y"
                     { add_option(GEU_OPTION); }
#line 2879 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 240: /* compare_spec: NUM_OPTION  */
#line 528 "ptx.y"
                     { add_option(NUM_OPTION); }
#line 2885 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 241: /* compare_spec: NAN_OPTION  */
#line 529 "ptx.y"
                     { add_option(NAN_OPTION); }
#line 2891 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 242: /* prmt_spec: PRMT_F4E_MODE  */
#line 532 "ptx.y"
                         { add_option( PRMT_F4E_MODE); }
#line 2897 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 243: /* prmt_spec: PRMT_B4E_MODE  */
#line 533 "ptx.y"
                         { add_option( PRMT_B4E_MODE); }
#line 2903 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 244: /* prmt_spec: PRMT_RC8_MODE  */
#line 534 "ptx.y"
                         { add_option( PRMT_RC8_MODE); }
#line 2909 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 245: /* prmt_spec: PRMT_RC16_MODE  */
#line 535 "ptx.y"
                         { add_option( PRMT_RC16_MODE);}
#line 2915 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 246: /* prmt_spec: PRMT_ECL_MODE  */
#line 536 "ptx.y"
                         { add_option( PRMT_ECL_MODE); }
#line 2921 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 247: /* prmt_spec: PRMT_ECR_MODE  */
#line 537 "ptx.y"
                         { add_option( PRMT_ECR_MODE); }
#line 2927 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 248: /* wmma_spec: WMMA_DIRECTIVE LAYOUT CONFIGURATION  */
#line 540 "ptx.y"
                                              {add_space_spec(global_space,0);add_ptr_spec(global_space); add_wmma_option((yyvsp[-2].int_value));add_wmma_option((yyvsp[-1].int_value));add_wmma_option((yyvsp[0].int_value));}
#line 2933 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 249: /* wmma_spec: WMMA_DIRECTIVE LAYOUT LAYOUT CONFIGURATION  */
#line 541 "ptx.y"
                                                    {add_wmma_option((yyvsp[-3].int_value));add_wmma_option((yyvsp[-2].int_value));add_wmma_option((yyvsp[-1].int_value));add_wmma_option((yyvsp[0].int_value));}
#line 2939 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 252: /* operand: IDENTIFIER  */
#line 553 "ptx.y"
                     { add_scalar_operand( (yyvsp[0].string_value) ); }
#line 2945 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 253: /* operand: EXCLAMATION IDENTIFIER  */
#line 554 "ptx.y"
                                 { add_neg_pred_operand( (yyvsp[0].string_value) ); }
#line 2951 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 254: /* operand: MINUS IDENTIFIER  */
#line 555 "ptx.y"
                            { add_scalar_operand( (yyvsp[0].string_value) ); change_operand_neg(); }
#line 2957 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 259: /* operand: MINUS vector_operand  */
#line 560 "ptx.y"
                               { change_operand_neg(); }
#line 2963 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 261: /* operand: IDENTIFIER PLUS INT_OPERAND  */
#line 562 "ptx.y"
                                      { add_address_operand((yyvsp[-2].string_value),(yyvsp[0].int_value)); }
#line 2969 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 262: /* operand: IDENTIFIER LO_OPTION  */
#line 563 "ptx.y"
                               { add_scalar_operand( (yyvsp[-1].string_value) ); change_operand_lohi(1);}
#line 2975 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 263: /* operand: MINUS IDENTIFIER LO_OPTION  */
#line 564 "ptx.y"
                                     { add_scalar_operand( (yyvsp[-1].string_value) ); change_operand_lohi(1); change_operand_neg();}
#line 2981 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 264: /* operand: IDENTIFIER HI_OPTION  */
#line 565 "ptx.y"
                               { add_scalar_operand( (yyvsp[-1].string_value) ); change_operand_lohi(2);}
#line 2987 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 265: /* operand: MINUS IDENTIFIER HI_OPTION  */
#line 566 "ptx.y"
                                     { add_scalar_operand( (yyvsp[-1].string_value) ); change_operand_lohi(2); change_operand_neg();}
#line 2993 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 266: /* operand: IDENTIFIER PIPE IDENTIFIER  */
#line 567 "ptx.y"
                                     { add_2vector_operand((yyvsp[-2].string_value),(yyvsp[0].string_value)); change_double_operand_type(-1);}
#line 2999 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 267: /* operand: IDENTIFIER PIPE IDENTIFIER LO_OPTION  */
#line 568 "ptx.y"
                                               { add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); change_double_operand_type(-1); change_operand_lohi(1);}
#line 3005 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 268: /* operand: IDENTIFIER PIPE IDENTIFIER HI_OPTION  */
#line 569 "ptx.y"
                                               { add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); change_double_operand_type(-1); change_operand_lohi(2);}
#line 3011 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 269: /* operand: IDENTIFIER BACKSLASH IDENTIFIER  */
#line 570 "ptx.y"
                                          { add_2vector_operand((yyvsp[-2].string_value),(yyvsp[0].string_value)); change_double_operand_type(-3);}
#line 3017 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 270: /* operand: IDENTIFIER BACKSLASH IDENTIFIER LO_OPTION  */
#line 571 "ptx.y"
                                                    { add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); change_double_operand_type(-3); change_operand_lohi(1);}
#line 3023 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 271: /* operand: IDENTIFIER BACKSLASH IDENTIFIER HI_OPTION  */
#line 572 "ptx.y"
                                                    { add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); change_double_operand_type(-3); change_operand_lohi(2);}
#line 3029 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 272: /* vector_operand: LEFT_BRACE IDENTIFIER COMMA IDENTIFIER RIGHT_BRACE  */
#line 575 "ptx.y"
                                                                   { add_2vector_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); }
#line 3035 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 273: /* vector_operand: LEFT_BRACE IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER RIGHT_BRACE  */
#line 576 "ptx.y"
                                                                                      { add_3vector_operand((yyvsp[-5].string_value),(yyvsp[-3].string_value),(yyvsp[-1].string_value)); }
#line 3041 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 274: /* vector_operand: LEFT_BRACE IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER RIGHT_BRACE  */
#line 577 "ptx.y"
                                                                                                       { add_4vector_operand((yyvsp[-7].string_value),(yyvsp[-5].string_value),(yyvsp[-3].string_value),(yyvsp[-1].string_value)); }
#line 3047 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 275: /* vector_operand: LEFT_BRACE IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER RIGHT_BRACE  */
#line 578 "ptx.y"
                                                                                                                                                                           { add_8vector_operand((yyvsp[-15].string_value),(yyvsp[-13].string_value),(yyvsp[-11].string_value),(yyvsp[-9].string_value),(yyvsp[-7].string_value),(yyvsp[-5].string_value),(yyvsp[-3].string_value),(yyvsp[-1].string_value)); }
#line 3053 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 276: /* vector_operand: LEFT_BRACE IDENTIFIER RIGHT_BRACE  */
#line 579 "ptx.y"
                                                    { add_1vector_operand((yyvsp[-1].string_value)); }
#line 3059 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 277: /* $@16: %empty  */
#line 582 "ptx.y"
                                                  { add_scalar_operand((yyvsp[-1].string_value)); }
#line 3065 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 279: /* builtin_operand: SPECIAL_REGISTER DIMENSION_MODIFIER  */
#line 587 "ptx.y"
                                                     { add_builtin_operand((yyvsp[-1].int_value),(yyvsp[0].int_value)); }
#line 3071 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 280: /* builtin_operand: SPECIAL_REGISTER  */
#line 588 "ptx.y"
                           { add_builtin_operand((yyvsp[0].int_value),-1); }
#line 3077 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 281: /* memory_operand: LEFT_SQUARE_BRACKET address_expression RIGHT_SQUARE_BRACKET  */
#line 591 "ptx.y"
                                                                             { add_memory_operand(); }
#line 3083 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 282: /* memory_operand: IDENTIFIER LEFT_SQUARE_BRACKET address_expression RIGHT_SQUARE_BRACKET  */
#line 592 "ptx.y"
                                                                                 { add_memory_operand(); change_memory_addr_space((yyvsp[-3].string_value)); }
#line 3089 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 283: /* memory_operand: IDENTIFIER LEFT_SQUARE_BRACKET literal_operand RIGHT_SQUARE_BRACKET  */
#line 593 "ptx.y"
                                                                              { change_memory_addr_space((yyvsp[-3].string_value)); }
#line 3095 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 284: /* memory_operand: IDENTIFIER LEFT_SQUARE_BRACKET twin_operand RIGHT_SQUARE_BRACKET  */
#line 594 "ptx.y"
                                                                           { change_memory_addr_space((yyvsp[-3].string_value)); add_memory_operand();}
#line 3101 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 285: /* memory_operand: MINUS memory_operand  */
#line 595 "ptx.y"
                               { change_operand_neg(); }
#line 3107 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 286: /* twin_operand: IDENTIFIER PLUS IDENTIFIER  */
#line 598 "ptx.y"
                                          { add_double_operand((yyvsp[-2].string_value),(yyvsp[0].string_value)); change_double_operand_type(1); }
#line 3113 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 287: /* twin_operand: IDENTIFIER PLUS IDENTIFIER LO_OPTION  */
#line 599 "ptx.y"
                                               { add_double_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); change_double_operand_type(1); change_operand_lohi(1); }
#line 3119 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 288: /* twin_operand: IDENTIFIER PLUS IDENTIFIER HI_OPTION  */
#line 600 "ptx.y"
                                               { add_double_operand((yyvsp[-3].string_value),(yyvsp[-1].string_value)); change_double_operand_type(1); change_operand_lohi(2); }
#line 3125 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 289: /* twin_operand: IDENTIFIER PLUS EQUALS IDENTIFIER  */
#line 601 "ptx.y"
                                             { add_double_operand((yyvsp[-3].string_value),(yyvsp[0].string_value)); change_double_operand_type(2); }
#line 3131 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 290: /* twin_operand: IDENTIFIER PLUS EQUALS IDENTIFIER LO_OPTION  */
#line 602 "ptx.y"
                                                      { add_double_operand((yyvsp[-4].string_value),(yyvsp[-1].string_value)); change_double_operand_type(2); change_operand_lohi(1); }
#line 3137 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 291: /* twin_operand: IDENTIFIER PLUS EQUALS IDENTIFIER HI_OPTION  */
#line 603 "ptx.y"
                                                      { add_double_operand((yyvsp[-4].string_value),(yyvsp[-1].string_value)); change_double_operand_type(2); change_operand_lohi(2); }
#line 3143 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 292: /* twin_operand: IDENTIFIER PLUS EQUALS INT_OPERAND  */
#line 604 "ptx.y"
                                              { add_address_operand((yyvsp[-3].string_value),(yyvsp[0].int_value)); change_double_operand_type(3); }
#line 3149 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 293: /* literal_operand: INT_OPERAND  */
#line 607 "ptx.y"
                              { add_literal_int((yyvsp[0].int_value)); }
#line 3155 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 294: /* literal_operand: FLOAT_OPERAND  */
#line 608 "ptx.y"
                        { add_literal_float((yyvsp[0].float_value)); }
#line 3161 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 295: /* literal_operand: DOUBLE_OPERAND  */
#line 609 "ptx.y"
                         { add_literal_double((yyvsp[0].double_value)); }
#line 3167 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 296: /* address_expression: IDENTIFIER  */
#line 612 "ptx.y"
                               { add_address_operand((yyvsp[0].string_value),0); }
#line 3173 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 297: /* address_expression: IDENTIFIER LO_OPTION  */
#line 613 "ptx.y"
                               { add_address_operand((yyvsp[-1].string_value),0); change_operand_lohi(1);}
#line 3179 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 298: /* address_expression: IDENTIFIER HI_OPTION  */
#line 614 "ptx.y"
                               { add_address_operand((yyvsp[-1].string_value),0); change_operand_lohi(2); }
#line 3185 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 299: /* address_expression: IDENTIFIER PLUS INT_OPERAND  */
#line 615 "ptx.y"
                                      { add_address_operand((yyvsp[-2].string_value),(yyvsp[0].int_value)); }
#line 3191 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;

  case 300: /* address_expression: INT_OPERAND  */
#line 616 "ptx.y"
                      { add_address_operand2((yyvsp[0].int_value)); }
#line 3197 "cuobjdump_to_ptxplus/ptx.tab.c"
    break;


#line 3201 "cuobjdump_to_ptxplus/ptx.tab.c"

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

#line 619 "ptx.y"


extern int ptx_lineno;
extern const char *g_filename;

void syntax_not_implemented()
{
	printf("Parse error (%s:%u): this syntax is not (yet) implemented:\n",g_filename,ptx_lineno);
	ptx_error(NULL);
	abort();
}
