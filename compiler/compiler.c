#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// Initialize dynamic program buffer
void initProgramBuffer(ProgramBuffer* buf) {
    buf->capacity = 256;
    buf->length = 0;
    buf->data = malloc(buf->capacity);
    if (!buf->data) {
        perror("Failed to allocate program buffer");
        exit(EXIT_FAILURE);
    }
}

// Append a byte to program buffer, growing if needed
void appendInstruction(ProgramBuffer* buf, unsigned char instr) {
    if (buf->length >= buf->capacity) {
        buf->capacity *= 2;
        char* newData = realloc(buf->data, buf->capacity);
        if (!newData) {
            perror("Failed to reallocate program buffer");
            free(buf->data);
            exit(EXIT_FAILURE);
        }
        buf->data = newData;
    }
    buf->data[buf->length++] = instr;
}

// Free program buffer memory
void freeProgramBuffer(ProgramBuffer* buf) {
    free(buf->data);
    buf->data = NULL;
    buf->length = 0;
    buf->capacity = 0;
}

char registerCode(const char* regName) {
    switch (regName[0]) {
    case 'a': return 0; // 00
    case 'b': return 1; // 01
    case 'c': return 2; // 10
    case 'd': return 3; // 11
    default: return -1;
    }
}

void trimWhitespace(char** str) {
    while (**str == ' ' || **str == '\t' || **str == '\n') (*str)++;

    char* end = *str + strlen(*str) - 1;
    while (end > *str && (*end == ' ' || *end == '\t' || *end == '\n')) {
        *end-- = '\0';
    }
}

int parseRegisters(char* operands, char* regCode1, char* regCode2) {
    char* reg1 = strtok(operands, ",");
    char* reg2 = strtok(NULL, ",");

    if (!reg1 || !reg2) {
        printf("Error: Instruction requires two registers.\n");
        return -1;
    }

    trimWhitespace(&reg1);
    trimWhitespace(&reg2);

    *regCode1 = registerCode(reg1);
    *regCode2 = registerCode(reg2);

    if (*regCode1 == -1 || *regCode2 == -1) {
        printf("Error: Invalid register name.\n");
        return -1;
    }
    return 0;
}

void setRegisterBits(Instruction* instr, char reg1, char reg2) {
    instr->b3 = (reg1 >> 1) & 1;
    instr->b2 = reg1 & 1;
    instr->b1 = (reg2 >> 1) & 1;
    instr->b0 = reg2 & 1;
}

int handleCommand(char* operands, InstructionType type, ProgramBuffer* buf) {
    Instruction instr = { .value = 0 };

    switch (type) {
    case MOVI: {
        trimWhitespace(&operands);
        printf("Compiling MOVI instruction...\n");

        char* endptr;
        long value = strtol(operands, &endptr, 10);

        if (*endptr != '\0' || value < 0 || value > 127) {
            printf("Error: MOVI value must be between 0 and 127.\n");
            return -1;
        }

        instr.value = (unsigned char)value;
        instr.b7 = 0; // MOVI instruction first bit = 0

        appendInstruction(buf, instr.value);
        break;
    }

    case ADD:
    case SUB:
    case MUL:
    case DIV: {
        static const char* names[] = { "ADD", "SUB", "MUL", "DIV" };
        printf("Compiling %s instruction...\n", names[type - 1]);

        instr.b7 = 1;
        unsigned char opcode = (unsigned char)(type - 1); // ADD=1,...,DIV=4

        instr.b6 = (opcode >> 2) & 1;
        instr.b5 = (opcode >> 1) & 1;
        instr.b4 = opcode & 1;

        char reg1, reg2;
        if (parseRegisters(operands, &reg1, &reg2) != 0)
            return -1;

        setRegisterBits(&instr, reg1, reg2);

        appendInstruction(buf, instr.value);
        break;
    }

    case IN:
    case OUT: {
        printf("Compiling %s instruction...\n", (type == IN) ? "IN" : "OUT");

        trimWhitespace(&operands);

        char reg = registerCode(operands);
        if (reg == -1) {
            printf("Error: Invalid register name.\n");
            return -1;
        }

        instr.b7 = 1;
        instr.b6 = 1;
        instr.b5 = 0;
        instr.b4 = 0;
        instr.b3 = 0;
        instr.b2 = (type == OUT) ? 1 : 0;

        instr.b1 = (reg >> 1) & 1;
        instr.b0 = reg & 1;

        appendInstruction(buf, instr.value);
        break;
    }

    default:
        printf("Error: Unknown instruction.\n");
        return -1;
    }

    return 0;
}

// Find instruction type by mnemonic
InstructionType findInstructionType(const char* token) {
    static const char* instructions[] = { "movi", "add", "sub", "mul", "div", "in", "out" };
    for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
        if (_strnicmp(token, instructions[i], strlen(instructions[i])) == 0)
            return (InstructionType)i;
    }
    return INVALID_INSTR;
}

// Compile the entire source file
int compileFile(const char* inputFilename, ProgramBuffer* buf) {
    FILE* fin = fopen(inputFilename, "r");
    if (!fin) {
        perror("Failed to open input file");
        return -1;
    }

    char line[256];
    int lineNumber = 0;
    while (fgets(line, sizeof(line), fin)) {
        lineNumber++;
        char* token = strtok(line, "\n");
        if (!token) continue;

        char* commentStart = strchr(line, ';');
        if (commentStart) {
            *commentStart = '\0'; // terminate string at comment start
        }

        // Skip empty or comment lines (optional: add comment support)
        while (*token == ' ' || *token == '\t') token++;
        if (*token == '\0') continue;

        InstructionType type = findInstructionType(token);
        if (type == INVALID_INSTR) {
            printf("Error on line %d: Unknown instruction '%s'\n", lineNumber, token);
            fclose(fin);
            return -1;
        }

        char* operands = token + strlen((const char*)token);
        // Because findInstructionType matches only prefix, we find operand start by scanning token:
        // We'll re-scan line instead to find operands
        operands = line;
        while (*operands && !(*operands == ' ' || *operands == '\t')) operands++; // skip mnemonic
        while (*operands == ' ' || *operands == '\t') operands++; // skip whitespace before operands

        if (handleCommand(operands, type, buf) != 0) {
            printf("Compilation failed on line %d\n", lineNumber);
            fclose(fin);
            return -1;
        }
    }

    fclose(fin);
    return 0;
}

int writeBinaryFile(const char* outputFilename, ProgramBuffer* buf) {
    FILE* fout = fopen(outputFilename, "wb");
    if (!fout) {
        perror("Failed to open output file");
        return -1;
    }
    size_t written = fwrite(buf->data, 1, buf->length, fout);
    fclose(fout);
    if (written != buf->length) {
        printf("Error: Failed to write all bytes to output file\n");
        return -1;
    }
    printf("Successfully wrote %zu bytes to '%s'\n", buf->length, outputFilename);
    return 0;
}