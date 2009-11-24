/*
 *  disasm.cpp
 *  smcdis
 *
 *  Created by Alexander Strange on 12/25/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "disasm.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum addressing_mode
{
	None,
	Interrupt,
	Immediate,
	Absolute,
	AbsLong,
	AbsIndirect,
	AbsIndexed,
	AbsLongIndexed,
	AbsIndexedIndirect,
	AbsIndirectLong,
	DirectPage,
	DPIndexed,
	DPIndexedIndirect,
	DPIndirect,
	DPIndirectIndexed,
	DPIndirectLong,
	DPIndirectLongIndexed,
	PCRelative,
	PCRelativeLong,
	StackPush,
	StackPull,
	StackRelative,
	StackRelativeIndirectIndexed,
	BlockMove,
	WDM
} addressing_mode;

typedef enum instruction_name
{
	I_ADC, I_AND, I_ASL, I_BCC, I_BCS, I_BEQ, I_BIT, I_BMI, I_BNE,
	I_BPL, I_BRA, I_BRK, I_BRL, I_BVC, I_BVS, I_CLC, I_CLD, I_CLI,
	I_CLP, I_CLV, I_CMP, I_CPX, I_CPY, I_COP, I_DEC, I_DEX, I_DEY,
	I_EOR, I_INC, I_INX, I_INY, I_JMP, I_JSR, I_LDA, I_LDX, I_LDY,
	I_LSR, I_MVN, I_MVP, I_NOP, I_ORA, I_PEA, I_PEI, I_PER, I_PHA,
	I_PHB, I_PHD, I_PHK, I_PHP, I_PHX, I_PHY, I_PLA, I_PLB, I_PLD,
	I_PLP, I_PLX, I_PLY, I_RET, I_REP, I_ROL, I_ROR, I_RTI, I_RTL,
	I_RTS, I_SBC, I_SEC, I_SED, I_SEI, I_SEP, I_SHL, I_SHR, I_STA,
	I_STX, I_STY, I_STP, I_STZ, I_SWA, I_TAD, I_TAS, I_TAX, I_TAY,
	I_TCB, I_TCD, I_TCS, I_TDC, I_TRB, I_TSA, I_TSB, I_TSC, I_TSX,
	I_TXA, I_TXS, I_TXY, I_TYA, I_TYX, I_WAI, I_WDM, I_XBA, I_XCE
} instruction_name;

static const char *opcode_names[] = 
{
	"ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE",
	"BPL", "BRA", "BRK", "BRL", "BVC", "BVS", "CLC", "CLD", "CLI",
	"CLP", "CLV", "CMP", "CPX", "CPY", "COP", "DEC", "DEX", "DEY",
	"EOR", "INC", "INX", "INY", "JMP", "JSR", "LDA", "LDX", "LDY",
	"LSR", "MVN", "MVP", "NOP", "ORA", "PEA", "PEI", "PER", "PHA",
	"PHB", "PHD", "PHK", "PHP", "PHX", "PHY", "PLA", "PLB", "PLD",
	"PLP", "PLX", "PLY", "RET", "REP", "ROL", "ROR", "RTI", "RTL",
	"RTS", "SBC", "SEC", "SED", "SEI", "SEP", "SHL", "SHR", "STA",
	"STX", "STY", "STP", "STZ", "SWA", "TAD", "TAS", "TAX", "TAY",
	"TCB", "TCD", "TCS", "TDC", "TRB", "TSA", "TSB", "TSC", "TSX",
	"TXA", "TXS", "TXY", "TYA", "TYX", "WAI", "WDM", "XBA", "XCE"
};

enum
{
	Reg_A = 1<<0,
	Reg_X = 1<<1,
	Reg_Y = 1<<2,
	Reg_D = 1<<3,
	Reg_S = 1<<4,
	Reg_PBR = 1<<5,
	Reg_DBR = 1<<6,
	Reg_P = 1<<7
};

struct cpu_state
{
	unsigned status;
	uint8_t pb;
	uint16_t pc;
	bool emu;
};

struct instruction_def
{
	instruction_name name;
	addressing_mode args;
	unsigned regs;
};

struct instruction
{
	instruction_def *def;
	unsigned param;
	unsigned size;
};

#include "opcodes.h"

static void update_state(instruction &i, cpu_state &s)
{
	switch (i.def->name) {
		case I_CLC:
			s.status &= ~1;
			break;
		case I_REP:
			s.status &= ~i.param;
			break;
		case I_SEP:
			s.status |= i.param;
			break;
		case I_XCE:
		{
			bool tmp = s.emu;
			s.emu = s.status & 1;
			s.status = (s.status & ~1) | tmp;
		}
			break;
		case I_RTI:
		case I_RTS:
		case I_BRL:
		case I_JMP:
			printf("\t;End of the function!\n");
			exit(0);
	}
}

#define advance_pc(x) s.pc += x;
#define x_16() (!s.emu && ((s.status & 0x10) == 0))
#define a_16() (!s.emu && ((s.status & 0x20) == 0))

static const char *reg(instruction_def &i)
{
	switch(i.regs)
	{
		case Reg_A:
			return "A";
		case Reg_X:
			return "X";
		case Reg_Y:
			return "Y";
		default:
			return "???";
	}
}

#define a(n) for (int i = 0; i < n; i++) {param = (param << 8) | *mem++; isize++;}
#define a8() a(1)
#define a16() a(2)
#define a24() a(3)

static instruction disasm_one_insn(snes_mapper &map, cpu_state &s)
{
	uint8_t *mem = (uint8_t*)map.lookup(s.pb, s.pc);
	uint8_t op = *mem++;
	int param = 0, isize = 1;
	instruction_def &insn = opcodes[op];
	
	switch (insn.args) {
		case None:
		case Interrupt:
		case StackPull:
		case StackPush:
			break;
		case DirectPage:
		case DPIndirectIndexed:
		case DPIndirectLongIndexed:
		case DPIndexedIndirect:
		case DPIndexed:
		case DPIndirect:
		case DPIndirectLong:
		case PCRelative:
		case StackRelative:
		case StackRelativeIndirectIndexed:
			a8();
			break;
		case Absolute:
		case AbsIndexed:
		case AbsIndirect:
		case AbsIndexedIndirect:
		case PCRelativeLong:
		case BlockMove:
			a16();
			break;
		case AbsLong:
		case AbsLongIndexed:
			a24();
			break;
		case Immediate:
			if (s.emu) {a8();} else {a16();}
			break;
	}
	
	instruction i = (instruction){&insn, param, isize};
	
	update_state(i, s);
	return i;
}

extern void disassemble(snes_mapper &map, rom_header_t &rh)
{
	cpu_state s = (cpu_state){0x34, 0, rh.emu_exc.main, true};
	fprintf(stderr, "-Disassembling from entry point\n");
	for (int i = 0; i < 128; i++) disasm_one_insn(map, s);
	fprintf(stderr, "-That's all for now\n");
}