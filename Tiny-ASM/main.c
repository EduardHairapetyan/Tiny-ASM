#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	MOVI,
	ADD,
	SUB,
	MUL,
	DIV,
	IN,
	OUT
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
	// Trim leading
	while (**str == ' ' || **str == '\t' || **str == '\n') {
		(*str)++;
	}

	// Trim trailing
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

int handleCommand(char* operands, InstructionType type) {
	Instruction instr = { .value = 0 };

	switch (type) {
	case MOVI: {
		printf("Handling MOVI instruction: %s\n", operands);
		trimWhitespace(&operands);

		char* endptr;
		long value = strtol(operands, &endptr, 10);

		if (*endptr != '\0' || value < 0 || value > 127) {
			printf("Error: MOVI value must be between 0 and 127.\n");
			return -1;
		}

		instr.value = (unsigned char)value;
		instr.b7 = 0; // First bit = 0 for MOVI

		printf("%02x\n", instr.value);
		break;
	}

	case ADD:
	case SUB:
	case MUL:
	case DIV: {
		static const char* names[] = { "ADD", "SUB", "MUL", "DIV" };
		printf("Handling %s instruction: %s\n", names[type - 1], operands);

		instr.b7 = 1; // First bit = 1 for all except MOVI

		// Set opcode (b6 b5 b4)
		unsigned char opcode = type - 1; // ADD=1, ..., DIV=4
		instr.b6 = (opcode >> 2) & 1;
		instr.b5 = (opcode >> 1) & 1;
		instr.b4 = opcode & 1;

		char reg1, reg2;
		if (parseRegisters(operands, &reg1, &reg2) != 0) return -1;

		setRegisterBits(&instr, reg1, reg2);
		printf("%02x\n", instr.value);
		break;
	}

	case IN:
	case OUT: {
		printf("Handling %s instruction: %s\n", (type == IN) ? "IN" : "OUT", operands);

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

		printf("%02x\n", instr.value);
		break;
	}

	default:
		printf("Error: Unknown instruction.\n");
		return -1;
	}

	return 0;
}

int main() {
	const char* instructions[] = { "movi", "add", "sub", "mul", "div", "in", "out" };

	while (printf("Tiny-ASM> ") && fflush(stdout), 1) {
		char line[256];
		if (!fgets(line, sizeof(line), stdin)) break;

		char* token = strtok(line, "\n");
		if (!token) continue;

		if (_strnicmp(token, "exit", 4) == 0) {
			printf("Exiting Tiny-ASM.\n");
			break;
		}

		int found = 0;
		for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
			if (_strnicmp(token, instructions[i], strlen(instructions[i])) == 0) {
				char* operands = token + strlen(instructions[i]);
				handleCommand(operands, (InstructionType)i);
				found = 1;
				break;
			}
		}

		if (!found) {
			printf("Unknown instruction: '%s'\n", token);
		}
	}

	return 0;
}
