#include "dbg.h"

void registersDump(Z80Cpu* z80Cpu) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	static unsigned char previous8[10] = { 0 };
	static unsigned short previous16[4] = { 0 };

	unsigned char current8[10] =
	{
		z80Cpu->basicGpRegisters[B], z80Cpu->basicGpRegisters[C],
		z80Cpu->basicGpRegisters[D], z80Cpu->basicGpRegisters[E],
		z80Cpu->basicGpRegisters[H], z80Cpu->basicGpRegisters[L],
		z80Cpu->basicGpRegisters[F], z80Cpu->basicGpRegisters[A],
		z80Cpu->spRegisters8[I],z80Cpu->spRegisters8[R]
	};
	unsigned short current16[4] =
	{
		z80Cpu->spRegisters16[IX],z80Cpu->spRegisters16[IY],
		z80Cpu->spRegisters16[SP],z80Cpu->spRegisters16[PC]
	};

	unsigned char changed8[10] =
	{
	current8[0] - previous8[0],
	current8[1] - previous8[1],
	current8[2] - previous8[2],
	current8[3] - previous8[3],
	current8[4] - previous8[4],
	current8[5] - previous8[5],
	current8[6] - previous8[6],
	current8[7] - previous8[7],
	current8[8] - previous8[8],
	current8[9] - previous8[9]
	};
	unsigned short changed16[4] =
	{
	current16[0] - previous16[0],
	current16[1] - previous16[1],
	current16[2] - previous16[2],
	current16[3] - previous16[3]
	};

	static char basicMnemonics[10] = { 'B','C','D','E','H','L','F','A','I','R' };
	static const char* spMnemonics[4] = { "IX","IY","SP","PC" };

	for (size_t i = 0; i < 10; i++)
	{
		if (changed8[i]) {
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
		}
		else {
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
		}
		if (i % 2 == 0) {
			printf("\n\n");
		}
		printf("%c = 0x%02X \t ", basicMnemonics[i], current8[i]);
	}
	for (size_t i = 0; i < 4; i++)
	{
		if (changed16[i]) {
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
		}
		else {
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
		}
		if (i % 2 == 0) {
			printf("\n\n");
		}
		printf("%s = 0x%04X \t ", spMnemonics[i], current16[i]);
	}

	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
	printf("\n\n\n");
	memcpy(previous8, current8, 10);
	memcpy(previous16, current16, 8);
}