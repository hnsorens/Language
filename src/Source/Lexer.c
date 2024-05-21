#include "Lexer.h"

/**
 * TODO
 * make it so identifiers can have numbers in them 
*/

Token create_token(TokenType type, const char* lexeme, int line, int column)
{
    Token token;
    token.type = type;
    token.lexeme = strdup(lexeme);
    token.line = line;
    token.column = column;
    return token;
}

Token next_token(LexerState *state)
{
    while (state->position < state->length)
    {
        char current_char = state->source[state->position];

        // Skip whitespace
        if (isspace(current_char))
        {
            if (current_char == '\n')
            {
                state->line++;
                state->column = 0;
            }
            else
            {
                state->column++;
            }
            state->position++;
            continue;
        }

        // Handle operators
        if (strchr("+-*/=<>&|", current_char))
        {
            size_t start = state->position;
            while (strchr("+-*/=<>&|", state->source[state->position]))
            {
                state->position++;
                state->column++;
            }
            size_t length = state->position - start;
            char *lexeme = strndup(&state->source[start], length);
            Token token = create_token(TOKEN_OPERATOR, lexeme, state->line, state->column);
            free(lexeme);
            return token;
        }


        // Handle numbers
        if (isdigit(current_char))
        {
            size_t start = state->position;
            while (isdigit(state->source[state->position]))
            {
                state->position++;
                state->column++;
            }
            size_t length = state->position - start;
            char *lexeme = strndup(&state->source[start], length);
            Token token = create_token(TOKEN_NUMBER, lexeme, state->line, state->column);
            free(lexeme);
            return token;
        }

        // Handle identifiers and keywords
        if (isalpha(current_char) || current_char == '_')
        {
            size_t start = state->position;
            while (isalpha(state->source[state->position]) || state->source[state->position] == '_')
            {
                state->position++;
                state->column++;
            }
            size_t length = state->position - start;
            char *lexeme = strndup(&state->source[start], length);
            // Keywords
            if (
                strcmp(lexeme, "int") == 0
                || strcmp(lexeme, "bool") == 0
                || strcmp(lexeme, "if") == 0
                || strcmp(lexeme, "for") == 0
                || strcmp(lexeme, "while") == 0
                || strcmp(lexeme, "else") == 0
                || strcmp(lexeme, "return") == 0
            )
            {
                Token token = create_token(TOKEN_KEYWORD, lexeme, state->line, state->column);
                free(lexeme);
                return token;
            }
            // Identifiers
            Token token = create_token(TOKEN_IDENTIFIER, lexeme, state->line, state->column);
            free(lexeme);
            return token;
        }


        // Handle Punctuation
        if (strchr(".,;(){}[]", current_char))
        {
            char lexeme[2] = {current_char, '\0'}; // Single character lexeme
            Token token = create_token(TOKEN_PUNCTUATOR, lexeme, state->line, state->column);
            state->position++;
            state->column++;
            return token;
        }

        // Hande String
        if (strchr("\"\'", current_char))
        {
            char start_char = current_char;
            size_t start = state->position;
            while (state->source[state->position] != start_char || state->source[state->position-1] == '\\')
            {
                state->position++;
                state->column++;
                if (state->source[state->position] == '\n')
                {
                    state->line++;
                    state->column = 0;
                }
            }
            size_t length = state->position - start;
            char *lexeme = strndup(&state->source[start]+1, length-2);
            Token token = create_token(TOKEN_STRING, lexeme, state->line, state->column);
            free(lexeme);
            return token;
        }

        // Unknown character (error handling)
        state->position++;
        state->column++;

        perror("Compiler error\n");
    }

    return create_token(TOKEN_EOF, "", state->line, state->column);
}