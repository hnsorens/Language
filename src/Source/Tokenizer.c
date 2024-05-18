#include "Tokenizer.h"

unsigned long hash(const char *buffer, size_t offset, size_t length)
{
    unsigned long hash = 5381; // Initial hash value

    for (size_t i = 0; i < length; i++) {
        hash = ((hash << 5) + hash) + buffer[i + offset]; /* hash * 33 + str[i] */
    }

    return hash;
}

void addToken(TokenList* tokenList, Token token)
{
    if (tokenList->capacity == tokenList->tokenCount)
    {
        tokenList->tokens = realloc(tokenList->tokens, tokenList->capacity+=256);
    }

    tokenList->tokens[tokenList->tokenCount++] = token;
}

int CheckForTokenWord(TokenList* tokenList, const char* buffer, int* offset, const char* tokenString, int tokenLength, TokenWords tokenWord)
{
    for (int i = 0; i < tokenLength; i++)
    {
        if (buffer[*offset+i] != tokenString[i])
        {
            return 0;
        }
    }
    *offset += tokenLength;

    Token token = {(TokenForms)tokenWord, TOKEN_FORM_WORD};
    addToken(tokenList, token);
    printf("Added token (word): %s\n", tokenString);
    return 1;
}

int SkipWord(TokenList* tokenList, const char* buffer, int* offset, const char* tokenString, int tokenLength)
{
    for (int i = 0; i < tokenLength; i++)
    {
        if (buffer[*offset+i] != tokenString[i])
        {
            return 0;
        }
    }
    *offset += tokenLength;
    return 1;
}

int isAlphabet(char ch)
{
    return isalpha(ch) && !isspace(ch) && !ispunct(ch);
}

int isNumber(char ch)
{
    return isdigit(ch);
}

int ToInteger(const char* buffer, int offset, int length)
{
    
    int number = 0;
    for (int i = 0; i < length; i++)
    {
        number += (buffer[i+offset] - '0') * pow(10, length-i-1);
    }
    return number;
}

double ToDecimal(const char* buffer, int offset, int length, int placesToDecimal)
{
    double number;
    for (int i = 0; i < placesToDecimal; i++)
    {
        number += (buffer[i+offset] - '0') * pow(10, placesToDecimal-i-1);
    }

    for (int i = 0; i < length-placesToDecimal-1; i++)
    {
        number += (buffer[i+offset+placesToDecimal+1] - '0') * pow(10, (i+1)*-1);
    }
    return number;
}

int CheckForWord(TokenList* tokenList, const char* buffer, int* offset)
{
    int initialOffset = *offset;
    while (isAlphabet(buffer[*offset]))
    {
        *offset+=1;
    }

    if (*offset == initialOffset) return 0;
    Token token = {(TokenForms)hash(buffer, initialOffset, *offset-initialOffset), TOKEN_FORM_IDENTIFIER};
    addToken(tokenList, token);
    printf("Added token (Identifier): %li\n", hash(buffer, initialOffset, *offset-initialOffset));
    return 1;
}

int CheckForNumber(TokenList* tokenList, const char* buffer, int* offset)
{
    int initialOffset = *offset;
    while (isdigit(buffer[*offset]))
    {
        *offset+=1;
    }
    int palcesToDecimal = *offset-initialOffset;
    if (*offset == initialOffset) return 0;
    if (buffer[*offset] == '.')
    {
        *offset += 1;
        while (isdigit(buffer[*offset]))
        {
            *offset+=1;
        }
        Token token = {(TokenForms)ToDecimal(buffer, initialOffset, *offset-initialOffset, palcesToDecimal), TOKEN_FORM_DECIMAL};
        addToken(tokenList, token);
        printf("Added token (Decimal): %f\n", ToDecimal(buffer, initialOffset, *offset-initialOffset, palcesToDecimal));
    }
    else
    {
        Token token = {(TokenForms)ToInteger(buffer, initialOffset, *offset-initialOffset), TOKEN_FORM_INTEGER};
        addToken(tokenList, token);
        printf("Added token (integer): %i\n", ToInteger(buffer, initialOffset, *offset-initialOffset));
    }

    return 1;
}


void tokenize(TokenList* tokenList, const char* buffer)
{
    int charAt = 0;

    while (buffer[charAt] != '\0')
    {
        // Comment
        if (buffer[charAt] == '/' && buffer[charAt+1] == '/')
        {
            while (buffer[charAt] != '\n')
            {
                if (buffer[charAt] == '\0')
                {
                    return;
                }
                charAt++;
            }
        }
        else if (!(
            SkipWord(tokenList, buffer, &charAt, " ", 1)
            || SkipWord(tokenList, buffer, &charAt, "\n", 1)
            || SkipWord(tokenList, buffer, &charAt, ";\n", 2)
            || CheckForTokenWord(tokenList, buffer, &charAt, "void", 4, TOKEN_VOID)
            || CheckForTokenWord(tokenList, buffer, &charAt, "while", 5, TOKEN_WHILE)
            || CheckForTokenWord(tokenList, buffer, &charAt, "for", 3, TOKEN_FOR)
            || CheckForTokenWord(tokenList, buffer, &charAt, "if", 2, TOKEN_IF)

            || CheckForTokenWord(tokenList, buffer, &charAt, "(", 1, TOKEN_LPARENTHESIS)
            || CheckForTokenWord(tokenList, buffer, &charAt, ")", 1, TOKEN_RPARENTHESIS)
            || CheckForTokenWord(tokenList, buffer, &charAt, "{", 1, TOKEN_LCBRACKET)
            || CheckForTokenWord(tokenList, buffer, &charAt, "}", 1, TOKEN_RCBRACKET)
            || CheckForTokenWord(tokenList, buffer, &charAt, "[", 1, TOKEN_LSBRACKET)
            || CheckForTokenWord(tokenList, buffer, &charAt, "]", 1, TOKEN_RSBRACKET)

            || CheckForTokenWord(tokenList, buffer, &charAt, "+", 1, TOKEN_PLUS)
            || CheckForTokenWord(tokenList, buffer, &charAt, "-", 1, TOKEN_MINUS)
            || CheckForTokenWord(tokenList, buffer, &charAt, "*", 1, TOKEN_MULTIPLY)
            || CheckForTokenWord(tokenList, buffer, &charAt, "/", 1, TOKEN_DIVIDE)
            || CheckForTokenWord(tokenList, buffer, &charAt, "=", 1, TOKEN_EQUAL)

            || CheckForTokenWord(tokenList, buffer, &charAt, "int", 3, TOKEN_INT)

            || CheckForWord(tokenList, buffer, &charAt)
            || CheckForNumber(tokenList, buffer, &charAt)
        ))
        {
            perror("compiler error!");
        }
    }
}