#include "Patterns/FunctionDeclaration.h"

int CheckFunctionDeclaration(Token* tokenList, int* offset, int allowedPatterns, void** structPtr)
{
    if (allowedPatterns & PATTERN_FUNCTION_DELARATION)
    {
        // DataType Identifier(FunctionParameters){FunctionInterior}
        ParameterList* parameterList;
        StatementList* statementList;
        Token* functionNameToken;

        int tokenOffset = *offset;
        if (IsDataType(tokenList, &tokenOffset)
        && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_IDENTIFIER, 0, &functionNameToken)
        && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_LPARENTHESIS, NULL)
        && findPattern(tokenList, &tokenOffset, PATTERN_PARAMETER_LIST | PATTERN_DONE, &parameterList)
        && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_RPARENTHESIS, NULL)
        && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_LCBRACKET, NULL)
        && findPattern(tokenList, &tokenOffset, PATTERN_STATEMENT_LIST | PATTERN_DONE, &statementList)
        && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_RCBRACKET, NULL)
        )
        {
            // Create the data structure for the function
            FunctionDelaration* functionDeclaration = (FunctionDelaration*)malloc(sizeof(FunctionDelaration));
            functionDeclaration->pParameterList = parameterList;
            functionDeclaration->pStatementList = statementList;
            functionDeclaration->pFunctionName = &functionNameToken->token.hashedString;
            *structPtr = functionDeclaration;

            printf("Function\n");
            *offset = tokenOffset;
            return findPattern(tokenList, offset, PATTERN_FUNCTION_DELARATION | PATTERN_GLOBAL_VARIABLE_DECLARATION | PATTERN_DONE, &functionDeclaration->pNext);
        }
    }
    return 0;
}