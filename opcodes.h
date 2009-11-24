instruction_def opcodes[] = {
	{I_BRK, Interrupt},
	{I_ORA, DPIndexedIndirect, Reg_X},
	{I_COP, Interrupt},
	{I_ORA, StackRelative},
	{I_TSB, DirectPage},
	{I_ORA, DirectPage},
	{I_ASL, DirectPage},
	{I_ORA, DPIndirectLong},
	{I_PHP, StackPush, Reg_P},
	{I_ORA, Immediate, Reg_A},
	{I_ASL, None},
	{I_PHD, StackPush, Reg_D},
	{I_TSB, Absolute},
	{I_ORA, Absolute},
	{I_ASL, Absolute},
	{I_ORA, AbsLong},
	{I_BPL, PCRelativeLong},
	{I_ORA, DPIndirectIndexed, Reg_Y},
	{I_ORA, DPIndirect},
	{I_ORA, StackRelativeIndirectIndexed, Reg_Y},
	{I_TRB, DirectPage},
	{I_ORA, DPIndexed, Reg_X},
	{I_ASL, DPIndexed, Reg_X},
	{I_ORA, DPIndirectLongIndexed, Reg_Y},
	{I_CLC, None},
	{I_ORA, AbsIndexed, Reg_Y},
	{I_INC, None},
	{I_TCS, None},
	{I_TRB, Absolute},
	{I_ORA, AbsIndexed, Reg_X},
	{I_ASL, AbsIndexed, Reg_X},
	{I_ORA, AbsLongIndexed, Reg_X},
	{I_JSR, Absolute},
	{I_AND, DPIndexedIndirect, Reg_X},
	{I_JSR, AbsLong},
	{I_AND, StackRelative},
	{I_BIT, DirectPage},
	{I_AND, DirectPage},
	{I_ROL, DirectPage},
	{I_AND, DPIndirectLong},
	{I_PLP, StackPull, Reg_P},
	{I_AND, Immediate, Reg_A},
	{I_ROL, None},
	{I_PLD, StackPull, Reg_D},
	{I_BIT, Absolute},
	{I_AND, Absolute},
	{I_ROL, Absolute},
	{I_AND, AbsLong},
	{I_BMI, PCRelative},
	{I_AND, DPIndirectIndexed, Reg_Y},
	{I_AND, DPIndirect},
	{I_AND, StackRelativeIndirectIndexed, Reg_Y},
	{I_BIT, DPIndexed, Reg_X},
	{I_AND, DPIndexed, Reg_X},
	{I_ROL, DPIndexed, Reg_X},
	{I_AND, DPIndirectLongIndexed, Reg_Y},
	{I_SEC, None},
	{I_AND, AbsIndexed, Reg_Y},
	{I_DEC, None},
	{I_TSC, None},
	{I_BIT, AbsIndexed, Reg_X},
	{I_AND, AbsIndexed, Reg_X},
	{I_ROL, AbsIndexed, Reg_X},
	{I_AND, AbsLongIndexed, Reg_X},
	{I_RTI, None},
	{I_EOR, DPIndexedIndirect, Reg_X},
	{I_WDM, WDM},
	{I_EOR, StackRelative},
	{I_MVP, BlockMove},
	{I_EOR, DirectPage},
	{I_LSR, DirectPage},
	{I_EOR, DPIndirectLong},
	{I_PHA, StackPush, Reg_A},
	{I_EOR, Immediate, Reg_A},
	{I_LSR, None},
	{I_PHK, StackPush, Reg_PBR},
	{I_JMP, Absolute},
	{I_EOR, Absolute},
	{I_LSR, Absolute},
	{I_EOR, AbsLong},
	{I_BVC, PCRelative},
	{I_EOR, DPIndirectIndexed, Reg_Y},
	{I_EOR, DPIndirect},
	{I_EOR, StackRelativeIndirectIndexed, Reg_Y},
	{I_MVN, BlockMove},
	{I_EOR, DPIndexed, Reg_X},
	{I_LSR, DPIndexed, Reg_X},
	{I_EOR, DPIndirectLongIndexed, Reg_Y},
	{I_CLI, None},
	{I_EOR, AbsIndexed, Reg_Y},
	{I_PHY, StackPush, Reg_Y},
	{I_TCD, None},
	{I_JMP, AbsLong},
	{I_EOR, AbsIndexed, Reg_X},
	{I_LSR, AbsIndexed, Reg_X},
	{I_EOR, AbsLongIndexed, Reg_X},
	{I_RTS, None},
	{I_ADC, DPIndirectIndexed,  Reg_X},
	{I_PER, StackPush},
	{I_ADC, StackRelative},
	{I_STZ, DirectPage},
	{I_ADC, DirectPage},
	{I_ROR, DirectPage},
	{I_ADC, DPIndirectLong},
	{I_PLA, StackPull, Reg_A},
	{I_ADC, Immediate, Reg_A},
	{I_ROR, None},
	{I_RTL, None},
	{I_JMP, AbsIndirect},
	{I_ADC, Absolute},
	{I_ROR, Absolute},
	{I_ADC, AbsLong},
	{I_BVS, PCRelative},
	{I_ADC, DPIndirectIndexed, Reg_Y},
	{I_ADC, DPIndirect},
	{I_ADC, StackRelativeIndirectIndexed, Reg_Y},
	{I_STZ, DPIndexedIndirect, Reg_X},
	{I_ADC, DPIndexed, Reg_X},
	{I_ROR, DPIndexed, Reg_X},
	{I_ADC, DPIndirectLongIndexed, Reg_Y},
	{I_SEI, None},
	{I_ADC, AbsIndexed, Reg_Y},
	{I_PLY, StackPull, Reg_Y},
	{I_TDC, None},
	{I_JMP, AbsIndexedIndirect},
	{I_ADC, AbsIndexed, Reg_X},
	{I_ROR, AbsIndexed, Reg_X},
	{I_ADC, AbsLongIndexed, Reg_X},
	{I_BRA, PCRelative},
	{I_STA, DPIndexedIndirect, Reg_X},
	{I_BRL, PCRelativeLong},
	{I_STA, StackRelative},
	{I_STY, DirectPage},
	{I_STA, DirectPage},
	{I_STX, DirectPage},
	{I_STA, DPIndirectLong},
	{I_DEY, None},
	{I_BIT, Immediate, Reg_A},
	{I_TXA, None},
	{I_PHB, StackPush, Reg_DBR},
	{I_STY, Absolute},
	{I_STA, Absolute},
	{I_STX, Absolute},
	{I_STA, AbsLong},
	{I_BCC, PCRelative},
	{I_STA, DPIndirectIndexed, Reg_Y},
	{I_STA, DPIndirect},
	{I_STA, StackRelativeIndirectIndexed, Reg_Y},
	{I_STY, DPIndexedIndirect, Reg_X},
	{I_STA, DPIndexedIndirect, Reg_X},
	{I_STX, DPIndirectIndexed, Reg_Y},
	{I_STA, DPIndirectLongIndexed, Reg_Y},
	{I_TYA, None},
	{I_STA, AbsIndexed, Reg_Y},
	{I_TXS, None},
	{I_TXY, None},
	{I_STZ, Absolute},
	{I_STA, AbsIndexed, Reg_X},
	{I_STZ, AbsIndexed, Reg_X},
	{I_STA, AbsLongIndexed, Reg_X},
	{I_LDY, Immediate, Reg_Y},
	{I_LDA, DPIndexedIndirect, Reg_X},
	{I_LDX, Immediate, Reg_X},
	{I_LDA, StackRelative},
	{I_LDY, DirectPage},
	{I_LDA, DirectPage},
	{I_LDX, DirectPage},
	{I_LDA, DPIndirectLong},
	{I_TAY, None},
	{I_LDA, Immediate, Reg_A},
	{I_TAX, None},
	{I_PLB, StackPull, Reg_DBR},
	{I_LDY, Absolute},
	{I_LDA, Absolute},
	{I_LDX, Absolute},
	{I_LDA, AbsLong},
	{I_BCS, PCRelative},
	{I_LDA, DPIndirectIndexed, Reg_Y},
	{I_LDA, DPIndirect},
	{I_LDA, StackRelativeIndirectIndexed, Reg_Y},
	{I_LDY, DPIndexed, Reg_X},
	{I_LDA, DPIndexed, Reg_X},
	{I_LDX, DPIndexed, Reg_Y},
	{I_LDA, DPIndirectLongIndexed, Reg_Y},
	{I_CLV, None},
	{I_LDA, AbsIndexed, Reg_Y},
	{I_TSX, None},
	{I_TYX, None},
	{I_LDY, AbsIndexed, Reg_X},
	{I_LDA, AbsIndexed, Reg_X},
	{I_LDX, AbsIndexed, Reg_Y},
	{I_LDA, AbsLongIndexed, Reg_X},
	{I_CPY, Immediate, Reg_Y},
	{I_CMP, DPIndexedIndirect, Reg_X},
	{I_REP, Immediate},
	{I_CMP, StackRelative},
	{I_CPY, DirectPage},
	{I_CMP, DirectPage},
	{I_DEC, DirectPage},
	{I_CMP, DPIndirectLong},
	{I_INY, None},
	{I_CMP, Immediate, Reg_A},
	{I_DEX, None},
	{I_WAI, None},
	{I_CPY, Absolute},
	{I_CMP, Absolute},
	{I_DEC, Absolute},
	{I_CMP, AbsLong},
	{I_BNE, PCRelative},
	{I_CMP, DPIndirectIndexed, Reg_Y},
	{I_CMP, DPIndirect},
	{I_CMP, StackRelativeIndirectIndexed, Reg_Y},
	{I_PEI, DPIndirect},
	{I_CMP, DPIndexed, Reg_X},
	{I_DEC, DPIndexed, Reg_X},
	{I_CMP, DPIndirectLongIndexed, Reg_Y},
	{I_CLD, None},
	{I_CMP, AbsIndexed, Reg_Y},
	{I_PHX, StackPush, Reg_X},
	{I_STP, None},
	{I_JMP, AbsIndirectLong},
	{I_CMP, AbsIndexed, Reg_X},
	{I_DEC, AbsIndexed, Reg_X},
	{I_CMP, AbsLongIndexed, Reg_X},
	{I_CPX, Immediate, Reg_X},
	{I_SBC, DPIndexedIndirect, Reg_X},
	{I_SEP, Immediate},
	{I_SBC, StackRelative},
	{I_CPX, DirectPage},
	{I_SBC, DirectPage},
	{I_INC, DirectPage},
	{I_SBC, DPIndirectLong},
	{I_INX, None},
	{I_SBC, Immediate, Reg_A},
	{I_NOP, None},
	{I_XBA, None},
	{I_CPX, Absolute},
	{I_SBC, Absolute},
	{I_INC, Absolute},
	{I_SBC, AbsLong},
	{I_BEQ, PCRelative},
	{I_SBC, DPIndirectIndexed, Reg_Y},
	{I_SBC, DPIndirect},
	{I_SBC, StackRelativeIndirectIndexed, Reg_Y},
	{I_PEA, Absolute},
	{I_SBC, DPIndexedIndirect, Reg_X},
	{I_INC, DPIndexedIndirect, Reg_X},
	{I_SBC, DPIndirectLongIndexed, Reg_Y},
	{I_SED, None},
	{I_SBC, AbsIndexed, Reg_Y},
	{I_PLX, StackPull, Reg_X},
	{I_XCE, None},
	{I_JSR, AbsIndexedIndirect, Reg_X},
	{I_SBC, AbsIndexed, Reg_X},
	{I_INC, AbsIndexed, Reg_X},
	{I_SBC, AbsLongIndexed, Reg_X}
};