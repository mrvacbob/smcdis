/*
 *  disasm.cpp
 *  smcdis
 *
 *  Created by Alexander Strange on 12/25/07.
 *
 */

#include "disasm.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

enum addressing_mode
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
};

enum instruction_name
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
};

static const char opcode_names[][4] = 
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
	Reg_PBR = 1<<5, /// Program bank register
	Reg_DBR = 1<<6, /// Data bank register
	Reg_P = 1<<7
};

union cpu_status
{
	uint16_t flags;
	struct {
		bool c:1; /// Carry flag
		bool z:1; /// Zero flag
		bool i:1; /// IRQ disable
		bool d:1; /// Decimal mode
		bool x:1; /// Index select 8-bit/16-bit
		bool m:1; /// Accumulator select 8-bit/16-bit
		bool v:1; /// Overflow flag
		bool n:1; /// Negative flag
		bool e:1; /// Emulation (always 1)
	};            /// Native mode flags

	struct {
		bool c:1; /// Carry flag
		bool z:1; /// Zero flag
		bool i:1; /// IRQ disable
		bool d:1; /// Decimal mode
		bool b:1; /// BRK caused last interrupt
		bool _:1; /// Unused
		bool v:1; /// Overflow flag
		bool n:1; /// Negative flag
		bool e:1; /// Emulation (always 0)
	} emu;        /// Emulation mode flags

	cpu_status() : flags(0), e(1), m(1), x(1), i(1) {}
};

struct cpu_state
{
	cpu_status p;
	uint8_t    pb;     /// Program bank
	uint16_t   pc;

	cpu_state(snes_rom_header &rh) : pb(0), pc(rh.emu_exc.main) {}
};

struct instruction_def
{
	uint8_t name;
	uint8_t args;
	uint8_t regs;
};

struct instruction
{
	const instruction_def *def;
	unsigned param;
	uint8_t size;
};

#include "opcodes.h"

static void update_state(instruction &i, cpu_state &s)
{
	bool tmp;
	switch (i.def->name) {
		case I_CLC:
			s.p.c = 0;
			break;
		case I_REP:
			tmp = s.p.e;
			s.p.flags &= ~i.param;
			s.p.e = tmp;
			break;
		case I_SEP:
			s.p.flags |= i.param;
			break;
		case I_XCE:
			tmp = s.p.e;
			s.p.e = s.p.c;
			s.p.c = tmp;
			break;
		case I_RTI:
		case I_RTS:
		case I_BRK:
		case I_BRL:
		case I_JMP:
		case I_JSR:
		case I_STP:
		case I_WAI:
			printf("\t; End of the function!\n");
			exit(0);
		case I_PLP:
			printf("\t; Unable to track PLP\n");
			exit(0);
	}
	
	s.pc += i.size;
}

static const char *reg(const instruction_def *i)
{
	switch(i->regs)
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

static void print_insn(instruction &i, uint8_t *mem)
{
	printf("\t%s\t", opcode_names[i.def->name]);

	switch ((addressing_mode)i.def->args) {
		case None:
			break;
		case Immediate:
		case Interrupt:
			printf("#$%X", i.param);
			break;
		case Absolute:
		case AbsLong:
			printf("$%X", i.param);
			break;
		case AbsIndirect:
			printf("($%X)", i.param);
			break;
		case AbsIndexed:
		case AbsLongIndexed:
			printf("$%X,%s", i.param, reg(i.def));
			break;
		case AbsIndexedIndirect:
			printf("($%X,%s)", i.param, reg(i.def));
			break;
		case AbsIndirectLong:
			printf("[$%X]", i.param);
			break;
		case DirectPage:
			printf("$%X ;d", i.param);
			break;
		case DPIndexed:
			printf("$%X,%s ;d", i.param, reg(i.def));
			break;
		case DPIndexedIndirect:
			printf("($%X,%s) ;d", i.param, reg(i.def));
			break;
		case DPIndirect:
			printf("($%X) ;d", i.param);
			break;
		case DPIndirectIndexed:
			printf("($%X),%s ;d", i.param, reg(i.def));
			break;
		case DPIndirectLong:
			printf("[$%X] ;d", i.param);
			break;
		case DPIndirectLongIndexed:
			printf("[$%X],%s ;d", i.param, reg(i.def));
			break;
		case PCRelative:
		case PCRelativeLong:
			printf("$%X ;pc", i.param);
			break;
		case StackRelative:
			printf("$%X ;s", i.param);
			break;
		case StackRelativeIndirectIndexed:
			printf("(#%X),%s ;", i.param, reg(i.def));
			break;
		case BlockMove:
			printf("$%X", i.param);
			break;
		case StackPush:
		case StackPull:
		case WDM:
			break;
	}

	printf("\t; ");

	for (size_t n = 0; n < i.size; n++) {
		printf("%.2X ", mem[n]);
	}

	printf("\n");
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
	const instruction_def &insn = opcodes[op];
	
	switch (insn.args) {
		case None:
		case StackPull:
		case StackPush:
			break;
		case Interrupt:
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
			if (s.p.e) {a8();} else {a16();}
			break;
	}
	
	instruction i = (instruction){&insn, param, isize};
	
	print_insn(i, mem-isize);
	update_state(i, s);
	return i;
}

void disassemble(snes_mapper &map, snes_rom_header &rh)
{
	cpu_state s(rh);

	assert(s.p.flags == 0x134);

	fprintf(stderr, "-Disassembling from entry point\n");
	for (int i = 0; i < 128; i++) disasm_one_insn(map, s);
	fprintf(stderr, "-That's all for now\n");
}
