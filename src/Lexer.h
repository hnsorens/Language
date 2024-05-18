#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "Tokenizer.h"

// typedef enum
// {
//     ZERO,
//     PATTERN_FUNCTION_DELARATION,
//     PATTERN_VARIABLE_DECLARATION,
// } PatternTypes;

int Lex(TokenList* tokenList);