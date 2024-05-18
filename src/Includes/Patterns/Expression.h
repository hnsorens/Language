#pragma once

#include "Lexer.h"

int CheckExpression(Token* tokenList, int* offset, int allowedPatterns, void** structPtr);