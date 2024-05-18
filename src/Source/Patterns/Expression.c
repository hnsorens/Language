#include "Patterns/Expression.h"

int CheckExpression(Token* tokenList, int* offset, int allowedPatterns, void** structPtr)
{
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
    }
    return 0;
}