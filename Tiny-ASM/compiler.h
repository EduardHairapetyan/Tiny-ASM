#ifndef _COMPILER_H
#define _COMPILER_H


typedef enum {
    MOVI,
    ADD,
    SUB,
    MUL,
    DIV,
    IN,
    OUT,
    INVALID_INSTR = -1
} InstructionType;

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

typedef struct {
    char* data;
    size_t capacity;
    size_t length;
} ProgramBuffer;


void initProgramBuffer(ProgramBuffer* buf);
void appendInstruction(ProgramBuffer* buf, unsigned char instr);
void freeProgramBuffer(ProgramBuffer* buf);
char registerCode(const char* regName);
void trimWhitespace(char** str);
int parseRegisters(char* operands, char* regCode1, char* regCode2);
void setRegisterBits(Instruction* instr, char reg1, char reg2);
int handleCommand(char* operands, InstructionType type, ProgramBuffer* buf);
InstructionType findInstructionType(const char* token);
int compileFile(const char* inputFilename, ProgramBuffer* buf);
int writeBinaryFile(const char* outputFilename, ProgramBuffer* buf);

#endif