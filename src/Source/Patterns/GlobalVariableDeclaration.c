#include "Patterns/GlobalVariableDeclaration.h"

int CheckGlobalVariableDeclaration(Token* tokenList, int* offset, int allowedPatterns, void** structPtr)
{
    if (allowedPatterns & PATTERN_GLOBAL_VARIABLE_DECLARATION)
    {
        Token* variableName;
        Expression* assignment;

        // Declaration with initialization
        int tokenOffset = *offset;
        if (
            IsDataType(tokenList, &tokenOffset)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_IDENTIFIER, 0, &variableName)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_EQUAL, NULL)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &assignment)
        )
        {
            // Create the data structure for the declaration
            GlobalVariableDelaration* declaration = (GlobalVariableDelaration*)malloc(sizeof(GlobalVariableDelaration));
            declaration->pAssignment = assignment;
            declaration->pVariableName = &variableName->token.hashedString;
            *structPtr = declaration;

            printf("Declare\n");
            *offset = tokenOffset;
            return findPattern(tokenList, offset, PATTERN_FUNCTION_DELARATION | PATTERN_GLOBAL_VARIABLE_DECLARATION | PATTERN_DONE, &declaration->pNext);
        }

        // Declaration without initialization
        tokenOffset = *offset;
        if (
            IsDataType(tokenList, &tokenOffset)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_IDENTIFIER, 0, &variableName)
        )
        {
            // Create the data structure for the declaration
            GlobalVariableDelaration* declaration = (GlobalVariableDelaration*)malloc(sizeof(GlobalVariableDelaration));
            declaration->pAssignment = NULL;
            declaration->pVariableName = &variableName->token.hashedString;
            *structPtr = declaration;

            printf("Declare\n");
            *offset = tokenOffset;
            return findPattern(tokenList, offset, PATTERN_FUNCTION_DELARATION | PATTERN_GLOBAL_VARIABLE_DECLARATION | PATTERN_DONE, &declaration->pNext);
        }
    }
    return 0;
}