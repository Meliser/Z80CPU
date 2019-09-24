#include "dbg.h"

void registersDump(Z80Cpu* z80Cpu) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	static unsigned char previous8[10] = { 0 };
	static unsigned short previous16[4] = { 0 };
	//remove current
	/*unsigned char current8[10] =
	{
		z80Cpu->basicGpRegisters[B],	z80Cpu->basicGpRegisters[C],
		z80Cpu->basicGpRegisters[D],	z80Cpu->basicGpRegisters[E],
		z80Cpu->basicGpRegisters[H],	z80Cpu->basicGpRegisters[L],
		z80Cpu->basicGpRegisters[F],	z80Cpu->basicGpRegisters[A],
		z80Cpu->spRegisters8[I],		z80Cpu->spRegisters8[R]
	};*/
	/*unsigned short current16[4] =
	{
		z80Cpu->spRegisters16[IX],z80Cpu->spRegisters16[IY],
		z80Cpu->spRegisters16[SP],z80Cpu->spRegisters16[PC]
	};*/

	unsigned char changed8[10] =
	{
	z80Cpu->basicGpRegisters[B] ^ previous8[B],
	z80Cpu->basicGpRegisters[C] ^ previous8[C],
	z80Cpu->basicGpRegisters[D] ^ previous8[D],
	z80Cpu->basicGpRegisters[E] ^ previous8[E],
	z80Cpu->basicGpRegisters[H] ^ previous8[H],
	z80Cpu->basicGpRegisters[L] ^ previous8[L],
	z80Cpu->basicGpRegisters[F] ^ previous8[F],
	z80Cpu->basicGpRegisters[A] ^ previous8[A],
	z80Cpu->spRegisters8[I] ^ previous8[I],
	z80Cpu->spRegisters8[R] ^ previous8[R]
	};
	unsigned short changed16[4] =
	{
	z80Cpu->spRegisters16[IX] ^ previous16[IX],
	z80Cpu->spRegisters16[IY] ^ previous16[IY],
	z80Cpu->spRegisters16[SP] ^ previous16[SP],
	z80Cpu->spRegisters16[PC] ^ previous16[PC]
	};

	static char gpMnem[6] = { 'B','C','D','E','H','L' };
	static char gpAddMnem[6] = { 'B\'','C\'','D\'','E\'','H\'','L\'' };
	WORD color;
	for (size_t i = 0; i < 6; i++)
	{
		color = changed8[i] ? GREEN : GREY;
		SetConsoleTextAttribute(hConsole, color);
		if(i % 2 == 0) {
			printf("\n\n");
		}
		printf("%c = 0x%02X \t", gpMnem[i], z80Cpu->basicGpRegisters[i]);
	}
	printf("\n\n");

	static const char* flagMnem[8] = { "C","N","P/V","-","H","-","Z","S" };
	for (size_t i = 0; i < 8; i++)
	{
		color = (changed8[F] >> i) & 1 ? GREEN : GREY;
		SetConsoleTextAttribute(hConsole, color);
		printf("%-3s = %d \n", flagMnem[i], (z80Cpu->basicGpRegisters[F] >> i) & 1);
	}
	printf("\n");

	
	color = changed8[A] ? GREEN : GREY;
	SetConsoleTextAttribute(hConsole, color);
	printf("A = 0x%02X \t", z80Cpu->basicGpRegisters[A]);

	
	color = changed8[I] ? GREEN : GREY;
	SetConsoleTextAttribute(hConsole, color);
	printf("I = 0x%02X \t", z80Cpu->basicGpRegisters[I]);

	
	color = changed8[R] ? GREEN : GREY;
	SetConsoleTextAttribute(hConsole, color);
	printf("R = 0x%02X", z80Cpu->basicGpRegisters[R]);

	static const char* spMnem[4] = { "IX","IY","SP","PC" };
	for (size_t i = 0; i < 4; i++)
	{
		
		color = changed16[i] ? GREEN : GREY;
		SetConsoleTextAttribute(hConsole, color);
		if (i % 2 == 0) {
			printf("\n\n");
		}
		printf("%s = 0x%04X \t", spMnem[i], z80Cpu->spRegisters16[i]);
	}

	SetConsoleTextAttribute(hConsole, (WORD)GREY);
	printf("\n\n\n");
	memcpy(previous8, z80Cpu->basicGpRegisters, 8);
	memcpy(previous8+8, z80Cpu->spRegisters8, 2);
	memcpy(previous16, z80Cpu->spRegisters16, 8);
}
