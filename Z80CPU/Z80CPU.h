#pragma once

#include <stdio.h>
#include <assert.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include "IOController.h"
using namespace std;
//general purpose registers
enum GP_REGISTERS : unsigned char
{
	B, C, D, E, H, L, F, A, GP_REGISTERS_SIZE
};
//special purpose 8 bit registers
enum SP_REGISTERS8 : unsigned char
{
	I, R, SP_REGISTERS8_SIZE
};
//special purpose 16 bit registers
enum SP_REGISTERS16 : unsigned char
{
	IX, IY, SP, PC, SP_REGISTERS16_SIZE
};
enum CONDITION_BITS
{
	CB_C, CB_N, CB_PV, NOT_USED1, CB_H, NOT_USED2, CB_Z, CB_S
};
#define SET_CONDITION_BIT(bit) (z80Cpu->basicGpRegisters[F] |=  (1 << bit))
#define RESET_CONDITION_BIT(bit) (z80Cpu->basicGpRegisters[F] &= ~(1 << bit))

//8 bit opcodes
enum OPCODES8
{
	//00
	LD_HL_N,
	JR_E, //test
	LD_R_N,
	LD_DD_NN,
	NOP,
	//01
	LD_HL_R,
	LD_R_HL,
	LD_R1_R2,
	//10
	ADD_A_R,
	//11
	EX_DE_HL,
	CALL_NN,
	RET,
	JP_NN,
	IN_A_N,

	OPCODES8_SIZE
};

//16 bit opcodes
enum OPCODES16
{
LD_R_IY_D,
RETI,
LD_R_IX_D,
BIT_B_R,

OPCODES16_SIZE
};


struct Z80Cpu
{
	unsigned char ram[0x10000];

	unsigned char basicGpRegisters[GP_REGISTERS_SIZE];
	unsigned char additionalGpRegisters[GP_REGISTERS_SIZE];

	unsigned char spRegisters8[SP_REGISTERS8_SIZE];
	unsigned short spRegisters16[SP_REGISTERS16_SIZE];

	unsigned short* BC;
	unsigned short* DE;
	unsigned short* HL;
	unsigned short* AF;

	unsigned short* ADDITIONAL_BC;
	unsigned short* ADDITIONAL_DE;
	unsigned short* ADDITIONAL_HL;
	unsigned short* ADDITIONAL_AF;

	IOController ioController;
	IPort* busyPort;

	bool running;
};

void init(Z80Cpu* z80Cpu);

unsigned char fetch(Z80Cpu* z80Cpu);

template<typename T>
bool search(unsigned int opcode, T* table, size_t tableSize, size_t& index) {
	size_t temp = index;
	for (size_t i = 0; i < tableSize; i++,index++)
	{
		if (opcode == table[i]) {
			//index += i; ???
			return true;
		}
	}
	index = temp;
	return false;
}
template<typename T>
bool evaluate(unsigned int opcode, T* table, size_t tableSize, size_t& index) {
	//search by masks
	static unsigned short masks[3] =
	{
		0xffc7,// (1)11000111
		0xfff8,// (1)11111000
		0xffc0,// (1)11000000
	};
	unsigned short currentTemplate;
	//size_t temp = index;
	for (size_t i = 0; i < 3; i++)
	{
		currentTemplate = opcode & masks[i];
		index = 0;
		for (size_t j = 0; j < tableSize; j++, index++)
		{
			if (currentTemplate == table[j])
			{
				return true;
			}
		}
	}
	return false;
}

void execute(Z80Cpu* z80Cpu);

void swap(unsigned short* leftPtr, unsigned short* rightPtr);


static void checkPorts(Z80Cpu* z80Cpu) {
	
	//check priority
	//save registers
	//handle interrupt;
	//restore registers
	auto ports = z80Cpu->ioController.getPorts();
	
	for (auto port : ports) {
		if (port->getOvlp().getStatus()) {
			z80Cpu->busyPort = port;
			port->getOvlp().setStatus(false);
			z80Cpu->spRegisters16[SP] -= 2;
			*(unsigned short*)(z80Cpu->ram + z80Cpu->spRegisters16[SP]) = z80Cpu->spRegisters16[PC];
			z80Cpu->spRegisters16[PC] = 0xeeee;
		}
	}
}








