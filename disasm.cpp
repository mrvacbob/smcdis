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

#include <queue>
#include <set>
#include <vector>

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

    cpu_status() : flags(0) {}
    cpu_status(const cpu_status &p) : flags(p.flags) {}
};

// Which bits of cpu state have known values
struct cpu_status_known
{
    union {
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
            bool e:1; /// Emulation
        };
    };
    
    struct {
        bool pb:1; // Program bank register
        bool pc:1; // Program counter
    };
    
    cpu_status_known() : flags(0), pb(0), pc(0) {}
    cpu_status_known(const cpu_status_known &p_k) : flags(p_k.flags), pb(p_k.pb), pc(p_k.pc) {}
};

struct cpu_state
{
	cpu_status p;
    cpu_status_known p_k;
    
	uint8_t    pb;     /// Program bank
	uint16_t   pc;
    
    // TODO: regs
    
    uint16_t   block_size; // Count since this basic block started

    cpu_state() : pb(0), pc(0), block_size(0) {}
    cpu_state(const cpu_state &s) : p(s.p), p_k(s.p_k), pb(s.pb), pc(s.pc), block_size(s.block_size) {}
};

struct disassembler
{
    // List of CPU states left to scan
    std::queue<cpu_state> worklist;
    
    // List of addresses of known basic blocks
    std::set<uint16_t> visited_addresses;
    
    std::vector<cpu_state> basic_blocks;
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

static void print_insn(instruction &i, uint8_t *mem);

#include "opcodes.h"

static snes_mapper *map;
static snes_rom_header *rh;
static disassembler *d;



static instruction disasm_one_insn(cpu_state &s)
{
#define read_byte(off) (*(uint8_t*)map->lookup(s.pb, s.pc + (off)))
#define a(n)  for (int i = 0; i < n; i++) {param = (param << 8) | read_byte(i+1); isize++;}
#define a8()  a(1)
#define a16() a(2)
#define a24() a(3)
    
	uint8_t op = read_byte(0);
	int     param = 0, isize = 1;
	const   instruction_def &insn = opcodes[op];
	
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
	return i;
    
#undef a24
#undef a16
#undef a8
#undef read_byte
}

static void record_jump(cpu_state &s, uint16_t dest)
{
    cpu_state sj(s);
    
    sj.pc = dest;
    d->worklist.push(sj);
}

static void scan_basic_block(cpu_state &to_scan)
{
    bool block_ended = false;
    bool btmp;
    uint16_t stmp;

    cpu_state si(to_scan); // Initial state (to have metadata filled in)
    cpu_state s(si); // Iterating state
    
    s.block_size = 0;
    
#define check_e() if (s.p_k.e == false) {block_ended = true; break;}
    do {
        instruction i = disasm_one_insn(s);
        
        print_insn(i, (uint8_t*)map->lookup(s.pb, s.pc));
        
        s.block_size++;

        switch (i.def->name) {
            case I_CLC:
                s.p.c = 0; s.p_k.c = 1;
                break;
            case I_REP:
                btmp = s.p.e;
                s.p.flags &= ~i.param;
                s.p_k.flags &= ~i.param;
                s.p.e = btmp;
                break;
            case I_SEI:
                s.p.i = 0; s.p_k.i = 1;
            case I_SEP:
                s.p.flags |= i.param;
                s.p_k.flags |= i.param;
                break;
            case I_XCE:
                btmp  = s.p.e;
                s.p.e = s.p.c;
                s.p.c = btmp;
                
                btmp = s.p_k.e;
                s.p_k.e = s.p_k.c;
                s.p_k.c = btmp;
                break;
            case I_BRK:
                check_e();
                record_jump(s, s.p.e ? rh->emu_exc.brk : rh->norm_exc.brk);
                block_ended = true;
                break;
            case I_BRL:
                record_jump(s, s.pc + i.size + i.param);
                block_ended = true;
                break;
            case I_RTI:
            case I_RTS:
            case I_JMP:
            case I_JSR:
            case I_STP:
            case I_WAI:
                block_ended = true;
                break;
            case I_PLP:
                s.p_k.pc = false;
                break;
        }
        
        if (s.p_k.pc == false) break; // can't continue if we don't know where to go...
        else s.pc += i.size;
        
    } while (!block_ended);
    
    si.block_size = s.block_size;
    
    d->visited_addresses.insert(si.pc);
    d->basic_blocks.push_back(si);
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
			printf("$%X ; d", i.param);
			break;
		case DPIndexed:
			printf("$%X,%s ; d", i.param, reg(i.def));
			break;
		case DPIndexedIndirect:
			printf("($%X,%s) ; d", i.param, reg(i.def));
			break;
		case DPIndirect:
			printf("($%X) ; d", i.param);
			break;
		case DPIndirectIndexed:
			printf("($%X),%s ; d", i.param, reg(i.def));
			break;
		case DPIndirectLong:
			printf("[$%X] ; d", i.param);
			break;
		case DPIndirectLongIndexed:
			printf("[$%X],%s ; d", i.param, reg(i.def));
			break;
		case PCRelative:
		case PCRelativeLong:
			printf("$%X ; pc", i.param);
			break;
		case StackRelative:
			printf("$%X ; s", i.param);
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

	printf("\t; args: %#x regs: %#x # ", i.def->args, i.def->regs);

	for (size_t n = 0; n < i.size; n++) {
		printf("%.2X ", mem[n]);
	}

	printf("\n");
}

void disassemble(snes_mapper &_map, snes_rom_header &_rh)
{
    map = &_map;
    rh  = &_rh;
    
    disassembler _d;
    d = &_d;

    {
        cpu_state s;
        
        // Scan forwards from initial execution state
        s.p.i = 1; s.p_k.i = 1;
        s.p.x = 1; s.p_k.x = 1;
        s.p.m = 1; s.p_k.m = 1;
        s.p.e = 1; s.p_k.e = 1;
        s.pc = rh->emu_exc.main; s.p_k.pc = 1;
        s.pb = 0; s.p_k.pb = 0;
        
        assert(s.p.flags == 0x134);
        
        d->worklist.push(s);
    }
    
    do {
        cpu_state &s = d->worklist.front();
        scan_basic_block(s);
        d->worklist.pop();
    } while (!d->worklist.empty());
    
    int i = 0, n_basic_blocks = d->basic_blocks.size();
    
    for (; i < n_basic_blocks; i++) {
        cpu_state &s = d->basic_blocks[i];
        printf("- Basic block %d of %d at %#x, %d insns\n", i, n_basic_blocks, s.pc, s.block_size);
    }
    
//	fprintf(stderr, "-Disassembling from entry point\n");
//	for (int i = 0; i < 128; i++) disasm_one_insn(map, s);
//	fprintf(stderr, "-That's all for now\n");
}
