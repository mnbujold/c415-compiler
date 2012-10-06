/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 5 "compiler.y"

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "myerror.h"

extern symbol 		*sList;  
extern myerror 		*eList;  
extern symbol 		*sSelected;  
extern int 			iserror;
extern int 			lineno, oldlineno;
extern int 			last_column;
extern char 		*errortext;
extern int              errorTextLength;
extern int 			looperrordetection;

void yyerror(const char *str)
{
	eList = addError(eList, str, last_column, lineno);

}

int yywrap()
{
        return 1;
} 





/* Line 268 of yacc.c  */
#line 105 "compiler.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 293 of yacc.c  */
#line 42 "compiler.y"

        int number;
        char string[4096];



/* Line 293 of yacc.c  */
#line 206 "compiler.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 218 "compiler.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   390

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  126
/* YYNRULES -- Number of states.  */
#define YYNSTATES  254

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
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
      55,    56,    57,    58
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     8,    10,    19,    26,    29,    34,    35,
      39,    42,    44,    48,    52,    53,    57,    60,    61,    63,
      67,    71,    73,    75,    77,    79,    83,    87,    89,    93,
      98,   102,   105,   109,   113,   115,   119,   121,   125,   129,
     133,   134,   138,   141,   143,   147,   151,   155,   156,   158,
     160,   163,   168,   173,   180,   183,   187,   191,   194,   196,
     200,   204,   208,   213,   217,   222,   226,   228,   232,   236,
     238,   240,   241,   245,   247,   249,   252,   256,   258,   262,
     265,   268,   272,   276,   278,   282,   286,   290,   294,   298,
     302,   304,   307,   310,   314,   318,   322,   324,   328,   332,
     336,   340,   344,   346,   348,   352,   356,   358,   361,   363,
     365,   367,   369,   372,   376,   380,   384,   386,   393,   398,
     403,   405,   407,   409,   416,   421,   423
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      60,     0,    -1,    61,    62,    88,    39,    -1,     1,    -1,
      18,    47,    37,    47,    44,    47,    38,    40,    -1,    18,
      47,    37,     1,    38,    40,    -1,     1,    40,    -1,    63,
      66,    78,    81,    -1,    -1,     6,    64,    40,    -1,     1,
      40,    -1,    65,    -1,    64,    40,    65,    -1,    47,    24,
      95,    -1,    -1,    21,    67,    40,    -1,     1,    40,    -1,
      -1,    68,    -1,    67,    40,    68,    -1,    47,    24,    69,
      -1,    70,    -1,    73,    -1,    71,    -1,    47,    -1,    37,
      72,    38,    -1,    37,     1,    38,    -1,    47,    -1,    72,
      44,    47,    -1,     4,    74,    15,    69,    -1,    19,    76,
      10,    -1,     1,    10,    -1,    42,    75,    43,    -1,    42,
       1,    43,    -1,    95,    -1,    95,    45,    95,    -1,    77,
      -1,    76,    40,    77,    -1,     1,    40,    76,    -1,    47,
      41,    69,    -1,    -1,    22,    79,    40,    -1,     1,    40,
      -1,    80,    -1,    79,    40,    80,    -1,    47,    41,    69,
      -1,    47,    44,    80,    -1,    -1,    82,    -1,    83,    -1,
      82,    83,    -1,    84,    62,    88,    40,    -1,    17,    47,
      85,    40,    -1,    12,    47,    85,    41,    47,    40,    -1,
       1,    40,    -1,    37,    86,    38,    -1,    37,     1,    38,
      -1,    37,    38,    -1,    87,    -1,    86,    40,    87,    -1,
       1,    40,    86,    -1,    69,    41,    69,    -1,    22,    69,
      41,    69,    -1,     5,    89,    10,    -1,     1,     5,    89,
      10,    -1,     5,     1,    10,    -1,    90,    -1,    89,    40,
      90,    -1,     1,    40,    89,    -1,    91,    -1,   104,    -1,
      -1,    93,    36,    95,    -1,    92,    -1,    88,    -1,   102,
      38,    -1,    47,    37,    38,    -1,    47,    -1,    93,    39,
      47,    -1,    94,    43,    -1,     1,    43,    -1,    93,    42,
      95,    -1,    94,    44,    95,    -1,    96,    -1,    95,    24,
      96,    -1,    95,    25,    96,    -1,    95,    28,    96,    -1,
      95,    26,    96,    -1,    95,    29,    96,    -1,    95,    27,
      96,    -1,    97,    -1,    30,    97,    -1,    31,    97,    -1,
      96,    30,    97,    -1,    96,    31,    97,    -1,    96,    16,
      97,    -1,    98,    -1,    97,    32,    98,    -1,    97,    33,
      98,    -1,    97,    34,    98,    -1,    97,    35,    98,    -1,
      97,     3,    98,    -1,    93,    -1,    99,    -1,    37,    95,
      38,    -1,    37,     1,    38,    -1,   101,    -1,    14,    98,
      -1,   100,    -1,    58,    -1,    52,    -1,    53,    -1,   102,
      38,    -1,    47,    37,    38,    -1,    47,    37,   103,    -1,
     102,    44,   103,    -1,    95,    -1,    13,    95,    20,   105,
       9,    90,    -1,    13,    95,    20,    90,    -1,    23,    95,
       8,    90,    -1,     7,    -1,    11,    -1,    91,    -1,    13,
      95,    20,   105,     9,   105,    -1,    23,    95,     8,   105,
      -1,     7,    -1,    11,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    83,    83,    84,    98,    99,   100,   103,   106,   107,
     108,   111,   112,   115,   119,   120,   121,   124,   125,   126,
     129,   132,   133,   136,   137,   140,   141,   144,   145,   148,
     149,   150,   153,   154,   157,   158,   161,   162,   163,   166,
     169,   170,   171,   174,   175,   178,   179,   182,   183,   186,
     187,   190,   193,   194,   195,   198,   199,   200,   203,   204,
     205,   208,   209,   212,   213,   214,   217,   218,   219,   222,
     223,   226,   227,   228,   229,   232,   233,   236,   237,   238,
     239,   242,   243,   246,   247,   248,   249,   250,   251,   252,
     255,   256,   257,   258,   259,   260,   262,   263,   264,   265,
     266,   267,   270,   271,   272,   273,   274,   275,   278,   279,
     282,   283,   286,   287,   290,   291,   294,   297,   298,   299,
     300,   301,   304,   305,   306,   307,   308
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "AND", "ARRAY", "BEGIN_", "CONST",
  "CONTINUE", "DO", "ELSE", "END", "EXIT", "FUNCTION", "IF", "NOT", "OF",
  "OR", "PROCEDURE", "PROGRAM", "RECORD", "THEN", "TYPE", "VAR", "WHILE",
  "ISEQUAL", "NOTEQUAL", "LESSTHAN", "GREATERTHAN", "LESSTHANEQUALS",
  "GREATERTHANEQUALS", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "DIV", "MOD",
  "ASSIGN", "LEFTPAREN", "RIGHTPAREN", "PERIOD", "SEMICOLON", "COLON",
  "LEFTBRACKET", "RIGHTBRACKET", "COMMA", "DOUBLEPERIOD", "INVALIDTOKEN",
  "ID", "RETURN", "UNKNOWN", "UNCLOSED_COMMENT", "UNCLOSED_QUOTE",
  "INT_CONST", "REAL_CONST", "BOOL", "CHAR", "INT", "REAL", "STRING",
  "$accept", "program", "program_head", "decls", "const_decl_part",
  "const_decl_list", "const_decl", "type_decl_part", "type_decl_list",
  "type_decl", "type", "simple_type", "scalar_type", "scalar_list",
  "structured_type", "closed_array_type", "array_type", "field_list",
  "field", "var_decl_part", "var_decl_list", "var_decl", "proc_decl_part",
  "proc_decl_list", "proc_decl", "proc_heading", "f_parm_decl",
  "f_parm_list", "f_parm", "compound_stat", "stat_list", "stat",
  "simple_stat", "proc_invok", "var", "subscripted_var", "expr",
  "simple_expr", "term", "factor", "unsigned_const", "unsigned_num",
  "func_invok", "plist_finvok", "parm", "struct_stat", "matched_stat", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    59,    60,    60,    61,    61,    61,    62,    63,    63,
      63,    64,    64,    65,    66,    66,    66,    67,    67,    67,
      68,    69,    69,    70,    70,    71,    71,    72,    72,    73,
      73,    73,    74,    74,    75,    75,    76,    76,    76,    77,
      78,    78,    78,    79,    79,    80,    80,    81,    81,    82,
      82,    83,    84,    84,    84,    85,    85,    85,    86,    86,
      86,    87,    87,    88,    88,    88,    89,    89,    89,    90,
      90,    91,    91,    91,    91,    92,    92,    93,    93,    93,
      93,    94,    94,    95,    95,    95,    95,    95,    95,    95,
      96,    96,    96,    96,    96,    96,    97,    97,    97,    97,
      97,    97,    98,    98,    98,    98,    98,    98,    99,    99,
     100,   100,   101,   101,   102,   102,   103,   104,   104,   104,
     104,   104,   105,   105,   105,   105,   105
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     1,     8,     6,     2,     4,     0,     3,
       2,     1,     3,     3,     0,     3,     2,     0,     1,     3,
       3,     1,     1,     1,     1,     3,     3,     1,     3,     4,
       3,     2,     3,     3,     1,     3,     1,     3,     3,     3,
       0,     3,     2,     1,     3,     3,     3,     0,     1,     1,
       2,     4,     4,     6,     2,     3,     3,     2,     1,     3,
       3,     3,     4,     3,     4,     3,     1,     3,     3,     1,
       1,     0,     3,     1,     1,     2,     3,     1,     3,     2,
       2,     3,     3,     1,     3,     3,     3,     3,     3,     3,
       1,     2,     2,     3,     3,     3,     1,     3,     3,     3,
       3,     3,     1,     1,     3,     3,     1,     2,     1,     1,
       1,     1,     2,     3,     3,     3,     1,     6,     4,     4,
       1,     1,     1,     6,     4,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     3,     0,     0,     0,     6,     0,     1,     0,     0,
       0,     0,     0,    10,     0,     0,    11,     0,     0,     0,
       0,    17,     0,     0,     0,     0,     9,     0,     0,   120,
     121,     0,     0,    77,    74,     0,    66,    69,    73,     0,
       0,     0,    70,     2,    16,     0,     0,    18,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    77,   110,
     111,   109,   102,    13,    83,    90,    96,   103,   108,   106,
       0,    12,     0,     0,    65,     0,    80,     0,     0,     0,
      63,     0,     0,     0,     0,    79,     0,    75,     0,     0,
      15,    42,     0,     0,    43,     0,     0,     0,     7,     0,
      49,     0,     5,     0,   107,    91,    92,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   112,    64,    68,     0,     0,    76,
     116,   114,     0,    67,    72,    78,    81,    82,   115,     0,
       0,     0,     0,    24,    20,    21,    23,    22,    19,     0,
       0,    41,    54,     0,     0,    50,     0,     0,   105,   104,
     113,    84,    85,    87,    89,    86,    88,    95,    93,    94,
     101,    97,    98,    99,   100,   120,   121,     0,     0,   118,
      69,     0,   119,    31,     0,     0,     0,     0,     0,    36,
       0,    27,     0,    45,    46,    44,     0,     0,     0,     0,
       4,     0,     0,     0,     0,     0,    34,     0,     0,     0,
      30,     0,    26,    25,     0,     0,     0,    57,     0,     0,
      58,     0,    52,    51,     0,     0,   117,    80,    32,     0,
      29,    38,    39,    37,    28,    56,     0,     0,     0,    55,
       0,     0,     0,   124,    35,     0,    60,     0,    61,    59,
      53,     0,    62,   123
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,    10,    11,    15,    16,    22,    46,    47,
     218,   145,   146,   192,   147,   185,   205,   188,   189,    50,
      93,    94,    98,    99,   100,   101,   197,   219,   220,    34,
      35,    36,    37,    38,    62,    40,   130,    64,    65,    66,
      67,    68,    69,    70,   131,    42,   181
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -123
static const yytype_int16 yypact[] =
{
      90,   -26,   -15,    21,   278,  -123,    51,  -123,    -1,     1,
      23,   321,    22,  -123,    80,    74,  -123,   143,    24,   110,
     112,   114,   322,   121,   122,    37,     1,   229,    12,  -123,
    -123,    37,    37,   132,  -123,     5,  -123,  -123,  -123,   231,
      76,    -8,  -123,  -123,  -123,   146,   136,  -123,   139,   125,
     254,   140,   145,   147,   130,   130,   130,   144,   156,  -123,
    -123,  -123,    55,   354,   168,     9,  -123,  -123,  -123,  -123,
       2,  -123,    13,    77,  -123,   229,  -123,   338,   280,    69,
    -123,   240,    37,   153,    37,  -123,    37,  -123,    37,   225,
     114,  -123,    99,   163,  -123,   170,   165,   166,  -123,   284,
    -123,   278,  -123,   180,  -123,     9,     9,    58,   323,   104,
      37,    37,    37,    37,    37,    37,   130,   130,   130,   130,
     130,   130,   130,   130,  -123,  -123,   192,   214,   240,  -123,
     354,  -123,    11,  -123,   354,  -123,   354,   354,  -123,   210,
     196,    25,    26,  -123,  -123,  -123,  -123,  -123,  -123,   225,
     125,   125,  -123,   206,   206,  -123,    23,   208,  -123,  -123,
    -123,   168,   168,   168,   168,   168,   168,     9,     9,     9,
    -123,  -123,  -123,  -123,  -123,   247,   249,    37,    37,  -123,
     259,   268,  -123,  -123,   164,   266,   242,   245,    83,  -123,
     253,  -123,    48,  -123,  -123,  -123,   227,   251,   257,   258,
    -123,   348,   286,   240,   281,   282,   292,   225,    25,   225,
    -123,   255,  -123,  -123,   283,    75,   225,  -123,   287,    92,
    -123,   285,  -123,  -123,   214,   214,  -123,   314,  -123,    37,
    -123,   291,  -123,  -123,  -123,  -123,   167,   294,   225,  -123,
     256,   296,   331,  -123,   354,    88,   301,   225,  -123,  -123,
    -123,   214,  -123,  -123
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -123,  -123,  -123,   244,  -123,  -123,   320,  -123,  -123,   263,
     -83,  -123,  -123,  -123,  -123,  -123,  -123,   148,   149,  -123,
    -123,   -12,  -123,  -123,   260,  -123,   200,   119,   117,    -6,
     -14,   -78,  -122,  -123,   -18,  -123,   -24,   274,   -36,   -44,
    -123,  -123,  -123,   -16,   302,  -123,   -88
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -127
static const yytype_int16 yytable[] =
{
      39,    63,    41,   133,    19,   180,   144,    77,    78,    39,
     104,    41,   119,    73,     5,    80,    27,    27,    27,   105,
     106,     7,    74,    23,    17,    28,   186,   190,    18,    18,
      87,    29,     6,   108,   -71,    30,    88,    31,    53,    13,
     124,   120,   121,   122,   123,    81,    88,    32,    14,   179,
     182,    54,    75,    75,    76,    76,    76,    39,   134,    41,
     136,   126,   137,    39,   -71,    41,   193,    55,    56,    24,
      53,    33,   187,   191,    57,   170,   171,   172,   173,   174,
     167,   168,   169,    54,    58,   183,   213,   125,    12,    59,
      60,     1,   214,   210,    83,    61,   158,    84,   183,    55,
      56,    76,   180,   180,    25,    53,    57,   129,     2,    39,
      39,    41,    41,   235,    26,   236,    58,    81,    54,    85,
      86,    59,    60,   211,   230,   226,   232,    61,   236,   180,
     239,    53,   240,   237,    55,    56,   242,   243,   194,   195,
     149,    57,   160,   150,    54,   107,   179,   182,    27,    43,
     199,    58,    44,   201,   202,   248,    59,    60,    54,    51,
     206,    45,    61,   253,   252,   204,    52,    57,   245,    79,
      89,   140,    92,   226,    55,    56,    90,    58,    54,    91,
     102,    57,    59,    60,   116,    39,   141,    41,    61,   216,
      76,    58,   103,   109,    55,    56,    59,    60,   117,   118,
     135,    57,    61,   151,   142,   244,    39,    39,    41,    41,
     152,    58,   153,   154,   143,   132,    59,    60,   157,    18,
     183,   175,    61,   -71,   -71,   176,   139,   177,   215,   140,
      72,   140,    81,    39,    18,    41,    29,   178,   184,   -71,
      30,   132,    31,   196,   141,    18,   141,    29,   200,   216,
     -71,    30,    32,    31,   -71,    95,  -125,   139,  -126,   -47,
     140,    33,   142,    32,   142,   217,    96,    82,  -122,   -71,
      83,    97,   143,    84,   143,   141,    33,   203,   216,     8,
     -71,   207,   208,    -8,     9,    95,   209,    33,   128,   -48,
      -8,   212,   221,   142,   225,    -8,    96,   222,   223,    -8,
      -8,    97,   187,   143,   110,   111,   112,   113,   114,   115,
     110,   111,   112,   113,   114,   115,   110,   111,   112,   113,
     114,   115,    20,    48,   227,   228,   -14,   -40,   238,   -33,
     234,   211,   241,   -14,   -40,   247,   250,   229,   -14,   -40,
     251,   240,    21,   -14,    49,   156,    71,   110,   111,   112,
     113,   114,   115,   148,   198,   246,   231,   249,   127,   155,
     233,   159,   110,   111,   112,   113,   114,   115,   224,     0,
       0,     0,   110,   111,   112,   113,   114,   115,   110,   111,
     112,   113,   114,   115,   161,   162,   163,   164,   165,   166,
     138
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-123))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      18,    25,    18,    81,    10,   127,    89,    31,    32,    27,
      54,    27,     3,    27,    40,    10,     5,     5,     5,    55,
      56,     0,    10,     1,     1,     1,     1,     1,     5,     5,
      38,     7,    47,    57,    10,    11,    44,    13,     1,    40,
      38,    32,    33,    34,    35,    40,    44,    23,    47,   127,
     128,    14,    40,    40,    43,    43,    43,    75,    82,    75,
      84,    75,    86,    81,    40,    81,   149,    30,    31,    47,
       1,    47,    47,    47,    37,   119,   120,   121,   122,   123,
     116,   117,   118,    14,    47,    10,    38,    10,    37,    52,
      53,     1,    44,    10,    39,    58,    38,    42,    10,    30,
      31,    43,   224,   225,    24,     1,    37,    38,    18,   127,
     128,   127,   128,    38,    40,    40,    47,    40,    14,    43,
      44,    52,    53,    40,   207,   203,   209,    58,    40,   251,
      38,     1,    40,   216,    30,    31,   224,   225,   150,   151,
      41,    37,    38,    44,    14,     1,   224,   225,     5,    39,
     156,    47,    40,   177,   178,   238,    52,    53,    14,    38,
     184,    47,    58,   251,   247,     1,    44,    37,     1,    37,
      24,     4,    47,   251,    30,    31,    40,    47,    14,    40,
      40,    37,    52,    53,    16,   203,    19,   203,    58,    22,
      43,    47,    47,    37,    30,    31,    52,    53,    30,    31,
      47,    37,    58,    40,    37,   229,   224,   225,   224,   225,
      40,    47,    47,    47,    47,     1,    52,    53,    38,     5,
      10,     7,    58,     9,    10,    11,     1,    13,     1,     4,
       1,     4,    40,   251,     5,   251,     7,    23,    42,    10,
      11,     1,    13,    37,    19,     5,    19,     7,    40,    22,
      10,    11,    23,    13,    40,     1,     9,     1,     9,     5,
       4,    47,    37,    23,    37,    38,    12,    36,     9,    40,
      39,    17,    47,    42,    47,    19,    47,     9,    22,     1,
      40,    15,    40,     5,     6,     1,    41,    47,     8,     5,
      12,    38,    41,    37,     8,    17,    12,    40,    40,    21,
      22,    17,    47,    47,    24,    25,    26,    27,    28,    29,
      24,    25,    26,    27,    28,    29,    24,    25,    26,    27,
      28,    29,     1,     1,    43,    43,     5,     5,    41,    15,
      47,    40,    47,    12,    12,    41,    40,    45,    17,    17,
       9,    40,    21,    22,    22,   101,    26,    24,    25,    26,
      27,    28,    29,    90,   154,   236,   208,   240,    20,    99,
     211,    38,    24,    25,    26,    27,    28,    29,    20,    -1,
      -1,    -1,    24,    25,    26,    27,    28,    29,    24,    25,
      26,    27,    28,    29,   110,   111,   112,   113,   114,   115,
      88
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    18,    60,    61,    40,    47,     0,     1,     6,
      62,    63,    37,    40,    47,    64,    65,     1,     5,    88,
       1,    21,    66,     1,    47,    24,    40,     5,     1,     7,
      11,    13,    23,    47,    88,    89,    90,    91,    92,    93,
      94,   102,   104,    39,    40,    47,    67,    68,     1,    22,
      78,    38,    44,     1,    14,    30,    31,    37,    47,    52,
      53,    58,    93,    95,    96,    97,    98,    99,   100,   101,
     102,    65,     1,    89,    10,    40,    43,    95,    95,    37,
      10,    40,    36,    39,    42,    43,    44,    38,    44,    24,
      40,    40,    47,    79,    80,     1,    12,    17,    81,    82,
      83,    84,    40,    47,    98,    97,    97,     1,    95,    37,
      24,    25,    26,    27,    28,    29,    16,    30,    31,     3,
      32,    33,    34,    35,    38,    10,    89,    20,     8,    38,
      95,   103,     1,    90,    95,    47,    95,    95,   103,     1,
       4,    19,    37,    47,    69,    70,    71,    73,    68,    41,
      44,    40,    40,    47,    47,    83,    62,    38,    38,    38,
      38,    96,    96,    96,    96,    96,    96,    97,    97,    97,
      98,    98,    98,    98,    98,     7,    11,    13,    23,    90,
      91,   105,    90,    10,    42,    74,     1,    47,    76,    77,
       1,    47,    72,    69,    80,    80,    37,    85,    85,    88,
      40,    95,    95,     9,     1,    75,    95,    15,    40,    41,
      10,    40,    38,    38,    44,     1,    22,    38,    69,    86,
      87,    41,    40,    40,    20,     8,    90,    43,    43,    45,
      69,    76,    69,    77,    47,    38,    40,    69,    41,    38,
      40,    47,   105,   105,    95,     1,    86,    41,    69,    87,
      40,     9,    69,   105
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 83 "compiler.y"
    { printf("The program has reached the end successfully!\n"); }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 84 "compiler.y"
    {
			iserror = 1;
			yyerrok;
			looperrordetection++;
			if(looperrordetection == 300) {
				printf("loop error detected!(%d)\n",looperrordetection);
				looperrordetection = 0;
				yyclearin;
				/*yylex();*/
				return 1;
			} /* if */
		}
    break;



/* Line 1806 of yacc.c  */
#line 1731 "compiler.tab.c"
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 311 "compiler.y"


