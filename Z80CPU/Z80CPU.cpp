#include "Z80CPU.h"

//available 8 bit opcodes
static unsigned char opcodes8[] =
{
	//00
	0x36, //LD_HL_N
	0x18, //JR_E
	0x06, //LD_R_N
	0x01, //LD_DD_NN
	0x00, //NOP
	//01
	0x70, //LD_HL_R
	0x46, //LD_R_HL
	0x40, //LD_R1_R2
	//10
	0x80, // ADD_A_R
	//11
	0xEB, //EX_DE_HL
	0xCD, //CALL_NN
	0xC9, //RET
	0xC3, //JP_NN
};
const size_t opcodes8Size = sizeof(opcodes8) / sizeof(opcodes8[0]);
//unsigned char* opcodes8[] =
//{
//	block00,block01,block10,block11
//};
//const size_t opcodesSizes[4] =
//{	
//	sizeof(block00) / sizeof(block00[0]),
//	sizeof(block01) / sizeof(block01[0]),
//	sizeof(block10) / sizeof(block10[0]),
//	sizeof(block11) / sizeof(block11[0]),
//};
//const size_t opcodesOffsets[5] =
//{
//	0,
//	opcodesSizes[0],
//	opcodesSizes[0] + opcodesSizes[1],
//	opcodesSizes[0] + opcodesSizes[1] + opcodesSizes[2],
//	opcodesSizes[0] + opcodesSizes[1] + opcodesSizes[2] + opcodesSizes[3]
//};
//available 16 bit opcodes

static unsigned short opcodes16[] =
{
	0b1111110101000110, //LD_R_IY_D
	0b1101110101000110, //LD_R_IX_D
	0b1100101101000000, //BIT_B_R
};
const size_t opcodes16Size = sizeof(opcodes16) / sizeof(opcodes16[0]);

void init(Z80Cpu* z80Cpu)
{
	assert(opcodes8Size + opcodes16Size == OPCODES_SIZE);

	z80Cpu->BC = (unsigned short*)(z80Cpu->basicGpRegisters + B);
	z80Cpu->DE = (unsigned short*)(z80Cpu->basicGpRegisters + D);
	z80Cpu->HL = (unsigned short*)(z80Cpu->basicGpRegisters + H);
	z80Cpu->AF = (unsigned short*)(z80Cpu->basicGpRegisters + F);
	z80Cpu->ADDITIONAL_BC = (unsigned short*)(z80Cpu->additionalGpRegisters + B);
	z80Cpu->ADDITIONAL_DE = (unsigned short*)(z80Cpu->additionalGpRegisters + D);
	z80Cpu->ADDITIONAL_HL = (unsigned short*)(z80Cpu->additionalGpRegisters + H);
	z80Cpu->ADDITIONAL_AF = (unsigned short*)(z80Cpu->additionalGpRegisters + F);

	z80Cpu->running = true;
}

unsigned char fetch(Z80Cpu* z80Cpu) {
	return z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
}

