#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "Tokenizer.h"

#define PATTERN_CHECK_PARAMETERS tokenList, offset, allowedPatterns, structPtr

typedef enum
{
    PATTERN_NOTHING = 1,
    PATTERN_DONE = 2,
    PATTERN_CODE = 4,
    PATTERN_FUNCTION_DELARATION = 8,
    PATTERN_GLOBAL_VARIABLE_DECLARATION = 16,
    PATTERN_STATEMENT_LIST = 32,
    PATTERN_PARAMETER_LIST = 64,
    PATTERN_EXPRESSION = 128,
} PatternTypes;

typedef struct
{
    union
    {
        unsigned long hashedIdentifier;
        double f;
        int i;
    } value;
    void* first;
    void* second;
    TokenWords operator;
    enum
    {
        EXPRESSION_OPERATION,
        EXPRESSION_VALUE_INT,
        EXPRESSION_VALUE_DECIMAL,
        EXPRESSION_EXPRESSION,
        EXPRESSION_VALUE_IDENTIFIER
    } type;
} Expression;

typedef struct
{

} StatementList;

typedef struct
{

} ParameterList;

typedef struct
{
    unsigned long* pFunctionName;
    StatementList* pStatementList;
    ParameterList* pParameterList;
    void* pNext;
} FunctionDelaration;

typedef struct
{
    unsigned long* pVariableName;
    Expression* pAssignment;
    void* pNext;
} GlobalVariableDelaration;

typedef union
{
    FunctionDelaration* functionDeclaration;
    GlobalVariableDelaration* globalVariableDeclaration;
} CodePattern;

typedef struct
{
    CodePattern pattern;
    PatternTypes type;
} Code;

int CheckToken(Token* tokenList, int* offset, TokenType form, TokenWords type, void** ptr);
int CheckTokenWithoutOffset(Token* tokenList, int* offset, TokenType form, TokenWords type);
int IsDataType(Token* tokenList, int* offset);
int IsControlFlow(Token* tokenList, int* offset);
int IsValue(Token* tokenList, int* offset);
int IsOperator(Token* tokenList, int* offset, TokenWords** operator);
int findPattern(Token* tokenList, int* offset, int allowedPatterns, void** structPtr);

int Lex(TokenList* tokenList);