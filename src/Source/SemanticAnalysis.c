#include "SemanticAnalysis.h"

unsigned int hash(const char *str)
{
    unsigned int hash = 0;
    while (*str)
    {
        hash = (hash << 5) + *str++;
    }

    return hash % SYMBOL_TABLE_SIZE;
}

void init_symbol_table(SymbolTable *symtab)
{
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        symtab->table[i] = NULL;
    }
}

void insert_symbol(SymbolTable *symtab, const char* name, SymbolType type, int scope_level)
{
    unsigned int index = hash(name);
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = type;
    new_symbol->scope_level = scope_level;
    new_symbol->next = symtab->table[index];
    symtab->table[index] = new_symbol;
}

Symbol* lookup_symbol(SymbolTable *symtab, const char *name, int scope_level)
{
    unsigned int index = hash(name);
    Symbol *symbol = symtab->table[index];
    while (symbol)
    {
        if (strcmp(symbol->name, name) == 0 && symbol->scope_level <= scope_level)
        {
            return symbol;
        }

        symbol = symbol->next;
    }

    return NULL;
}

void remove_scope_symbols(SymbolTable *symtab, int scope_level) {
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        Symbol *symbol = symtab->table[i];
        Symbol *prev = NULL;
        while (symbol) {
            if (symbol->scope_level == scope_level) {
                if (prev) {
                    prev->next = symbol->next;
                } else {
                    symtab->table[i] = symbol->next;
                }
                free(symbol->name);
                free(symbol);
                symbol = (prev) ? prev->next : symtab->table[i];
            } else {
                prev = symbol;
                symbol = symbol->next;
            }
        }
    }
}

void free_symbol_table(SymbolTable *symtab) {
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        Symbol *symbol = symtab->table[i];
        while (symbol) {
            Symbol *temp = symbol;
            symbol = symbol->next;
            free(temp->name);
            free(temp);
        }
        symtab->table[i] = NULL;
    }
}

void enter_scope(SemanticState *state)
{
    state->current_scope++;
}

void exit_scope(SemanticState *state)
{
    remove_scope_symbols(state->symtab, state->current_scope);
    state->current_scope--;
}

void add_variable(SemanticState *state, const char *name)
{
    insert_symbol(state->symtab, name, SYMBOL_VARIABLE, state->current_scope);
}



void semantic_analysis(ASTNode *node, SemanticState *state)
{
    if (!node) return;

    switch (node->type)
    {
    case AST_VARIABLE_DECLARATION:
        if (lookup_symbol(state->symtab, node->data.variable.name, state->current_scope))
        {
            fprintf(stderr, "Error: Variable '%s' already declared in this scope.\n", node->data.variable.name);
        }
        else
        {
            insert_symbol(state->symtab, node->data.variable.name, SYMBOL_VARIABLE, state->current_scope);
        }
        break;
    case AST_ASSIGNMENT:
        if (!lookup_symbol(state->symtab, node->data.assignment.left->data.identifier_name, state->current_scope))
        {
            fprintf(stderr, "Error: Variable '%s' not declared.\n", node->data.assignment.left->data.identifier_name);
        }
        semantic_analysis(node->data.assignment.right, state);
        break;

    case AST_BINARY_OP:
        semantic_analysis(node->data.binary_op.left, state);
        semantic_analysis(node->data.binary_op.right, state);
        break;

    case AST_IDENTIFIER:
        if (!lookup_symbol(state->symtab, node->data.identifier_name, state->current_scope))
        {
            fprintf(stderr, "Error: Variable '%s' not declared.\n", node->data.identifier_name);
        }
        break;

    case AST_FUNCTION_DECLARATION:
        if (lookup_symbol(state->symtab, node->data.function_global_construct.name, state->current_scope))
        {
            fprintf(stderr, "Error: Function '%s' already declared in this scope.\n", node->data.function_global_construct.name);
        }
        else
        {
            insert_symbol(state->symtab, node->data.function_global_construct.name, SYMBOL_FUNCTION, state->current_scope);
        }
        enter_scope(state);
        semantic_analysis(node->data.function_global_construct.statements, state);
        remove_scope_symbols(state->symtab, state->current_scope);
        exit_scope(state);
        break;
    case AST_COMPOUND_STATEMENT:
        for (int i = 0; i < node->data.compound_statement.statement_count; i++)
        {
            semantic_analysis(node->data.compound_statement.statements[i], state);
        }
        break;
    case AST_FUNCTION_CALL:
        // if (!lookup_symbol(state->symtab, node->data.function_call.name, state->current_scope))
        // {
        //     fprintf(stderr, "Error: Function '%s' not declared.\n", node->data.function_call.name);
        // }
        semantic_analysis(node->data.function_call.argumentList, state);
        break;
    case AST_COMPOUND_ARGUMENT:
        for (int i = 0; i < node->data.compound_argument.argument_count; i++)
        {
            semantic_analysis(node->data.compound_argument.arguments[i], state);
        }
        break;
    case AST_ARGUMENT:
        semantic_analysis(node->data.argument.argument, state);
        break;
    case AST_PARAMETER_LIST:
        for (int i = 0; i < node->data.parameter_list.parameter_count; i++)
        {
            semantic_analysis(node->data.parameter_list.parameters[i], state);
        }
        break;
    case AST_COMPOUND_GLOBAL_CONSTRUCT:
        for (int i = 0; i < node->data.compound_global_construct.global_construct_count; i++)
        {
            semantic_analysis(node->data.compound_global_construct.global_constructs[i], state);
        }
        break;
    case AST_EXPRESSION_STATEMENT:
        semantic_analysis(node->data.expression_statement.expression, state);
        break;
    
    default:
        break;
    }
}