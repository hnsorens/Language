#pragma once

#include "Lexer.h"

int CheckGlobalVariableDeclaration(Token* tokenList, int* offset, int allowedPatterns, void** structPtr);