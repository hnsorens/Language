/**
 * TODO
 * Implement Expressions
 * Implement Parameters
 * Implement StatementLists
 * Implement Loops
*/

#include "Lexer.h"

#include "Patterns/Expression.h"
#include "Patterns/FunctionDeclaration.h"
#include "Patterns/GlobalVariableDeclaration.h"

int CheckToken(Token* tokenList, int* offset, TokenType form, TokenWords type, void** ptr)
{
    if ((form != TOKEN_FORM_WORD && tokenList[*offset].type == form) || (tokenList[*offset].type == form && tokenList[*offset].token.token == type))
    {
        if (ptr)
        *ptr = &tokenList[*offset];
        *offset += 1;
        return 1;
    }
    return 0;
}

int CheckTokenWithoutOffset(Token* tokenList, int* offset, TokenType form, TokenWords type)
{
    if ((form != TOKEN_FORM_WORD && tokenList[*offset].type == form) || (tokenList[*offset].type == form && tokenList[*offset].token.token == type))
    {
        return 1;
    }
    return 0;
}

int IsDataType(Token* tokenList, int* offset)
{
    if (
        CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_VOID)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_INT)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_IDENTIFIER, 0)
    )
    {
        *offset += 1;
        return 1;
    }
    return 0;
}

int IsControlFlow(Token* tokenList, int* offset)
{
    if (
        CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_IF)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_FOR)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_WHILE)
    )
    {
        *offset += 1;
        return 1;
    }
    return 0;
}

int IsValue(Token* tokenList, int* offset)
{
    if (
        CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_IDENTIFIER, 0)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_DECIMAL, 0)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_INTEGER, 0)
    )
    {
        *offset += 1;
        return 1;
    }
    return 0;
}

int IsOperator(Token* tokenList, int* offset, TokenWords** operator)
{
    if (
        CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_PLUS)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_MINUS)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_MULTIPLY)
        || CheckTokenWithoutOffset(tokenList, offset, TOKEN_FORM_WORD, TOKEN_DIVIDE)
    )
    {
        if (operator)
        *operator = &tokenList[*offset].token.token;
        *offset += 1;
        return 1;
    }
    return 0;
}

int findPattern(Token* tokenList, int* offset, int allowedPatterns, void** structPtr)
{
    if (allowedPatterns & PATTERN_CODE)
    {
        return findPattern(tokenList, offset, PATTERN_FUNCTION_DELARATION | PATTERN_GLOBAL_VARIABLE_DECLARATION, structPtr);
    }


    if (
        CheckFunctionDeclaration(PATTERN_CHECK_PARAMETERS)
        || CheckGlobalVariableDeclaration(PATTERN_CHECK_PARAMETERS)
        || CheckExpression(PATTERN_CHECK_PARAMETERS)
        || (allowedPatterns & PATTERN_STATEMENT_LIST || allowedPatterns & PATTERN_PARAMETER_LIST || allowedPatterns & PATTERN_DONE) //still need a check function for these
    )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int Lex(TokenList* tokenList)
{
    int tokenOffset = 0;
    Token* tokens = tokenList->tokens;

    Code* code = (Code*)malloc(sizeof(Code));
    return findPattern(tokens, &tokenOffset, PATTERN_CODE, &code);
    
}