void execute(Z80Cpu* z80Cpu) {

	// add evaluation of extended opcodes
	// test evaluation of 16 bits opcodes
	size_t index = 0;
	unsigned int opcode = fetch(z80Cpu);
	if (!evaluate(opcode, opcodes8, opcodes8Size, index))
	{
		opcode = (opcode << 8) | fetch(z80Cpu);
		evaluate(opcode, opcodes16, opcodes16Size, index);
	}

	switch (index)
	{
	case LD_HL_N:
	{
		z80Cpu->ram[*z80Cpu->HL] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
		printf("LD_HL_N\n");
		break;
	}
	case LD_R_N:
		z80Cpu->basicGpRegisters[opcode >> 3] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
		printf("LD_R_N\n");
		break;
	case LD_DD_NN:
	{
		unsigned char dd = (opcode & 0x30) >> 4;
		switch (dd)//make function
		{
		case 0b00:
			*z80Cpu->BC = *((unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[PC]));
			break;
		case 0b01:
			*z80Cpu->DE = *((unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[PC]));
			break;
		case 0b10:
			*z80Cpu->HL = *((unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[PC]));
			break;
		case 0b11:
			z80Cpu->spRegisters16[SP] = *((unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[PC]));
			break;
		default:
			z80Cpu->running = false;
			printf("LD_DD_NN OPCODE ERROR\n");
			break;
		}
		z80Cpu->spRegisters16[PC] += 2;
		printf("LD_DD_NN\n");
		break;
	}
	case NOP:
		z80Cpu->running = false;
		printf("NOP\n");
		break;
	case LD_HL_R:
	{
		z80Cpu->ram[*z80Cpu->HL] = z80Cpu->basicGpRegisters[opcode & 0b00000111];
		printf("LD_HL_R\n");
		break;
	}
	case LD_R_HL:
	{
		z80Cpu->basicGpRegisters[(opcode & 0b00111000) >> 3] = z80Cpu->ram[*z80Cpu->HL];
		printf("LD_R_HL\n");
		break;
	}
	case LD_R1_R2:
		z80Cpu->basicGpRegisters[(opcode & 0b00111000) >> 3] = z80Cpu->basicGpRegisters[opcode & 0b00000111];
		printf("LD_R1_R2\n");
		break;
	case ADD_A_R:
	{
		unsigned char tempA = z80Cpu->basicGpRegisters[A];
		unsigned char reg = opcode & 0b00000111;
		z80Cpu->basicGpRegisters[A] += z80Cpu->basicGpRegisters[reg];
		
		//S is set if result is negative; otherwise, it is reset
		(z80Cpu->basicGpRegisters[A] >> 7) & 1 ?
			SET_CONDITION_BIT(CB_S) :
			RESET_CONDITION_BIT(CB_S);
		//Z is set if result is 0; otherwise, it is reset
		z80Cpu->basicGpRegisters[A] == 0 ?
			SET_CONDITION_BIT(CB_Z) :
			RESET_CONDITION_BIT(CB_Z);
		//H is set if carry from bit 3; otherwise, it is reset
		(tempA & 0x10) ^
		(reg & 0x10) ^
		(z80Cpu->basicGpRegisters[A]) ?
			SET_CONDITION_BIT(CB_H) :
			RESET_CONDITION_BIT(CB_H);
		//P/V is set if overflow; otherwise, it is reset
		(char)tempA > 0 &&
		(char)reg > 0 &&
		(char)z80Cpu->basicGpRegisters[A] > 0 ||
		(char)tempA < 0 &&
		(char)reg < 0 &&
		(char)z80Cpu->basicGpRegisters[A] < 0 ?
			RESET_CONDITION_BIT(CB_PV) :
			SET_CONDITION_BIT(CB_PV);
		//N is reset
		RESET_CONDITION_BIT(CB_N);
		//C is set if carry from bit 7; otherwise, it is reset
		z80Cpu->basicGpRegisters[A] < tempA + reg ?
			SET_CONDITION_BIT(CB_C) :
			RESET_CONDITION_BIT(CB_C);
		printf("ADD_A_R\n");
		break;
	}
	case EX_DE_HL:
	{
		swap(z80Cpu->DE,z80Cpu->HL);
		printf("EX_DE_HL\n");
		break;
	}
	case CALL_NN:
		z80Cpu->spRegisters16[SP]-=2;
		*(unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[SP]) = z80Cpu->spRegisters16[PC] + 2;
		//printf("%x", z80Cpu->ram[z80Cpu->spRegisters16[SP]+1]);
		z80Cpu->spRegisters16[PC] = *((unsigned short*)(z80Cpu->ram+z80Cpu->spRegisters16[PC]));
		printf("CALL_NN\n");
		break;
	case RET:
		z80Cpu->spRegisters16[PC] = *((unsigned short*)(z80Cpu->ram+z80Cpu->spRegisters16[SP]));
		z80Cpu->spRegisters16[SP] += 2;
		printf("RET\n");
		break;
	case JP_NN:
		z80Cpu->spRegisters16[PC] = *((unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[PC]));
		printf("JP_NN\n");
		break;
	case LD_R_IY_D://test signed d
	{
		char d = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
		z80Cpu->basicGpRegisters[(opcode & 0x0038) >> 3] = z80Cpu->ram[z80Cpu->spRegisters16[IY] + d];
		printf("LD_R_IY_D\n");
		break;
	}
	case LD_R_IX_D://test signed d
	{
		char d = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
		z80Cpu->basicGpRegisters[(opcode & 0x0038) >> 3] = z80Cpu->ram[z80Cpu->spRegisters16[IX] + d];
		printf("LD_R_IX_D\n");
		break;
	}
	case BIT_B_R:
	{
		z80Cpu->basicGpRegisters[opcode & 0b111] &
		(1 << (opcode & 0b111000)) ?
			RESET_CONDITION_BIT(CB_Z) :
			SET_CONDITION_BIT(CB_Z);

		SET_CONDITION_BIT(CB_H);
		RESET_CONDITION_BIT(CB_N);
		printf("BIT_B_R\n");
		break;
	}
	default:
		z80Cpu->running = false;
		printf("UNKNOWN OP\n");
		break;
	}
}

void swap(unsigned short* leftPtr, unsigned short* rightPtr)
{
	unsigned short temp = *leftPtr;
	*leftPtr = *rightPtr;
	*rightPtr = temp;
}
