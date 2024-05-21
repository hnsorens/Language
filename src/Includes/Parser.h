#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "Lexer.h"

typedef enum
{
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_STRING,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_ASSIGNMENT,
    AST_COMPOUND_STATEMENT,
    AST_EXPRESSION_STATEMENT,
    AST_IF_STATEMENT,
    AST_FOR_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_RETURN_STATEMENT,
    AST_FUNCTION_DECLARATION,
    AST_VARIABLE_DECLARATION,
    AST_COMPOUND_GLOBAL_CONSTRUCT,
    AST_PARAMETER_LIST,
    AST_PARAMETER,
    AST_FUNCTION_CALL,
    AST_COMPOUND_ARGUMENT,
    AST_ARGUMENT,
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        int number_value; // For AST_NUMBER
        char *identifier_name; // For AST_IDENTIFIER
        char *string_value; // For AST_STRING
        struct {
            char* op; // '+', '-', '*', '/', etc.
            struct ASTNode *left;
            struct ASTNode *right;
        } binary_op; // For AST_BINARY_OP
        struct {
            char* op; // '-' for negation, etc.
            struct ASTNode *operand;
        } unary_op; // For AST_UNARY_OP
        struct {
            struct ASTNode *left; // Always an identifier node
            struct ASTNode *right; // The expression assigned to the identifier
        } assignment; // For AST_ASSIGNMENT
        struct {
            struct ASTNode **statements; // Array of statements
            size_t statement_count; // Number of statements
        } compound_statement; // For AST_COMPOUND_STATEMENT
        struct {
            struct ASTNode *expression; // The expression in the statement
        } expression_statement; // For AST_EXPRESSION_STATEMENT
        struct {
            struct ASTNode *condition; // The condition expression
            struct ASTNode *then_branch; // The then branch
            struct ASTNode *else_branch; // The else branch (optional)
        } if_statement; // For AST_IF_STATEMENT
        struct {
            struct ASTNode *condition; // The condition expression
            struct ASTNode *body; // The body of the while loop
        } while_statement; // For AST_WHILE_STATEMENT
        struct {
            struct ASTNode *expression; // The return expression (optional)
        } return_statement; // For AST_RETURN_STATEMENT
        struct
        {
            struct ASTNode **global_constructs;
            size_t global_construct_count;
        } compound_global_construct;
        struct 
        {
            const char *return_type;
            const char *name;
            struct ASTNode *parameters;
            struct ASTNode *statements;
        } function_global_construct;
        struct
        {
            const char *type;
            const char *name;
            struct ASTNode *expression;
        } variable;
        struct
        {
            const char *type;
            const char *name;
        } parameter;
        struct
        {
            struct ASTNode **parameters;
            size_t parameter_count;
        } parameter_list;
        struct
        {
            const char* name;
            struct ASTNode* argumentList;
        } function_call;
        struct
        {
            struct ASTNode** arguments;
            size_t argument_count;
        } compound_argument;
        struct
        {
            struct ASTNode* argument;
        } argument;
        
    } data;
} ASTNode;

typedef struct {
    Token *tokens;     // Array of tokens
    size_t token_count; // Total number of tokens
    size_t position;    // Current position in the token array
} ParserState;

ASTNode* parse_expression(ParserState *state);
ASTNode* parse_statement(ParserState *state);
ASTNode* parse_compound_statement(ParserState *state);
ASTNode* parse_compound_global_construct(ParserState *state);
ASTNode *parse_primary(ParserState *state);
void print_ast(ASTNode *node, int depth);