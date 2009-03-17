/*
 *  disasm.h
 *  smcdis
 *
 *  Created by Alexander Strange on 12/25/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __DISASM_H__
#define __DISASM_H__
#include "smc.h"
#include "memmap.h"

extern void disassemble(snes_mapper &map, rom_header_t &rh);

#endif