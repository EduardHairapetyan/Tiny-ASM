#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

typedef union {
    struct {
        unsigned char b0 : 1;
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3 : 1;
        unsigned char b4 : 1;
        unsigned char b5 : 1;
        unsigned char b6 : 1;
        unsigned char b7 : 1;
    };
    unsigned char value;
} Instruction;

// Explicit registers
unsigned char a = 0;
unsigned char b = 0;
unsigned char c = 0;
unsigned char d = 0;

// Get pointer to register by code (00=a, 01=b, 10=c, 11=d)
unsigned char* getRegister(unsigned char code) {
    switch (code) {
    case 0: return &a;
    case 1: return &b;
    case 2: return &c;
    case 3: return &d;
    default: return NULL;
    }
}

void printRegisters() {
    printf("Registers: a=%u b=%u c=%u d=%u\n", a, b, c, d);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <binaryfile>\n", argv[0]);
        return 1;
    }

    FILE* f = fopen(argv[1], "rb");
    if (!f) {
        perror("Failed to open binary file");
        return 1;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (filesize <= 0) {
        printf("Empty or invalid file.\n");
        fclose(f);
        return 1;
    }

    unsigned char* program = malloc(filesize);
    if (!program) {
        perror("Memory allocation failed");
        fclose(f);
        return 1;
    }

    fread(program, 1, filesize, f);
    fclose(f);

    long pc = 0;

    while (pc < filesize) {
        Instruction instr;
        instr.value = program[pc++];

        if (instr.b7 == 0) {
            // MOVI: immediate value 0..127
            unsigned char imm = instr.value & 0x7F;
            a = imm; // MOVI always loads to register a by convention
            printf("MOVI # %u -> a\n", imm);
        }
        else {
            // Instructions with b7=1: decode opcode in bits b6,b5,b4
            unsigned char opcode = (instr.b6 << 2) | (instr.b5 << 1) | instr.b4;

            switch (opcode) {
            case 0b000: // ADD
            case 0b001: // SUB
            case 0b010: // MUL
            case 0b011: // DIV
            {
                unsigned char r1 = (instr.b3 << 1) | instr.b2;
                unsigned char r2 = (instr.b1 << 1) | instr.b0;
                unsigned char* reg1 = getRegister(r1);
                unsigned char* reg2 = getRegister(r2);

                if (!reg1 || !reg2) {
                    printf("Invalid register code in arithmetic instruction\n");
                    free(program);
                    return 1;
                }

                switch (opcode) {
                case 0b000: // ADD
                    *reg1 += *reg2;
                    printf("ADD %c += %c -> %u\n", 'a' + r1, 'a' + r2, *reg1);
                    break;
                case 0b001: // SUB
                    *reg1 -= *reg2;
                    printf("SUB %c -= %c -> %u\n", 'a' + r1, 'a' + r2, *reg1);
                    break;
                case 0b010: // MUL
                    *reg1 *= *reg2;
                    printf("MUL %c *= %c -> %u\n", 'a' + r1, 'a' + r2, *reg1);
                    break;
                case 0b011: // DIV
                    if (*reg2 == 0) {
                        printf("Error: Division by zero\n");
                        free(program);
                        return 1;
                    }
                    *reg1 /= *reg2;
                    printf("DIV %c /= %c -> %u\n", 'a' + r1, 'a' + r2, *reg1);
                    break;
                }
                break;
            }
            case 0b100: { // IN
                int isIn = !((instr.b3 << 1) | instr.b2);
                if (isIn)
                {
                    unsigned char regCode = (instr.b1 << 1) | instr.b0;
                    unsigned char* reg = getRegister(regCode);
                    if (!reg) {
                        printf("Invalid register code in IN\n");
                        free(program);
                        return 1;
                    }

                    printf("IN %c: enter value (0-127): ", 'a' + regCode);

                    // Read input line
                    char input[16];
                    if (!fgets(input, sizeof(input), stdin)) {
                        printf("Input error.\n");
                        free(program);
                        return 1;
                    }

                    // Convert to integer
                    int val = atoi(input);
                    if (val < 0 || val > 127) val = 0;

                    *reg = (unsigned char)val;
                    printf("Read %u into %c\n", *reg, 'a' + regCode);
                    break;

                }
                else
                {
                    unsigned char regCode = (instr.b1 << 1) | instr.b0;
                    unsigned char* reg = getRegister(regCode);
                    if (!reg) {
                        printf("Invalid register code in OUT\n");
                        free(program);
                        return 1;
                    }
                    printf("OUT %c: %u\n", 'a' + regCode, *reg);
                    break;
                }
            }
            default:
                printf("Unknown opcode: %u\n", opcode);
                free(program);
                return 1;
            }
        }
    }

    printRegisters();
    free(program);
    return 0;
}
