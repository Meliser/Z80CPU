
#include "Z80CPU.h"
#include "dbg.h"

const size_t programmSize = 17;
unsigned char programm[programmSize] =
{
	0b00110110,		//LD_HL_N
	0b01110101,		

	0b00100110,		//LD_R_N
	0b10001111,

	0b00010001,		//LD_DD_NN
	0b11110000,		
	0b10101110,

	0b11111101,		//LD_R_IY_D
	0b01101110,
	0b00000001,

	0b11011101,		//LD_R_IX_D
	0b01101110,
	0b00000100,

	0b00000000,		//NOP	

	0b00010110,		//LD_R_N
	0b01101010,		//LD_R1_R2
	0b01111110,		//LD_R_HL
	
};

int main()
{
	Z80Cpu* z80Cpu = new Z80Cpu{ 0 };

	z80Cpu->BC = (unsigned short*)(z80Cpu->basicGpRegisters + B);
	z80Cpu->DE = (unsigned short*)(z80Cpu->basicGpRegisters + D);
	z80Cpu->HL = (unsigned short*)(z80Cpu->basicGpRegisters + H);
	z80Cpu->AF = (unsigned short*)(z80Cpu->basicGpRegisters + F);
	z80Cpu->ADDITIONAL_BC = (unsigned short*)(z80Cpu->additionalGpRegisters + B);
	z80Cpu->ADDITIONAL_DE = (unsigned short*)(z80Cpu->additionalGpRegisters + D);
	z80Cpu->ADDITIONAL_HL = (unsigned short*)(z80Cpu->additionalGpRegisters + H);
	z80Cpu->ADDITIONAL_AF = (unsigned short*)(z80Cpu->additionalGpRegisters + F);
	

	z80Cpu->running = true;

	*z80Cpu->DE = 0xffee;
	*z80Cpu->HL = 0xccdd;

	printf("%X\n", *z80Cpu->DE);
	swap(&z80Cpu->DE, &z80Cpu->HL);
	printf("%X", *z80Cpu->DE);

	memcpy(z80Cpu->ram, programm, programmSize);

	while (z80Cpu->running) {
		execute(z80Cpu);
		registersDump(z80Cpu);
	}

	delete z80Cpu;
	return 0;
}

