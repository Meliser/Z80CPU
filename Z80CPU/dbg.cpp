#include "dbg.h"

void registersDump(Z80Cpu* z80Cpu) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	static unsigned char previousGp[GP_REGISTERS_SIZE] = { 0 };
	static unsigned char previousAddGp[GP_REGISTERS_SIZE] = { 0 };
	static unsigned char previousSp8[SP_REGISTERS8_SIZE] = { 0 };
	static unsigned short previousSp16[SP_REGISTERS16_SIZE] = { 0 };
	
	unsigned char changedGp[GP_REGISTERS_SIZE] =
	{
		z80Cpu->basicGpRegisters[B] ^ previousGp[B],
		z80Cpu->basicGpRegisters[C] ^ previousGp[C],
		z80Cpu->basicGpRegisters[D] ^ previousGp[D],
		z80Cpu->basicGpRegisters[E] ^ previousGp[E],
		z80Cpu->basicGpRegisters[H] ^ previousGp[H],
		z80Cpu->basicGpRegisters[L] ^ previousGp[L],
		z80Cpu->basicGpRegisters[F] ^ previousGp[F],
		z80Cpu->basicGpRegisters[A] ^ previousGp[A],
	};
	unsigned char changedAddGp[GP_REGISTERS_SIZE] =
	{
		z80Cpu->additionalGpRegisters[B] ^ previousAddGp[B],
		z80Cpu->additionalGpRegisters[C] ^ previousAddGp[C],
		z80Cpu->additionalGpRegisters[D] ^ previousAddGp[D],
		z80Cpu->additionalGpRegisters[E] ^ previousAddGp[E],
		z80Cpu->additionalGpRegisters[H] ^ previousAddGp[H],
		z80Cpu->additionalGpRegisters[L] ^ previousAddGp[L],
		z80Cpu->additionalGpRegisters[F] ^ previousAddGp[F],
		z80Cpu->additionalGpRegisters[A] ^ previousAddGp[A],
	};
	unsigned char changedSp8[SP_REGISTERS8_SIZE] =
	{
		z80Cpu->spRegisters8[I] ^ previousSp8[I],
		z80Cpu->spRegisters8[R] ^ previousSp8[R]
	};
	unsigned short changedSp16[SP_REGISTERS16_SIZE] =
	{
	z80Cpu->spRegisters16[IX] ^ previousSp16[IX],
	z80Cpu->spRegisters16[IY] ^ previousSp16[IY],
	z80Cpu->spRegisters16[SP] ^ previousSp16[SP],
	z80Cpu->spRegisters16[PC] ^ previousSp16[PC]
	};

	static char gpMnem[GP_REGISTERS_SIZE-2] = { 'B','C','D','E','H','L' };

	WORD color;
	printf("\n\n");
	for (size_t i = 0; i < GP_REGISTERS_SIZE-2; i++)
	{
		color = changedGp[i] ? GREEN : GREY;
		SetConsoleTextAttribute(hConsole, color);
		printf("%c = 0x%02X \t", gpMnem[i], z80Cpu->basicGpRegisters[i]);
		if(i % 2 == 1) {
			for (size_t j = 0; j < 2; j++)
			{
				color = changedAddGp[i] ? GREEN : GREY;
				SetConsoleTextAttribute(hConsole, color);
				printf("%c' = 0x%02X \t", gpMnem[i], z80Cpu->additionalGpRegisters[i]);
			}
			printf("\n\n");
		}
	}

	static const char* flagMnem[8] = { "C","N","P/V","-","H","-","Z","S" };
	for (size_t i = 0; i < 8; i++)
	{
		color = (changedGp[F] >> i) & 1 ? GREEN : GREY;
		SetConsoleTextAttribute(hConsole, color);
		printf("%-3s = %d \t\t\t", flagMnem[i], (z80Cpu->basicGpRegisters[F] >> i) & 1);

		color = (changedAddGp[F] >> i) & 1 ? GREEN : GREY;
		SetConsoleTextAttribute(hConsole, color);
		printf("%-3s = %d \n", flagMnem[i], (z80Cpu->additionalGpRegisters[F] >> i) & 1);
	}
	printf("\n");

	color = changedGp[A] ? GREEN : GREY;
	SetConsoleTextAttribute(hConsole, color);
	printf("A = 0x%02X \t\t\t", z80Cpu->basicGpRegisters[A]);
	color = changedAddGp[A] ? GREEN : GREY;
	SetConsoleTextAttribute(hConsole, color);
	printf("A' = 0x%02X \n\n", z80Cpu->additionalGpRegisters[A]);

	color = changedSp8[I] ? GREEN : GREY;
	SetConsoleTextAttribute(hConsole, color);
	printf("I = 0x%02X \t", z80Cpu->spRegisters8[I]);

	color = changedSp8[R] ? GREEN : GREY;
	SetConsoleTextAttribute(hConsole, color);
	printf("R = 0x%02X", z80Cpu->spRegisters8[R]);

	static const char* spMnem[SP_REGISTERS16_SIZE] = { "IX","IY","SP","PC" };
	for (size_t i = 0; i < SP_REGISTERS16_SIZE; i++)
	{
		
		color = changedSp16[i] ? GREEN : GREY;
		SetConsoleTextAttribute(hConsole, color);
		if (i % 2 == 0) {
			printf("\n\n");
		}
		printf("%s = 0x%04X \t", spMnem[i], z80Cpu->spRegisters16[i]);
	}

	SetConsoleTextAttribute(hConsole, (WORD)GREY);
	printf("\n\n\n");

	memcpy(previousGp, z80Cpu->basicGpRegisters, GP_REGISTERS_SIZE);
	memcpy(previousAddGp, z80Cpu->additionalGpRegisters, GP_REGISTERS_SIZE);
	memcpy(previousSp8, z80Cpu->spRegisters8, SP_REGISTERS8_SIZE);
	memcpy(previousSp16, z80Cpu->spRegisters16, sizeof(unsigned short)* SP_REGISTERS16_SIZE);
}
