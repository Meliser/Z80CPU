#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <conio.h>
#include <stdio.h>

#include "Z80CPU.h"
#include "dbg.h"

using namespace boost::interprocess;
using namespace std;
unsigned char programm[] =
{
	0b01110001,		//LD_HL_R

	0b00100110,		//LD_R_N
	0b10000000,

	0b11001011,		//BIT_B_R
	0b01111100,

	0b11011101,		//LD_R_IX_D
	0b01101110,
	0b00000101,

	0b11111101,		//LD_R_IY_D
	0b01101110,
	0b00000001,

	0b00111110,		//LD_R_N
	0b10000000,

	0b10000100,		//ADD_A_R

	0b00000000,		//NOP

	0b00100001,		//LD_DD_NN
	0b11111111,
	0b10101010,

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

// need RETI opcode
unsigned char kbdIntHandler[] =
{
	0b11011011,		//IN_A_N
	0b00000000,
	
	0b11001001,		//RET
};
const size_t kbdIntHandlerSize = sizeof(kbdIntHandler) / sizeof(kbdIntHandler[0]);



int main()
{
	
	Z80Cpu* z80Cpu = new Z80Cpu{ 0 };
	init(z80Cpu);
	memcpy(z80Cpu->ram, programm, programmSize);
	memcpy(z80Cpu->ram + 0xeeee, kbdIntHandler, kbdIntHandlerSize);

	/*file_mapping m_file("programm.obj", read_write);
	mapped_region region(m_file, read_write);
	const void* addr = region.get_address();
	std::size_t size = region.get_size();

	file_mapping m_file1("func.obj", read_write);
	mapped_region region1(m_file1, read_write);
	const void* addr1 = region1.get_address();
	std::size_t size1 = region1.get_size();

	memcpy(z80Cpu->ram, addr, size);
	memcpy(z80Cpu->ram+0xaaff, addr1, size1);*/
	
	IPort* stdIn = new StdIn;
	stdIn->init();
	z80Cpu->ioController.attachPort(stdIn);
	while (z80Cpu->running) {
		execute(z80Cpu);
		checkPorts(z80Cpu);
		registersDump(z80Cpu);
	}

	delete z80Cpu;
	//close ports
	delete stdIn;
	
	return 0;
}

