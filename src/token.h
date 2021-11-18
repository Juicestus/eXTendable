#ifndef XT_TOKENS
#define XT_TOKENS

#include "main.h"

enum TOKEN_TYPES {
    TOK_EOF = 0,
    TOK_ID = 256,
    TOK_INT,
    TOK_FLOAT,
    TOK_STR,

    TOK_EQUAL,
    TOK_TYPEEQUAL,
    TOK_NEQUAL,
    TOK_NTYPEEQUAL,
    TOK_LEQUAL,
    TOK_LSHIFT,
    TOK_LSHIFTEQUAL,
    TOK_GEQUAL,
    TOK_RSHIFT,
    TOK_RSHIFTUNSIGNED,
    TOK_RSHIFTEQUAL,
    TOK_PLUSEQUAL,
    TOK_MINUSEQUAL,
    TOK_PLUSPLUS,
    TOK_MINUSMINUS,
    TOK_ANDEQUAL,
    TOK_ANDAND,
    TOK_OREQUAL,
    TOK_OROR,
    TOK_XOREQUAL,
    // reserved words
#define TOK_R_LIST_START TOK_R_IF
    TOK_R_IF,
    TOK_R_ELSE,
    TOK_R_DO,
    TOK_R_WHILE,
    TOK_R_FOR,
    TOK_R_BREAK,
    TOK_R_CONTINUE,
    TOK_R_FUNCTION,
    TOK_R_RETURN,
    TOK_R_VAR,
    TOK_R_TRUE,
    TOK_R_FALSE,
    TOK_R_NULL,
    TOK_R_UNDEFINED,
    TOK_R_NEW,

	TOK_R_LIST_END /* always the last entry */
};

#endif