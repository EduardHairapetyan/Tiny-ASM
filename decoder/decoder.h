#ifndef _DECODER_H_
#define _DECODER_H_

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

const char* regName(unsigned char code);

#endif // !_DECODER_H_
