#pragma once

#include "Lexer.h"

int CheckFunctionDeclaration(Token* tokenList, int* offset, int allowedPatterns, void** structPtr);