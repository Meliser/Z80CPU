﻿#pragma once
#include <Windows.h>
#include <stdio.h>

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

enum OPCODES
{
	//8 bit opcodes
	LD_HL_R,
	LD_R_HL,
	LD_R1_R2,
	LD_HL_N,
	LD_R_N,
	LD_DD_NN,
	NOP,
	//16 bit opcodes
	LD_R_IY_D,
	LD_R_IX_D
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

	bool running;
};

 unsigned char fetch(Z80Cpu* z80Cpu);

 size_t evaluate(unsigned char opcode);

 size_t evaluate16(unsigned short opcode);

 void execute(Z80Cpu* z80Cpu);

 void swap(unsigned short** leftPtr, unsigned short** rightPtr);








