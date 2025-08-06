#include "interpreater.h"
#include <stdio.h>

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

// Set register value by code (00=a, 01=b, 10=c, 11=d)
void setRegister(unsigned char code, unsigned char value) {
    unsigned char* reg = getRegister(code);
    if (reg) {
        *reg = value;
    } else {
        printf("Invalid register code: %u\n", code);
    }
}

void printRegisters() {
    printf("Registers: a=%u b=%u c=%u d=%u\n", a, b, c, d);
}