#include "Parser.h"

#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BOLD        "\033[1m"
#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"

#define TITLE BLUE
#define TYPE GREEN
#define VALUE MAGENTA


ASTNode* create_ast_node(ASTNodeType type)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

Token current_token(ParserState *state)
{
    return state->tokens[state->position];
}

void consume_token(ParserState *state)
{
    state->position++;
}

ASTNode* parse_argument(ParserState *state)
{
    
    ASTNode *node = create_ast_node(AST_ARGUMENT);
    node->data.argument.argument = parse_primary(state);
    return node;
}

ASTNode* parse_compound_argument(ParserState *state)
{
    if (current_token(state).lexeme[0] == ')') return NULL;

    ASTNode *compount_argument = create_ast_node(AST_COMPOUND_ARGUMENT);

    compount_argument->data.compound_argument.argument_count = 0;

    while (current_token(state).lexeme[0] != ')')
    {
        
        if (compount_argument->data.compound_argument.argument_count == 0)
        {
            compount_argument->data.compound_argument.argument_count++;
            compount_argument->data.compound_argument.arguments = (ASTNode **)malloc(sizeof(ASTNode*));
        }
        else
        {
            consume_token(state);
            compount_argument->data.compound_argument.argument_count++;
            compount_argument->data.compound_argument.arguments = (ASTNode **)realloc(compount_argument->data.compound_argument.arguments, sizeof(ASTNode*) * compount_argument->data.compound_argument.argument_count);
        }
        compount_argument->data.compound_argument.arguments[compount_argument->data.compound_argument.argument_count-1] = parse_argument(state);
    }

    return compount_argument;
}

ASTNode* parse_primary(ParserState *state)
{
    Token token = current_token(state);

    if (token.type == TOKEN_NUMBER)
    {
        consume_token(state);
        ASTNode *node = create_ast_node(AST_NUMBER);
        node->data.number_value = atoi(token.lexeme);
        return node;
    }

    

    if (token.type == TOKEN_IDENTIFIER) 
    {
        consume_token(state);

        if (current_token(state).lexeme[0] == '(')
        {
            consume_token(state);
            ASTNode *compound_argument = parse_compound_argument(state);
            if (current_token(state).lexeme[0] != ')')
            {
                fprintf(stderr, "Error: expected ')'");
            }
            consume_token(state);

            ASTNode *node = create_ast_node(AST_FUNCTION_CALL);
            node->data.function_call.argumentList = compound_argument;
            node->data.function_call.name = strdup(token.lexeme);

            return node;
        }
        else
        {
            ASTNode *node = create_ast_node(AST_IDENTIFIER);
            node->data.identifier_name = strdup(token.lexeme);
            return node;
        }
    }

    if (token.type == TOKEN_STRING) 
    {
        consume_token(state);
        ASTNode *node = create_ast_node(AST_STRING);
        node->data.string_value = strdup(token.lexeme);
        return node;
    }

    if (token.type == TOKEN_PUNCTUATOR && token.lexeme[0] == '(') {
        consume_token(state);
        ASTNode *node = parse_expression(state);
        if (current_token(state).lexeme[0] != ')') {
            fprintf(stderr, "Error: expected ')'\n");
            exit(1);
        }
        consume_token(state);
        return node;
    }

    fprintf(stderr, "Error: unexpected token '%s'\n", token.lexeme);
    exit(1);
    return NULL;
}

ASTNode* parse_expression(ParserState *state)
{
    ASTNode *left = parse_primary(state);

    if (left->type == AST_IDENTIFIER && current_token(state).lexeme[0] == '=')
    {
        // Assignment
        consume_token(state);
        ASTNode *right = parse_expression(state);
        ASTNode *node = create_ast_node(AST_ASSIGNMENT);
        node->data.assignment.left = left;
        node->data.assignment.right = right;

        return node;
    }

    Token token = current_token(state);
    while (token.type == TOKEN_OPERATOR)
    {
        consume_token(state);
        ASTNode *right = parse_primary(state);

        ASTNode *node = create_ast_node(AST_BINARY_OP);
        node->data.binary_op.op = token.lexeme;
        node->data.binary_op.left = left;
        node->data.binary_op.right = right;

        left = node;
        token = current_token(state);
    }

    return left;
}

