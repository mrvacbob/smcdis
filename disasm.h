/*
 *  disasm.h
 *  smcdis
 *
 *  Created by Alexander Strange on 12/25/07.
 *
 */

#ifndef __DISASM_H__
#define __DISASM_H__
#include "smc.h"
#include "memmap.h"

extern void disassemble(snes_mapper &map, snes_rom_header &rh);

#endif