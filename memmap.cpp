/*
 *  memmap.cpp
 *  smcdis
 *
 *  Created by Alexander Strange on 12/23/07.
 *
 */

#include "memmap.h"
#include <stdio.h>
#include <assert.h>

static uint32_t makeaddr(uint8_t bank, uint16_t addr) {return (bank << 16) | addr;}

void snes_mapper::map_rom(uint8_t bank_begin, uint8_t bank_end, uint16_t addr_begin, uint16_t addr_end, size_t offset, size_t rom_size)
{
	size_t r = 0;
	
	fprintf(stderr, "-Mapping %#lx bytes\n", rom_size);
	for (int b = bank_begin; b <= bank_end; b++) {
		map_item m = (map_item){makeaddr(b, addr_begin), makeaddr(b, addr_end), offset + r};
		r += m.end - m.begin;

		if (r >= rom_size) break;
		map.push_back(m);
	}
}

void snes_mapper::print()
{
	list<map_item>::iterator i;
	
	for (i = map.begin(); i != map.end(); i++) {
		map_item &m = *i;
		
		printf("$%.2X:%.4X - $%.2X:%.4X: %#x - %#x\n", m.begin >> 16, m.begin & 0xffff,
													   m.end >> 16, m.end & 0xffff,
													   m.rom_off, m.rom_off + (m.end - m.begin));
	}
}

uint8_t *snes_mapper::lookup(uint32_t a)
{
	list<map_item>::iterator i;
	
	for (i = map.begin(); i != map.end(); i++) {
		map_item &m = *i;

		if (m.begin > a) {return NULL;}
		else if (m.begin <= a && a <= m.end) return base + m.rom_off + (a - m.begin);
	}
	
	return NULL;
}

uint8_t *snes_mapper::lookup(uint8_t bank, uint16_t addr)
{
	uint8_t *result = lookup(makeaddr(bank, addr));
    
    if (!result) {
        fprintf(stderr, "- Couldn't read memory at $%.2X:%.4X. The memory map is likely wrong.\n", bank, addr);
        exit(1);
        return NULL;
    }
    
    return result;
}