ASTNode* parse_compound_statement(ParserState *state)
{
    ASTNode *compound_statement = create_ast_node(AST_COMPOUND_STATEMENT);

    while ((current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '}') && current_token(state).type != TOKEN_EOF)
    {
        if (compound_statement->data.compound_statement.statement_count == 0)
        {
            compound_statement->data.compound_statement.statement_count++;
            compound_statement->data.compound_statement.statements = (ASTNode **)malloc(sizeof(ASTNode*));
        }
        else
        {
            compound_statement->data.compound_statement.statement_count++;
            compound_statement->data.compound_statement.statements = (ASTNode **)realloc(compound_statement->data.compound_statement.statements, sizeof(ASTNode*) * compound_statement->data.compound_statement.statement_count);
        }

        compound_statement->data.compound_statement.statements[compound_statement->data.compound_statement.statement_count-1] = parse_statement(state);
    }

    return compound_statement;
}

ASTNode* parse_statement(ParserState *state)
{
    Token token = current_token(state);

    if (token.type == TOKEN_KEYWORD)
    {
        if (strcmp(token.lexeme, "if") == 0)
        {
            consume_token(state);
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '(')
            {
                fprintf(stderr, "Error: expected '('\n");
                exit(1);
            }
            consume_token(state);
            ASTNode *condition = parse_expression(state);
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != ')')
            {
                fprintf(stderr, "Error: expected ')'\n");
                exit(1);
            }
            consume_token(state);
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '{')
            {
                fprintf(stderr, "Error: expected '{'\n");
                exit(1);
            }
            consume_token(state);
            ASTNode *then_branch = parse_compound_statement(state);
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '}')
            {
                fprintf(stderr, "Error: expected '}'\n");
                exit(1);
            }
            consume_token(state);
            ASTNode *else_branch = NULL;
            if (current_token(state).type == TOKEN_KEYWORD && strcmp(current_token(state).lexeme, "else") == 0)
            {
                consume_token(state);
                if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '{')
                {
                    fprintf(stderr, "Error: expected '{'\n");
                    exit(1);
                }
                consume_token(state);
                else_branch = parse_compound_statement(state);
                if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '}')
                {
                    fprintf(stderr, "Error: expected '}'\n");
                    exit(1);
                }
                consume_token(state);
            }
            ASTNode *node = create_ast_node(AST_IF_STATEMENT);
            node->data.if_statement.condition = condition;
            node->data.if_statement.then_branch = then_branch;
            node->data.if_statement.else_branch = else_branch;
            return node;
        }

        if (strcmp(token.lexeme, "while") == 0)
        {
            consume_token(state);
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '(')
            {
                fprintf(stderr, "Error: expected '('\n");
                exit(1);
            }
            consume_token(state);
            ASTNode *condition = parse_expression(state);
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != ')')
            {
                fprintf(stderr, "Error: expected ')'\n");
                exit(1);
            }
            consume_token(state);
            ASTNode *body = parse_statement(state);
            ASTNode *node = create_ast_node(AST_WHILE_STATEMENT);
            node->data.while_statement.condition = condition;
            node->data.while_statement.body = body;
            return node;
        }

        if (strcmp(token.lexeme, "return") == 0)
        {
            consume_token(state);
            ASTNode *expression = NULL;
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != ';')
            {
                expression = parse_expression(state);
            }
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != ';')
            {
                fprintf(stderr, "Error: expected ';'\n");
                exit(1);
            }
            consume_token(state);
            ASTNode *node = create_ast_node(AST_RETURN_STATEMENT);
            node->data.return_statement.expression = expression;
            return node;
        }
    }

    if (state->tokens[state->position+1].type == TOKEN_IDENTIFIER)
    {
        // Declaration
        consume_token(state);
        const char* type = strdup(token.lexeme);
        if (current_token(state).type != TOKEN_IDENTIFIER)
        {
            fprintf(stderr, "Error: expected identifier\n");
            exit(1);
        }
        const char* name = strdup(current_token(state).lexeme);
        consume_token(state);
        if (current_token(state).type == TOKEN_PUNCTUATOR && current_token(state).lexeme[0] == ';')
        {
            ASTNode *node = create_ast_node(AST_VARIABLE_DECLARATION);
            node->data.variable.type = type;
            node->data.variable.name = name;
            node->data.variable.expression = NULL;

            return node;
        }
        else if (current_token(state).type == TOKEN_OPERATOR && current_token(state).lexeme[0] == '=')
        {
            consume_token(state);
            ASTNode *node = create_ast_node(AST_VARIABLE_DECLARATION);
            node->data.variable.type = type;
            node->data.variable.name = name;
            node->data.variable.expression = parse_expression(state);
            if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != ';')
            {
                fprintf(stderr, "Error: expected ';'");
                exit(1);
            }
            consume_token(state);

            return node;
        }

        fprintf(stderr, "Error: expected ';'");
        exit(1);
    }


    ASTNode *expression = parse_expression(state);
    if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != ';') {
        fprintf(stderr, "Error: expe1cted ';'\n");
        exit(1);
    }
    consume_token(state);
    ASTNode *node = create_ast_node(AST_EXPRESSION_STATEMENT);
    node->data.expression_statement.expression = expression;
    return node;
}

