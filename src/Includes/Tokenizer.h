#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

typedef enum {
    TOKEN_EOF=0,

    TOKEN_VOID,
    TOKEN_WHILE,
    TOKEN_IF,
    TOKEN_FOR,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_EQUAL,

    TOKEN_INT,

    TOKEN_LPARENTHESIS,
    TOKEN_RPARENTHESIS,
    TOKEN_LCBRACKET,
    TOKEN_RCBRACKET,
    TOKEN_LSBRACKET,
    TOKEN_RSBRACKET,
} TokenWords;

typedef enum
{
    TOKEN_FORM_WORD,
    TOKEN_FORM_IDENTIFIER,
    TOKEN_FORM_DECIMAL,
    TOKEN_FORM_INTEGER
} TokenType;

typedef union
{
    TokenWords token;
    unsigned long hashedString;
    double f;
    int i;
} TokenForms;

typedef struct
{
    TokenForms token;
    TokenType type;
} Token;

typedef struct
{
    int capacity;
    int tokenCount;
    Token* tokens;
} TokenList;

void tokenize(TokenList* tokenList, const char* buffer);