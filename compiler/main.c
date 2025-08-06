#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"


int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input.asm> <output.bin>\n", argv[0]);
        return 1;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];

    ProgramBuffer program;
    initProgramBuffer(&program);

    printf("Compiling '%s'...\n", inputFile);
    if (compileFile(inputFile, &program) != 0) {
        printf("Compilation failed.\n");
        freeProgramBuffer(&program);
        return 1;
    }

    if (writeBinaryFile(outputFile, &program) != 0) {
        freeProgramBuffer(&program);
        return 1;
    }

    freeProgramBuffer(&program);
    printf("Done.\n");
    return 0;
}