int IsDataType(const char* text)
{
    return strcmp(text, "int") == 0
        || strcmp(text, "bool") == 0;
}

ASTNode* parse_parameter(ParserState *state)
{
    ASTNode *parameter = create_ast_node(AST_PARAMETER);

    if ((current_token(state).type != TOKEN_KEYWORD || !IsDataType(current_token(state).lexeme)) && (current_token(state).type != TOKEN_IDENTIFIER))
    {
        fprintf(stderr, "Error: expected type\n");
        exit(1);
    }
    const char* type = current_token(state).lexeme;
    consume_token(state);

    if (current_token(state).type != TOKEN_IDENTIFIER)
    {
        fprintf(stderr, "Error: expected identifier\n");
        exit(1);
    }
    const char* name = current_token(state).lexeme;
    consume_token(state);

    parameter->data.parameter.type = type;
    parameter->data.parameter.name = name;

    return parameter;
}

ASTNode* parse_parameter_list(ParserState *state)
{
    ASTNode *parameter_list = create_ast_node(AST_PARAMETER_LIST);

    if (current_token(state).type == TOKEN_PUNCTUATOR && current_token(state).lexeme[0] == ')')
    {
        return NULL;
    }

    parameter_list->data.parameter_list.parameter_count = 1;
    parameter_list->data.parameter_list.parameters = (ASTNode **)malloc(sizeof(ASTNode*) * parameter_list->data.parameter_list.parameter_count);
    parameter_list->data.parameter_list.parameters[parameter_list->data.parameter_list.parameter_count-1] = parse_parameter(state);

    while (current_token(state).type == TOKEN_PUNCTUATOR && current_token(state).lexeme[0] == ',')
    {
        consume_token(state);
        parameter_list->data.parameter_list.parameter_count++;
        parameter_list->data.parameter_list.parameters = (ASTNode **)realloc(parameter_list->data.parameter_list.parameters, sizeof(ASTNode*) * parameter_list->data.parameter_list.parameter_count);
        parameter_list->data.parameter_list.parameters[parameter_list->data.parameter_list.parameter_count-1] = parse_parameter(state);
    }

    return parameter_list;
}

