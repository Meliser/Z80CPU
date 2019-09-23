
#include "Z80CPU.h"
#include "dbg.h"


unsigned char programm[] =
{
	0b00100001,		//LD_DD_NN
	0b11111111,
	0b10101010,

	0b01110001,		//LD_HL_R

	0xC3,			//JP_NN
	23,
	0b00000000,

	0b00000000,		//NOP

	0b00100110,		//LD_R_N
	0b01001001,

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

};
const size_t programmSize = sizeof(programm) / sizeof(programm[0]);

int main()
{
	Z80Cpu* z80Cpu = new Z80Cpu{ 0 };
	init(z80Cpu);
	
	/**z80Cpu->DE = 0xffee;
	*z80Cpu->HL = 0xccdd;

	printf("%X\n", *z80Cpu->DE);
	swap(&z80Cpu->DE, &z80Cpu->HL);
	printf("%X", *z80Cpu->DE);*/
	
	memcpy(z80Cpu->ram, programm, programmSize);

	while (z80Cpu->running) {
		execute(z80Cpu);
		registersDump(z80Cpu);
	}
	
	delete z80Cpu;
	return 0;
}

