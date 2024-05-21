gcc -o compiler src/Source/main.c src/Source/Lexer.c src/Source/Parser.c src/Source/SemanticAnalysis.c src/Source/ExecutableBuilder.c -I./src/Includes -lm
./compiler