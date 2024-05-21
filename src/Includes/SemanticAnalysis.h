#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "Parser.h"

typedef enum
{
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
} SymbolType;

typedef struct Symbol
{
    char *name;
    SymbolType type;
    int scope_level;
    struct Symbol *next;
} Symbol;

#define SYMBOL_TABLE_SIZE 100

typedef struct
{
    Symbol *table[SYMBOL_TABLE_SIZE];
} SymbolTable;

typedef struct
{
    SymbolTable *symtab;
    int current_scope;
} SemanticState;


void semantic_analysis(ASTNode *node, SemanticState *state);
void init_symbol_table(SymbolTable *symtab);