ASTNode* parse_global_construct(ParserState *state)
{
    if ((current_token(state).type == TOKEN_KEYWORD && IsDataType(current_token(state).lexeme)) || current_token(state).type == TOKEN_IDENTIFIER)
    {
        const char *type = current_token(state).lexeme;
        consume_token(state);

        if (current_token(state).type == TOKEN_IDENTIFIER)
        {
            const char *name = current_token(state).lexeme;
            consume_token(state);

            if (current_token(state).type == TOKEN_PUNCTUATOR)
            {
                if (current_token(state).lexeme[0] == '(')
                {
                    
                    consume_token(state);
                    ASTNode *parameters_list = parse_parameter_list(state);
                    if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != ')')
                    {
                        fprintf(stderr, "Error: expected ')'\n");
                        exit(1);
                    }
                    consume_token(state);
                    if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '{')
                    {
                        fprintf(stderr, "Error: expected '{'\n");
                        exit(1);
                    }
                    consume_token(state);
                    ASTNode *statements = parse_compound_statement(state);
                    if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != '}')
                    {
                        fprintf(stderr, "Error: expected '}'\n");
                        exit(1);
                    }
                    consume_token(state);
                    ASTNode *function = create_ast_node(AST_FUNCTION_DECLARATION);
                    function->data.function_global_construct.name = name;
                    function->data.function_global_construct.parameters = parameters_list;
                    function->data.function_global_construct.return_type = type;
                    function->data.function_global_construct.statements = statements;
                    return function;
                }
                else if (current_token(state).lexeme[0] == ';')
                {
                    consume_token(state);

                    ASTNode *variable_declaration = create_ast_node(AST_VARIABLE_DECLARATION);
                    variable_declaration->data.variable.name = name;
                    variable_declaration->data.variable.type = type;
                    variable_declaration->data.variable.expression = NULL;
                    return variable_declaration;
                }
            }

            if (current_token(state).type == TOKEN_OPERATOR && strcmp(current_token(state).lexeme, "=") == 0)
            {
                consume_token(state);
                ASTNode *expression = parse_expression(state);
                if (current_token(state).type != TOKEN_PUNCTUATOR || current_token(state).lexeme[0] != ';')
                {
                    fprintf(stderr, "Error: expected ';'\n");
                    exit(1);
                }
                consume_token(state);
                ASTNode *variable_declaration = create_ast_node(AST_VARIABLE_DECLARATION);
                variable_declaration->data.variable.name = name;
                variable_declaration->data.variable.type = type;
                variable_declaration->data.variable.expression = expression;
                return variable_declaration;
            }
        }
    }
}

ASTNode* parse_compound_global_construct(ParserState *state)
{
    
    ASTNode *compound_global_construct = create_ast_node(AST_COMPOUND_GLOBAL_CONSTRUCT);

    compound_global_construct->data.compound_global_construct.global_construct_count = 0;
    while (current_token(state).type != TOKEN_EOF)
    {

        if (compound_global_construct->data.compound_global_construct.global_construct_count == 0)
        {
            compound_global_construct->data.compound_global_construct.global_construct_count++;
            compound_global_construct->data.compound_global_construct.global_constructs = (ASTNode **)malloc(sizeof(ASTNode*));
        }
        else
        {
            compound_global_construct->data.compound_global_construct.global_construct_count++;
            compound_global_construct->data.compound_global_construct.global_constructs = (ASTNode **)realloc(compound_global_construct->data.compound_global_construct.global_constructs, sizeof(ASTNode*) * compound_global_construct->data.compound_global_construct.global_construct_count);
        }
        
        compound_global_construct->data.compound_global_construct.global_constructs[compound_global_construct->data.compound_global_construct.global_construct_count-1] = parse_global_construct(state);

    }
    return compound_global_construct;
}

