#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "Tokenizer.h"
#include "Lexer.h"

FILE *file;



int main() {
    // Open the file in read mode
    FILE *file = fopen("program.code", "rb"); // Open in binary mode to handle non-text files

    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file
    long file_size = ftell(file); // Get the current position, which is the size of the file
    fseek(file, 0, SEEK_SET); // Move the file pointer back to the beginning

    if (file_size == -1) {
        perror("Error getting file size");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Allocate memory to store the file contents
    char *buffer = (char *)malloc(file_size + 1); // Allocate an extra byte for the null terminator

    if (buffer == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Read the file contents into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);

    if (bytes_read != file_size) {
        perror("Error reading file");
        fclose(file);
        free(buffer);
        return EXIT_FAILURE;
    }

    // Add null terminator at the end of the buffer
    buffer[file_size] = '\0';

    // Close the file
    fclose(file);

    // Now you have the file contents in 'buffer' and its size in 'file_size'
    printf("File contents: %s\n", buffer);
    printf("File size: %ld bytes\n", file_size);

    TokenList tokenList;
    tokenList.capacity = 256;
    tokenList.tokenCount = 0;
    tokenList.tokens = (Token*)malloc(sizeof(Token) * tokenList.capacity);

    tokenize(&tokenList, buffer);
    if (Lex(&tokenList) == 0)
    {
        perror("Compile Error");
    }

    for (int i = 0; i < 40; i++)
    {
        printf("%i ", tokenList.tokens[i].token.i);
    }

    // Don't forget to free the allocated memory
    free(buffer);

    return EXIT_SUCCESS;
}