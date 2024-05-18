/**
 * TODO
 * Implement Expressions
 * Implement Parameters
 * Implement StatementLists
 * Implement Loops
*/

#include "Lexer.h"

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
    if (allowedPatterns & PATTERN_EXPRESSION)
    {
        Expression* first;
        TokenWords* operator;
        Expression* second;
        Token* value;

        // (expr) op (expr)
        int tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_LPARENTHESIS, 0)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &first)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_RPARENTHESIS, 0)
            && IsOperator(tokenList, &tokenOffset, &operator)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_LPARENTHESIS, 0)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &second)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_RPARENTHESIS, 0)
        )
        {
            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->first = first;
            expression->operator = *operator;
            expression->second = second;
            *structPtr = expression;

            printf("Expression (expr) op (expr)\n");
            *offset = tokenOffset;

            return 1;
        }

        // (expr)
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_LPARENTHESIS, 0)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &first)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_RPARENTHESIS, 0)
        )
        {
            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->first = first;
            expression->type = EXPRESSION_EXPRESSION;
            *structPtr = expression;

            printf("Expression (expr)\n");
            *offset = tokenOffset;

            return 1;
        }

        // int op (expr)
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_INTEGER, 0, &value)
            && IsOperator(tokenList, &tokenOffset, &operator)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &second)
        )
        {
            Expression* valueExpr = (Expression*)malloc(sizeof(Expression));
            valueExpr->type = EXPRESSION_VALUE_INT;
            valueExpr->value.i = value->token.i;

            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->first = valueExpr;
            expression->operator = *operator;
            expression->second = second;
            *structPtr = expression;

            printf("Expression int op (expr)\n");
            *offset = tokenOffset;

            return 1;
        }

        // decimal op (expr)
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_DECIMAL, 0, &value)
            && IsOperator(tokenList, &tokenOffset, &operator)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &second)
        )
        {
            Expression* valueExpr = (Expression*)malloc(sizeof(Expression));
            valueExpr->type = EXPRESSION_VALUE_DECIMAL;
            valueExpr->value.f = value->token.f;

            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->first = valueExpr;
            expression->operator = *operator;
            expression->second = second;
            *structPtr = expression;

            printf("Expression decimal op (expr)\n");
            *offset = tokenOffset;

            return 1;
        }

        // identifier op (expr)
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_IDENTIFIER, 0, &value)
            && IsOperator(tokenList, &tokenOffset, &operator)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &second)
        )
        {
            Expression* valueExpr = (Expression*)malloc(sizeof(Expression));
            valueExpr->type = EXPRESSION_VALUE_IDENTIFIER;
            valueExpr->value.hashedIdentifier = value->token.hashedString;

            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->first = valueExpr;
            expression->operator = *operator;
            expression->second = second;
            *structPtr = expression;

            printf("Expression identifier op (expr)\n");
            *offset = tokenOffset;

            return 1;
        }

        // (expr) op int
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_LPARENTHESIS, 0)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &first)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_RPARENTHESIS, 0)
            && IsOperator(tokenList, &tokenOffset, &operator)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_INTEGER, 0, &value)
        )
        {
            Expression* valueExpr = (Expression*)malloc(sizeof(Expression));
            valueExpr->type = EXPRESSION_VALUE_INT;
            valueExpr->value.i = value->token.i;

            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->first = first;
            expression->operator = *operator;
            expression->second = valueExpr;
            *structPtr = expression;

            printf("Expression (expr) op int\n");
            *offset = tokenOffset;

            return 1;
        }

        // (expr) op decimal
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_LPARENTHESIS, 0)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &first)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_RPARENTHESIS, 0)
            && IsOperator(tokenList, &tokenOffset, &operator)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_DECIMAL, 0, &value)
        )
        {
            Expression* valueExpr = (Expression*)malloc(sizeof(Expression));
            valueExpr->type = EXPRESSION_VALUE_DECIMAL;
            valueExpr->value.f = value->token.f;

            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->first = first;
            expression->operator = *operator;
            expression->second = valueExpr;
            *structPtr = expression;

            printf("Expression (expr) op decimal\n");
            *offset = tokenOffset;

            return 1;
        }

        // (expr) op identifier
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_LPARENTHESIS, 0)
            && findPattern(tokenList, &tokenOffset, PATTERN_EXPRESSION, &first)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_WORD, TOKEN_RPARENTHESIS, 0)
            && IsOperator(tokenList, &tokenOffset, &operator)
            && CheckToken(tokenList, &tokenOffset, TOKEN_FORM_IDENTIFIER, 0, &value)
        )
        {
            Expression* valueExpr = (Expression*)malloc(sizeof(Expression));
            valueExpr->type = EXPRESSION_VALUE_IDENTIFIER;
            valueExpr->value.hashedIdentifier = value->token.hashedString;

            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->first = first;
            expression->operator = *operator;
            expression->second = valueExpr;
            *structPtr = expression;

            printf("Expression (expr) op identifier\n");
            *offset = tokenOffset;

            return 1;
        }
        
        // integer
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_INTEGER, 0, value)
        )
        {
            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->type = EXPRESSION_VALUE_INT;
            expression->value.i = value->token.i;

            *structPtr = expression;

            printf("Expression integer\n");
            *offset = tokenOffset;

            return 1;
        }

        // Decimal
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_DECIMAL, 0, value)
        )
        {
            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->type = EXPRESSION_VALUE_DECIMAL;
            expression->value.f = value->token.f;
            
            *structPtr = expression;

            printf("Expression decimal\n");
            *offset = tokenOffset;

            return 1;
        }

        // Identifier
        tokenOffset = *offset;
        if (
            CheckToken(tokenList, &tokenOffset, TOKEN_FORM_IDENTIFIER, 0, value)
        )
        {
            Expression* expression = (Expression*)malloc(sizeof(Expression));
            expression->type = EXPRESSION_VALUE_IDENTIFIER;
            expression->value.hashedIdentifier = value->token.hashedString;
            
            *structPtr = expression;

            printf("Expression identifier\n");
            *offset = tokenOffset;

            return 1;
        }

        return 0;
    }
    if (allowedPatterns & PATTERN_STATEMENT_LIST)
    {
        return 1;
    }
    if (allowedPatterns & PATTERN_PARAMETER_LIST)
    {
        return 1;
    }
    if (allowedPatterns & PATTERN_DONE)
    {
        return 1;
    }

    return 0;
}

int Lex(TokenList* tokenList)
{
    int tokenOffset = 0;
    Token* tokens = tokenList->tokens;

    Code* code = (Code*)malloc(sizeof(Code));
    return findPattern(tokens, &tokenOffset, PATTERN_CODE, &code);
    
}