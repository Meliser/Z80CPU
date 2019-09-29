
#include "Z80CPU.h"
#include "dbg.h"


unsigned char programm[] =
{
	
	0b00100110,		//LD_R_N
	0b10000000,

	0b00111110,		//LD_R_N
	0b10000000,

	0b10000100,		//ADD_A_R

	0b00000000,		//NOP

	0b00100001,		//LD_DD_NN
	0b11111111,
	0b10101010,

	0b01110001,		//LD_HL_R

	0b00010110,		//LD_R_N
	0b00101000,

	0b00011110,		//LD_R_N
	0b00100010,

	0b11101011,		//EX_DE_HL

	0b11011101,		//LD_R_IX_D
	0b01101110,
	0b00000100,

	0b11111101,		//LD_R_IY_D
	0b01101110,
	0b00000001,

	0b01101010,		//LD_R1_R2

	0b01111110,		//LD_R_HL

	0b00110110,		//LD_HL_N
	0b01110101,		

	0b00100110,		//LD_R_N
	0b10001111,

	0b00000000,		//NOP
};
const size_t programmSize = sizeof(programm) / sizeof(programm[0]);

int main()
{
	
	Z80Cpu* z80Cpu = new Z80Cpu{ 0 };
	init(z80Cpu);
	
	memcpy(z80Cpu->ram, programm, programmSize);

	while (z80Cpu->running) {
		execute(z80Cpu);
		registersDump(z80Cpu);
	}
	
	delete z80Cpu;
	return 0;
}

