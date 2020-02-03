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
	0xDB, //IN_A_N
	//EXTEND
	0xFD,
	0xED,
	0xDD,
	0xCB
};
const size_t opcodes8Size = sizeof(opcodes8) / sizeof(opcodes8[0]);

//available 16 bit opcodes

static unsigned short opcodes16[] =
{
	0b1111110101000110, //LD_R_IY_D
	0b1110110101001101, //RETI
	0b1101110101000110, //LD_R_IX_D
	0b1100101101000000, //BIT_B_R
};
const size_t opcodes16Size = sizeof(opcodes16) / sizeof(opcodes16[0]);

void init(Z80Cpu* z80Cpu)
{
	assert(opcodes8Size == OPCODES8_SIZE + 4);
	assert(opcodes16Size == OPCODES16_SIZE);

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

	if (!search(opcode, opcodes8, opcodes8Size, index))
	{
		evaluate(opcode, opcodes8, opcodes8Size, index);
	}
	//decode 8 bit opcode, add function
	switch (index)
	{
	case LD_HL_N:
	{
		z80Cpu->ram[*z80Cpu->HL] = fetch(z80Cpu);
		printf("LD_HL_N\n");
		return;
	}
	case LD_R_N:
		z80Cpu->basicGpRegisters[opcode >> 3] = fetch(z80Cpu);
		printf("LD_R_N\n");
		return;
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
		return;
	}
	case NOP:
		z80Cpu->running = false;
		printf("NOP\n");
		return;
	case LD_HL_R:
	{
		z80Cpu->ram[*z80Cpu->HL] = z80Cpu->basicGpRegisters[opcode & 0b00000111];
		printf("LD_HL_R\n");
		return;
	}
	case LD_R_HL:
	{
		z80Cpu->basicGpRegisters[(opcode & 0b00111000) >> 3] = z80Cpu->ram[*z80Cpu->HL];
		printf("LD_R_HL\n");
		return;
	}
	case LD_R1_R2:
		z80Cpu->basicGpRegisters[(opcode & 0b00111000) >> 3] = z80Cpu->basicGpRegisters[opcode & 0b00000111];
		printf("LD_R1_R2\n");
		return;
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
		return;
	}
	case EX_DE_HL:
	{
		swap(z80Cpu->DE,z80Cpu->HL);
		printf("EX_DE_HL\n");
		return;
	}
	case CALL_NN:
		z80Cpu->spRegisters16[SP]-=2;
		*(unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[SP]) = z80Cpu->spRegisters16[PC] + 2;
		z80Cpu->spRegisters16[PC] = *((unsigned short*)(z80Cpu->ram+z80Cpu->spRegisters16[PC]));
		printf("CALL_NN\n");
		return;
	case RET:
		z80Cpu->spRegisters16[PC] = *((unsigned short*)(z80Cpu->ram+z80Cpu->spRegisters16[SP]));
		z80Cpu->spRegisters16[SP] += 2;
		printf("RET\n");
		return;
	case JP_NN:
		z80Cpu->spRegisters16[PC] = *((unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[PC]));
		printf("JP_NN\n");
		return;
	case IN_A_N:
	{
		// accumulator value should be the index of buffer
		
		unsigned char port = fetch(z80Cpu);
		//OVERLAPPEDPLUS ovlp = z80Cpu->ioController.getPort(port)->getOvlp();
		
		z80Cpu->basicGpRegisters[A] = z80Cpu->ioController.getPort(port)->getOvlp().getBuffer()[z80Cpu->basicGpRegisters[A]];
		printf("IN_A_N\n");
		return;
	}
	default:
		opcode = (opcode << 8) | fetch(z80Cpu);
		index = 0;
		if (!search(opcode, opcodes16, opcodes16Size, index))
		{
			evaluate(opcode, opcodes16, opcodes16Size, index);
		}
		printf("EXTENDED OP\n");
	}
	switch (index)
	{
	case LD_R_IY_D://test signed d
	{
		char d = fetch(z80Cpu);
		z80Cpu->basicGpRegisters[(opcode & 0x0038) >> 3] = z80Cpu->ram[z80Cpu->spRegisters16[IY] + d];
		printf("LD_R_IY_D\n");
		return;
	}
	case RETI:
		z80Cpu->busyPort->restart();
		z80Cpu->busyPort = nullptr;
		z80Cpu->spRegisters16[PC] = *((unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[SP]));
		z80Cpu->spRegisters16[SP] += 2;
		printf("RETI\n");
		return;
	case LD_R_IX_D://test signed d
	{
		char d = fetch(z80Cpu);
		z80Cpu->basicGpRegisters[(opcode & 0x0038) >> 3] = z80Cpu->ram[z80Cpu->spRegisters16[IX] + d];
		printf("LD_R_IX_D\n");
		return;
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
		return;
	}
	default:
		z80Cpu->running = false;
		printf("UNKNOWN OP\n");
		return;
	}
	
}
//replace with stl
void swap(unsigned short* leftPtr, unsigned short* rightPtr)
{
	unsigned short temp = *leftPtr;
	*leftPtr = *rightPtr;
	*rightPtr = temp;
}
