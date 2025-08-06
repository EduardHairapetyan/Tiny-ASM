#include "decoder.h"

const char* regName(unsigned char code) {
    switch (code) {
    case 0: return "a";
    case 1: return "b";
    case 2: return "c";
    case 3: return "d";
    default: return "?";
    }
}