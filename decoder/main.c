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

const char* regName(unsigned char code) {
    switch (code) {
    case 0: return "a";
    case 1: return "b";
    case 2: return "c";
    case 3: return "d";
    default: return "?";
    }
}

int main(int argc, char* argv[]) {
    const char* inputFile = "out.bin";
    const char* outputFile = "disassembled.asm";

    FILE* in = fopen(inputFile, "rb");
    if (!in) {
        perror("Failed to open input binary file");
        return 1;
    }

    FILE* out = fopen(outputFile, "w");
    if (!out) {
        perror("Failed to create disassembly output file");
        fclose(in);
        return 1;
    }

    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    fseek(in, 0, SEEK_SET);

    if (size <= 0) {
        printf("Empty binary file.\n");
        fclose(in);
        fclose(out);
        return 1;
    }

    printf("Disassembly:\n");

    for (long pc = 0; pc < size; pc++) {
        Instruction instr;
        fread(&instr.value, 1, 1, in);

        if (instr.b7 == 0) {
            // MOVI
            unsigned char imm = instr.value & 0x7F;
            printf("MOVI #%u\n", imm);
            fprintf(out, "MOVI #%u\n", imm);
        }
        else {
            unsigned char opcode = (instr.b6 << 2) | (instr.b5 << 1) | instr.b4;
            unsigned char r1 = (instr.b3 << 1) | instr.b2;
            unsigned char r2 = (instr.b1 << 1) | instr.b0;

            switch (opcode) {
            case 0b000:
                printf("ADD %s,%s\n", regName(r1), regName(r2));
                fprintf(out, "ADD %s,%s\n", regName(r1), regName(r2));
                break;
            case 0b001:
                printf("SUB %s,%s\n", regName(r1), regName(r2));
                fprintf(out, "SUB %s,%s\n", regName(r1), regName(r2));
                break;
            case 0b010:
                printf("MUL %s,%s\n", regName(r1), regName(r2));
                fprintf(out, "MUL %s,%s\n", regName(r1), regName(r2));
                break;
            case 0b011:
                printf("DIV %s,%s\n", regName(r1), regName(r2));
                fprintf(out, "DIV %s,%s\n", regName(r1), regName(r2));
                break;
            case 0b100: {
                int isIn = !((instr.b3 << 1) | instr.b2);
                if (isIn) {
                    printf("IN %s\n", regName(r2));
                    fprintf(out, "IN %s\n", regName(r2));
                }
                else {
                    printf("OUT %s\n", regName(r2));
                    fprintf(out, "OUT %s\n", regName(r2));
                }
                break;
            }
            default:
                printf("??? (Unknown opcode: %u)\n", opcode);
                fprintf(out, "??? (Unknown opcode: %u)\n", opcode);
                break;
            }
        }
    }

    fclose(in);
    fclose(out);
    printf("Disassembly complete. Output written to %s\n", outputFile);
    return 0;
}
