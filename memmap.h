/*
 *  memmap.h
 *  smcdis
 *
 *  Created by Alexander Strange on 12/23/07.
 *
 */

#ifndef __MEMMAP_H__
#define __MEMMAP_H__
#include <list>
#include <stdint.h>
#include <stddef.h>

using std::list;

struct map_item
{
	uint32_t begin, end;
	uint8_t *rom;
};

class snes_mapper
{
public:
	snes_mapper(uint8_t *base) : base(base) {}
	void map_rom(uint8_t bank_begin, uint8_t bank_end, uint16_t addr_begin, uint16_t addr_end, size_t offset, size_t rom_size);
	void print();
	
	uint8_t *lookup(uint8_t bank, uint16_t addr);
	uint8_t *lookup(uint32_t addr);

	uint8_t *base;

protected:
	list<map_item> map;
};

#endif