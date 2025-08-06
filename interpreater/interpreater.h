#ifndef _INTERPREATER_H_
#define _INTERPREATER_H_

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

unsigned char* getRegister(unsigned char code);

void printRegisters();

void setRegister(unsigned char code, unsigned char value);

#endif