void print_ast(ASTNode *node, int depth) {
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    switch (node->type) {
        case AST_NUMBER:
            printf(TYPE"Number: "VALUE"%d"RESET"\n", node->data.number_value);
            break;
        case AST_IDENTIFIER:
            printf(TYPE"Identifier: "VALUE"%s"RESET"\n", node->data.identifier_name);
            break;
        case AST_STRING:
            printf(TYPE"String: "VALUE"%s"RESET"\n", node->data.string_value);
            break;
        case AST_BINARY_OP:
            printf(TITLE"Binary Op: "VALUE"%s"RESET"\n", node->data.binary_op.op);
            print_ast(node->data.binary_op.left, depth + 1);
            print_ast(node->data.binary_op.right, depth + 1);
            break;
        case AST_UNARY_OP:
            printf(TYPE"Unary Op: "VALUE"%s"RESET"\n", node->data.unary_op.op);
            print_ast(node->data.unary_op.operand, depth + 1);
            break;
        case AST_ASSIGNMENT:
            printf(TITLE"Assignment:"RESET"\n");
            print_ast(node->data.assignment.left, depth + 1);
            print_ast(node->data.assignment.right, depth + 1);
            break;
        case AST_COMPOUND_STATEMENT:
            printf(TITLE"Compound Statement:"RESET"\n");
            for (size_t i = 0; i < node->data.compound_statement.statement_count; i++) {
                print_ast(node->data.compound_statement.statements[i], depth + 1);
            }
            break;
        case AST_EXPRESSION_STATEMENT:
            printf(TITLE"Expression Statement:"RESET"\n");
            print_ast(node->data.expression_statement.expression, depth + 1);
            break;
        case AST_IF_STATEMENT:
            printf(TITLE"If Statement:"RESET"\n");
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TITLE"  Condition:"RESET"\n");
            print_ast(node->data.if_statement.condition, depth + 2);
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TITLE"  Then:"RESET"\n");
            print_ast(node->data.if_statement.then_branch, depth + 2);
            if (node->data.if_statement.else_branch) {
                for (int i = 0; i < depth; i++) {
                    printf("  ");
                }
                printf(TITLE"  Else:"RESET"\n");
                print_ast(node->data.if_statement.else_branch, depth + 2);
            }
            break;
        case AST_WHILE_STATEMENT:
            printf(TITLE"While Statement:"RESET"\n");
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TITLE"  Condition:"RESET"\n");
            print_ast(node->data.while_statement.condition, depth + 2);
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TITLE"  Body:"RESET"\n");
            print_ast(node->data.while_statement.body, depth + 2);
            break;
        case AST_RETURN_STATEMENT:
            printf(TITLE"Return Statement:"RESET"\n");
            if (node->data.return_statement.expression) {
                print_ast(node->data.return_statement.expression, depth + 1);
            }
            break;
        case AST_FUNCTION_DECLARATION:
            printf(TITLE"Function Declaration:"RESET"\n");
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TYPE"  Return Type: "VALUE"%s"RESET"\n", node->data.function_global_construct.return_type);
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TYPE"  Function Name: "VALUE"%s"RESET"\n", node->data.function_global_construct.name);
            print_ast(node->data.function_global_construct.parameters, depth + 2);
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TITLE"  Body:"RESET"\n");
            print_ast(node->data.function_global_construct.statements, depth + 2);
            break;
        case AST_COMPOUND_GLOBAL_CONSTRUCT:
            printf(TITLE"Compound Global Construct:"RESET"\n");
            for (size_t i = 0; i < node->data.compound_global_construct.global_construct_count; i++) {
                print_ast(node->data.compound_global_construct.global_constructs[i], depth + 1);
            }
            break;
        case AST_PARAMETER_LIST:
            printf(TITLE"Parameter List:"RESET"\n");
            for (size_t i = 0; i < node->data.parameter_list.parameter_count; i++)
            {
                print_ast(node->data.parameter_list.parameters[i], depth + 1);
            }
            break;
        case AST_PARAMETER:
            printf(TITLE"Parameter:" RESET "\n");
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TYPE"  Type:"VALUE" %s"RESET"\n", node->data.parameter.type);
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TYPE"  Name: "VALUE"%s"RESET"\n", node->data.parameter.name);
            break;
        case AST_COMPOUND_ARGUMENT:
            printf(TITLE"Arguments:"RESET"\n");
            for (int i = 0; i < node->data.compound_argument.argument_count; i++)
            {
                print_ast(node->data.compound_argument.arguments[i], depth + 1);
            }
            break;
        case AST_ARGUMENT:
            printf(TITLE"Argument:"RESET"\n");
            print_ast(node->data.argument.argument, depth + 1);
            break;
        case AST_FUNCTION_CALL:
            printf(TITLE"Function Call:"RESET"\n");
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TYPE"  Name: "VALUE"%s"RESET"\n", node->data.function_call.name);
            print_ast(node->data.function_call.argumentList, depth + 1);
            break;
        case AST_VARIABLE_DECLARATION:
            printf(TITLE"Variable Declaration:"RESET"\n");
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TYPE"  Type: "VALUE"%s"RESET"\n", node->data.variable.type);
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf(TYPE"  Name: "VALUE"%s"RESET"\n", node->data.variable.name);
            if (node->data.variable.expression)
            {
                for (int i = 0; i < depth; i++) {
                    printf("  ");
                }
                printf(TITLE"  Value:"RESET"\n");
                print_ast(node->data.variable.expression, depth + 2);
            }
            break;
            
        default:
            printf("Unknown AST Node Type\n");
    }
}