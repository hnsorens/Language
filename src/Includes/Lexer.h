#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATOR,
    TOKEN_STRING,
    TOKEN_EOF
} TokenType;

typedef struct
{
    TokenType type;
    char *lexeme;
    int line;
    int column;
} Token;

typedef struct
{
    const char *source;
    size_t length;
    size_t position;
    int line;
    int column;
} LexerState;

Token create_token(TokenType type, const char* lexeme, int line, int column);
Token next_token(LexerState *state);