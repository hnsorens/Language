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

void lookup_symbol(SymbolTable *symtab, const char *name, int scope_level)
{
    unsigned int index = hash(name);
    Symbol *symbol = symtab->table[index];
    while (symbol)
    {
        if (strcmp(symbol->name, name) == 0 && symbol->scope_level == scope_level)
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

void ender_scope(SemanticState *state)
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




void semantic_analysis(ASTNode *node, SemanticState *state);

void semantic_analysis_block(ASTNode *block, SemanticState *state)
{

}

void semantic_analysis(ASTNode *node, SemanticState *state)
{
    if (!node) return;

    // switch (node->type)
    // {
    // case /* constant-expression */:
    //     /* code */
    //     break;
    
    // default:
    //     break;
    // }
}