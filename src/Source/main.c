#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalysis.h"
#include "ExecutableBuilder.h"

int ReadFile(const char* name, const char** fileContents, size_t* length)
{
    // open the file in read mode
    FILE *file = fopen(name, "rb");

    if (file == NULL)
    {
        perror("Error opening file!");
        return 0;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file
    size_t file_size = ftell(file); // Get the current position, which is the size of the file
    fseek(file, 0, SEEK_SET); // Move the file pointer back to the beginning

    if (file_size == -1) {
        perror("Error getting file size");
        fclose(file);
        return 0;
    }

    // Allocate memory to store the file contents
    *fileContents = (char *)malloc(file_size + 1); // Allocate an extra byte for the null terminator

    if (*fileContents == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return 0;
    }

    // Read the file contents into the buffer
    *length = fread(*fileContents, 1, file_size, file);

    if (*length != file_size) {
        perror("Error reading file");
        fclose(file);
        free(*fileContents);
        return 0;
    }

    return 1;
}

int main() {
    

    size_t file_size;
    char *source_code;
    if (ReadFile("program.code", &source_code, &file_size))

    // Now you have the file contents in 'buffer' and its size in 'file_size'
    printf("File contents: %s\n", source_code);
    printf("File size: %ld bytes\n", file_size);

    LexerState lexer = {source_code, file_size, 0, 1, 1};
    Token tokens[100]; // Assuming a maximum of 100 tokens
    size_t token_count = 0;

    Token token;
    do 
    {
        token = next_token(&lexer);
        tokens[token_count++] = token;
        printf("Token: %s, Type: %d, Line: %d, Column: %d\n", token.lexeme, token.type, token.line, token.column);
    }
    while (token.type != TOKEN_EOF);

    ParserState parser_state = {tokens, token_count, 0};
    ASTNode *ast = parse_compound_global_construct(&parser_state);
    print_ast(ast, 0);

    SymbolTable symtab;
    init_symbol_table(&symtab);
    SemanticState semantic_state = {&symtab, 0};
    semantic_analysis(ast, &semantic_state);

    BuildExecutable(ast);


    // Free allocated lexeme strings
    for (size_t i = 0; i < token_count; i++) {
        free(tokens[i].lexeme);
    }

    return EXIT_SUCCESS;
}