#include "Z80CPU.h"

//available 8 bit opcodes
static unsigned char opcodes00[] =
{
	//00
	0x36,//LD_HL_N
	0x06,//LD_R_N
	0x01,//LD_DD_NN
	0x00, //NOP
};
static unsigned char opcodes01[] =
{
	//01
	0x70,//LD_HL_R
	0x46,//LD_R_HL
	0x40,//LD_R1_R2
};
static unsigned char opcodes10[1] =
{
	0xBF // for test
};
static unsigned char opcodes11[] =
{
	//11
	0xEB,//EX_DE_HL
};
unsigned char* opcodes8[] =
{
	opcodes00,opcodes01,opcodes10,opcodes11
};
const size_t opcodesSizes[4] =
{	
	sizeof(opcodes00) / sizeof(opcodes00[0]),
	sizeof(opcodes01) / sizeof(opcodes01[0]),
	sizeof(opcodes10) / sizeof(opcodes10[0]),
	sizeof(opcodes11) / sizeof(opcodes11[0]),
};
const size_t opcodesOffsets[5] =
{
	0,
	opcodesSizes[0],
	opcodesSizes[0] + opcodesSizes[1],
	opcodesSizes[0] + opcodesSizes[1] + opcodesSizes[2],
	opcodesSizes[0] + opcodesSizes[1] + opcodesSizes[2] + opcodesSizes[3]
};
//available 16 bit opcodes
static unsigned short opcodes16[] =
{
	0b1111110101000110,//LD_R_IY_D
	0b1101110101000110, //LD_R_IX_D
};
const size_t opcodes16Size = sizeof(opcodes16) / sizeof(opcodes16[0]);

void init(Z80Cpu* z80Cpu)
{
	assert(!(	opcodesSizes[0] +
				opcodesSizes[1] + 
				opcodesSizes[2] +
				opcodesSizes[3] +
				opcodes16Size -
				OPCODES_SIZE));

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

 size_t evaluate(unsigned char opcode,bool &success) {
	static const size_t masksSize = 2;
	static unsigned char masks[masksSize] =
	{
		0b00111000,
		0b00000111
	};
	unsigned char block = opcode >> 6;
	unsigned char* currentBlock = opcodes8[block];
	size_t j = 0;
	for (size_t i = 0; i < masksSize; i++)
	{
		for (; j < opcodesSizes[block]; j++)
		{
			if (((currentBlock[j] & masks[i]) == 0) or (opcode & masks[i]) == (currentBlock[j] & masks[i]))
			{
				break;
			}
		}
	}
	if (j == opcodesSizes[block]) {
		success = false;
	}
	return j + opcodesOffsets[block];
}

 size_t evaluate16(unsigned short opcode) {
	static const size_t masksSize = 6;
	//optimize masks
	static unsigned short masks[masksSize] =
	{
		0b1100000000000000,
		0b0011100000000000,
		0b0000011100000000,
		0b0000000011000000,
		0b0000000000111000,
		0b0000000000000111
	};

	size_t j = 0;
	for (size_t i = 0; i < masksSize; i++)
	{
		for (; j < opcodes16Size; j++)
		{
			if (((opcodes16[j] & masks[i]) == 0) or (opcode & masks[i]) == (opcodes16[j] & masks[i]))
			{
				break;
			}
		}
	}

	return j + opcodesOffsets[4];
}

void execute(Z80Cpu* z80Cpu) {
	unsigned char opcode8 = fetch(z80Cpu);
	bool success = true;
	size_t index = evaluate(opcode8, success);

	unsigned short opcode16 = 0;
	if (!success) {
		opcode16 = (opcode8 << 8) | fetch(z80Cpu);
		index = evaluate16(opcode16);
	}

	switch (index)
	{
	case EX_DE_HL:
	{
		swap(&z80Cpu->DE, &z80Cpu->HL);
		printf("EX_DE_HL\n");
		break;
	}
	case LD_HL_R:
	{
		unsigned short HL = (z80Cpu->basicGpRegisters[H] << 8) | z80Cpu->basicGpRegisters[L];
		z80Cpu->ram[HL] = z80Cpu->basicGpRegisters[opcode8 & 0b00000111];
		printf("LD_HL_R\n");
		break;
	}
	case LD_R_HL:
	{
		unsigned short HL = (z80Cpu->basicGpRegisters[H] << 8) | z80Cpu->basicGpRegisters[L];
		z80Cpu->basicGpRegisters[(opcode8 & 0b00111000) >> 3] = z80Cpu->ram[HL];
		printf("LD_R_HL\n");
		break;
	}
	case LD_R1_R2:
		z80Cpu->basicGpRegisters[(opcode8 & 0b00111000) >> 3] = z80Cpu->basicGpRegisters[opcode8 & 0b00000111];
		printf("LD_R1_R2\n");
		break;
	case LD_HL_N:
	{
		unsigned short HL = (z80Cpu->basicGpRegisters[H] << 8) | z80Cpu->basicGpRegisters[L];
		z80Cpu->ram[HL] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
		printf("LD_HL_N\n");
		break;
	}
	case LD_R_N:
		z80Cpu->basicGpRegisters[opcode8 >> 3] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
		printf("LD_R_N\n");
		break;
	case LD_DD_NN:
	{
		unsigned char dd = (opcode8 & 0x30) >> 4;
		switch (dd)//make function
		{
		case 0b00:
			z80Cpu->basicGpRegisters[B] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
			z80Cpu->basicGpRegisters[C] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
			break;
		case 0b01:
			z80Cpu->basicGpRegisters[D] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
			z80Cpu->basicGpRegisters[E] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
			break;
		case 0b10:
			z80Cpu->basicGpRegisters[H] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
			z80Cpu->basicGpRegisters[L] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
			break;
		case 0b11:
			z80Cpu->spRegisters16[SP] = z80Cpu->ram[z80Cpu->spRegisters16[PC]++] << 8;
			z80Cpu->spRegisters16[SP] |= z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
			break;
		default:
			printf("LD_DD_NN OPCODE ERROR\n");
			break;
		}
		printf("LD_DD_NN\n");
		break;
	}
	case NOP:
		z80Cpu->running = false;
		printf("NOP\n");
		break;
	case LD_R_IY_D://test signed d
	{
		char d = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
		z80Cpu->basicGpRegisters[(opcode16 & 0x0038) >> 3] = z80Cpu->ram[z80Cpu->spRegisters16[IY] + d];
		printf("LD_R_IY_D\n");
		break;
	}
	case LD_R_IX_D://test signed d
	{
		char d = z80Cpu->ram[z80Cpu->spRegisters16[PC]++];
		z80Cpu->basicGpRegisters[(opcode16 & 0x0038) >> 3] = z80Cpu->ram[z80Cpu->spRegisters16[IX] + d];
		printf("LD_R_IX_D\n");
		break;
	}
	default:
		z80Cpu->running = false;
		printf("UNKNOWN OP\n");
		break;
	}
}

void swap(unsigned short** leftPtr, unsigned short** rightPtr)
{
	unsigned short* temp = *leftPtr;
	*leftPtr = *rightPtr;
	*rightPtr = temp;
}