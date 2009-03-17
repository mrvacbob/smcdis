/*
 *  smc.h
 *  smcdis
 *
 *  Created by Alexander Strange on 12/21/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __SMC_H__
#define __SMC_H__
#include <stdint.h>

typedef enum rom_type {
	LoROM = 0,
	HiROM,
	ExLoROM,
	ExHiROM,
	BSCLoROM,
	BSCHiROM
} rom_type;

typedef enum coprocessor_type {
	DSP = 1,
	SuperFX,
	OBC1,
	SA1,
	UnknownCPU,
	Custom = 0xF
} coprocessor_type;

struct snes_interrupt_t {
	uint16_t cop;
	uint16_t brk;
	uint16_t abort;
	uint16_t vbl;
	uint16_t main; // 0xFFFC
	uint16_t irq;
};

struct rom_header_t {
	uint8_t company1;
	char game_code[4];
	char zero_fill[7];
	uint8_t expansion_ram;
	uint8_t special_version;
	uint8_t subcartridge;
	char rom_name[21];
	uint8_t map_mode;
	uint8_t cart_type;
	uint8_t rom_size;
	uint8_t sram_size;
	uint8_t region;
	uint8_t company2;
	uint8_t version;
	uint16_t ichecksum;
	uint16_t checksum; // 0xFFDE
	snes_interrupt_t norm_exc, emu_exc;
};

struct rom_info_t {
	rom_type type;
	bool interleaved;
	coprocessor_type coprocessor;
	uint8_t company;
	uint8_t dsp_ver;
	uint8_t rom_mbit;
	uint8_t eram_kbit;
	uint8_t sram_kbit;
	uint8_t map_mode;
	bool ram;
	bool battery;
	bool fastrom;
	bool hirom;
};

struct swc_header
{
	uint16_t size;
	uint8_t mode_select;
	uint8_t byte3;
	uint16_t magic;
	uint8_t code;
	
	bool mode, multi, sram, dram;
	uint8_t sram_size;
};

struct fig_header
{
	uint16_t size;
	uint8_t multi;
	uint8_t hirom;
	uint8_t ex1; // 0xFD or 0x47 or 0x77
	uint8_t ex2; // 0x82 or 0x83
};
#endif