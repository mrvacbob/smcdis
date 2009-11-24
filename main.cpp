#include <stdio.h>
#include <ctype.h>

#include "buf.h"
#include "smc.h"
#include "tables.h"
#include "memmap.h"
#include "disasm.h"

//32768 bytes/32KB
#define SMC_BLOCK_SIZE 0x8000

static const unsigned header_offsets[] = {0x7fc0, 0xffc0, 0x40ffc0};
static size_t smc_off=0, header_off=0, rom_size;
static rom_header_t rom_header;
static rom_info_t rom_info = {(rom_type)0};
static void dump_rom_header(rom_header_t &h);

static swc_header read_swc_header(stream_reader &smc)
{
	swc_header swc;
	swc.size = smc.read_u16le();
	swc.mode_select = smc.read_u8();
	swc.byte3 = smc.read_u8();
	smc.skip(4);
	swc.magic = smc.read_u16be();
	swc.code = smc.read_u8();
	
	swc.mode =  swc.mode_select >> 7;
	swc.multi= (swc.mode_select >> 6) & 1;
	swc.sram = (swc.mode_select >> 5) & 1;
	swc.dram = (swc.mode_select >> 4) & 1;
	swc.sram_size = (swc.mode_select >> 2) & 3;
	
	return swc;
}

static fig_header read_fig_header(stream_reader &smc)
{
	fig_header fig;
	fig.size = smc.read_u16le();
	fig.multi = smc.read_u8();
	fig.hirom = smc.read_u8();
	fig.ex1 = smc.read_u8();
	fig.ex2 = smc.read_u8();
	
	return fig;
}

static const char *yes(bool y)
{
	return y ? "yes" : "no";
}

static size_t smc_header(stream_reader &smc) {
	size_t fs = smc.size(), extrasize = fs % SMC_BLOCK_SIZE;
	
	fprintf(stderr, "-File size: %lu 32KB blocks, %lu 8KB pages\n", fs/0x8000, fs/8192);
	
	if (extrasize == 0) {
		fprintf(stderr, "-No copier header found (%lu bytes left over).\n", extrasize);
	} else {
		swc_header swc;
		fig_header fig;
		fprintf(stderr, "-Copier header found?\n\n");
		
		swc = read_swc_header(smc);
		smc.seek(0);
		fig = read_fig_header(smc);
		
		if (swc.magic == 0xAABB) {
			if (swc.mode_select) {
				printf("Front Fareast SWC/SMC header:\n");			
				printf("\t8KB pages: %d (%s)\n", swc.size, (swc.size == fs/8192) ? "correct" : "incorrect");
				printf("\tMode select: %x\n", swc.mode_select);
				printf("\tJump to $8000 instead of RST: %s\n", yes(swc.mode));
				printf("\tMulti-image: %s\n", yes(swc.multi));
				printf("\tSRAM map: %s\n", swc.sram ? "HiROM" : "LoROM");
				printf("\tDRAM map: %s\n", swc.dram ? "HiROM" : "LoROM");
				printf("\tSRAM size: %x (%d KBit)\n", swc.sram_size, swc.sram_size ? 1 << (swc.sram_size+3) : 0);
				printf("\tCode: %d (%s)\n\n", swc.code, (swc.code == 4) ? "correct" : "incorrect");
			} else {
				printf("---Broken SWC/SMC header(?):\n");
				printf("\t8KB pages: %d (%s)\n", swc.size, (swc.size == fs/8192) ? "correct" : "incorrect");
				printf("\tBytes 2 and 3: %#x %#x\n", swc.mode_select, swc.byte3);
				printf("\tCode: %d (%s)\n\n", swc.code, (swc.code == 4) ? "correct" : "incorrect");
			}
		} else if (fig.ex2 == 0x80 || fig.ex2 == 0x82 || fig.ex2 == 0x83 || fig.ex1 == 0xDD) {
			printf("Pro Fighter header:\n");
			printf("\t8KB pages: %d (%s)\n", fig.size, (fig.size == fs/8192) ? "correct" : "incorrect");
			printf("\tMulti-image: %s\n", yes(fig.multi));
			printf("\tROM type: %s\n", fig.hirom ? "HiROM" : "LoROM");
			printf("\tComplicated code: %.2X%.2X\n", fig.ex1, fig.ex2);
		} else printf("Magic %#x unknown...\n", swc.magic);
		
		smc.seek(512);
	}
	
	return extrasize;
}

static snes_interrupt_t read_exception_table(stream_reader &smc)
{
	snes_interrupt_t i;
	
	i.cop = smc.read_u16le();
	i.brk = smc.read_u16le();
	i.abort = smc.read_u16le();
	i.vbl = smc.read_u16le();
	i.main = smc.read_u16le();
	i.irq = smc.read_u16le();
	
	return i;
}

static void read_rom_header_at(stream_reader &smc, size_t at, rom_header_t *h)
{
	char maker[2];
	smc.seek(at - 16);
	
	smc.read_bytes(maker, 2);
	sscanf(maker, "%2hhx", &h->company1);
	smc.read_bytes(h->game_code, 4);
	smc.read_bytes(h->zero_fill, 7);
	h->expansion_ram = smc.read_u8();
	h->special_version = smc.read_u8();
	h->subcartridge = smc.read_u8();
	smc.read_bytes(h->rom_name, 21);
	h->map_mode = smc.read_u8();
	h->cart_type = smc.read_u8();
	h->rom_size = smc.read_u8();
	h->sram_size = smc.read_u8();
	h->region = smc.read_u8();
	h->company2 = smc.read_u8();
	h->version = smc.read_u8();
	h->ichecksum = smc.read_u16le();
	h->checksum = smc.read_u16le();
	
	smc.skip(4);
	h->norm_exc = read_exception_table(smc);
	smc.skip(4);
	h->emu_exc = read_exception_table(smc);
}

static int rom_header_score(rom_header_t &h, rom_type type)
{
	int s = 0;
	
	s += h.cart_type < 8;
	s += h.rom_size < 16;
	s += h.sram_size < 8;
	s += h.region < 14;
	s += h.version < 128;
	s += (h.ichecksum + h.checksum) == 0xffff;
	
	return s;
}

static rom_header_t find_rom_header(stream_reader &smc, size_t *off)
{
	rom_header_t headers[3];
	int scores[3] = {0};
	
	for (size_t i = 0; i < sizeof(header_offsets) / sizeof(unsigned); i++) {
		unsigned off = header_offsets[i] + smc_off;
		
		if (rom_size < off) {
			fprintf(stderr, "-The ROM is too small for a header at %#x\n", off);
			scores[i] = -1;
		} else {
			fprintf(stderr, "-Checking for header at %#x\n", off);
			
			read_rom_header_at(smc, off, &headers[i]);
			scores[i] = rom_header_score(headers[i], (rom_type)i);
		}
		fprintf(stderr, "-Score: %d\n", scores[i]);
	}
	
	fprintf(stderr, "\n");
	
	int hnum = 0;
	
	for (int i = 0; i < 3; i++) if (scores[i] > scores[hnum]) hnum = i;
	
	*off = header_offsets[hnum] + smc_off;
	rom_info.type = (rom_type)hnum;
	return headers[hnum];
}

static void clean_puts(char *n, size_t s)
{
	while (s--) {
		char c = *n++;
		putchar((c == ' ' || isgraph(c)) ? c : '.');
	}
	
	putchar('\n');
}

static void hex_puts(char *n, size_t s, size_t w)
{
	size_t lines = s / w;
	
	for (size_t l = 0; l < lines; l++) {
		if (!s) break;
		printf("\t");
		for (size_t i = 0; i < w; i++) {
			if (!s) break;
			printf("%.2hhX", *n++);
			if ((i != w) && i && (i % 2 == 0)) printf(" ");
			s--;
		}
		printf("\n");
	}
}

static void dump_rom_header(rom_header_t &h)
{
	if (h.company2 != 0x33) printf("This is a compact copied ROM header. (missing first 16 bytes)\n");
	printf("This is a %s%s.\n\n", rom_info.interleaved ? "interleaved " : "", get_rom_type_name(rom_info));
	printf("ROM header:\n");
	if (h.company2 == 0x33) {
		printf("Game code: ");
		clean_puts(h.game_code, 4);
		printf("\tExpansion RAM size: %d (%d KBit)\n"
			   "\tSpecial version: v%d\n"
			   "\tCartridge sub-id: %d\n",
			   h.expansion_ram, h.expansion_ram ? (1 << (h.expansion_ram + 3)) : 0, h.special_version, h.subcartridge);
	}
	printf("\tName: ");
	clean_puts(h.rom_name, 21);
	printf("\tMap mode: %#x\n"
		   "\tROM type: %#x\n"
		   "\tROM size: %d (%d MBit)\n"
		   "\tSRAM size: %d (%d KBit)\n"
		   "\tRegion: %d (%s)\n"
		   "\tCompany: %d (%s)\n"
		   "\tVersion: 1.%d\n"
		   "\tInverse checksum: %#x\n"
		   "\tChecksum: %#x %s\n\n",
		   h.map_mode , h.cart_type, h.rom_size, 1 << (h.rom_size-7),
		   h.sram_size, 1 << (h.sram_size + 3), h.region, get_rom_region_name(h), h.company2 == 0x33 ? h.company1 : h.company2, get_rom_company_name(h),
		   h.version, h.ichecksum, h.checksum, (h.ichecksum + h.checksum == 0xFFFF) ? "" : "(!)");
}

static void set_ram_type(uint8_t type, rom_info_t &ri)
{
	switch (type) {
		case 2:
			ri.battery = true;
		case 1:
			ri.ram = true;
	}
}

static void find_game_chips(rom_header_t &rh, rom_info_t &ri)
{
	//if (rh.company == 0x33) ri.type = ri.type ? BSCHiROM : BSCLoROM;
	ri.hirom = rh.map_mode % 2;
	if (ri.type != ri.hirom) ri.interleaved = true;
	if (ri.type == LoROM && rom_size >= 0x401000) ri.type = ExLoROM;
	
	if (rh.map_mode >= 0x30) ri.fastrom = true;
	ri.map_mode = rh.map_mode & ~16;
	
	if (rh.cart_type < 3) set_ram_type(rh.cart_type, ri);
	else {
		set_ram_type((rh.cart_type - 3) & 3, ri);
		ri.coprocessor = (coprocessor_type)(rh.cart_type >> 4);
	}
	
	if (ri.coprocessor == DSP) ri.dsp_ver = 1; // wrong
	
	ri.rom_mbit  = 1 << (rh.rom_size-7);
	ri.sram_kbit = 1 << (rh.sram_size+3);
	ri.eram_kbit = 1 << (rh.expansion_ram+3);
}

static void dump_chip_list(rom_info_t &ri)
{
	const char *coproc_names[] = {"None", "DSP", "Super FX", "OBC1", "SA-1"};
	printf("Hardware features:\n");
	if (ri.coprocessor) printf("\tCoprocessor: %s\n", ri.coprocessor >= UnknownCPU ? "Unknown" : coproc_names[ri.coprocessor]);
	if (ri.dsp_ver) printf("\tDSP version: %d\n", ri.dsp_ver);
	printf("\tMemory map: Mode %x\n", ri.map_mode);
	printf("\tCPU in fast mode: %s\n", yes(ri.fastrom));
	printf("\tRAM: %s\n", yes(ri.ram));
	printf("\tBattery: %s\n\n", yes(ri.battery));
}

static bool map_memory(snes_mapper &map, stream_reader &smc, rom_info_t &ri)
{	
	if (ri.map_mode == 0x20) {
		fprintf(stderr, "-Mapping LoROM...\n");
		map.map_rom(0x00, 0x7D, 0x8000, 0xFFFF, smc_off, rom_size - smc_off);
		fprintf(stderr, "\n");
		return 0;
	}
	
	fprintf(stderr, "-No mapping for mode %x\n", ri.map_mode);
	return 1;
}

static void print_one_vector(const char *name, snes_mapper &map, uint16_t v)
{
	printf("\t%s: %.2X ", name, v);
	char *real = map.lookup(0, v);
	
	if (real) printf("(%#x)\n", unsigned(real - map.base));
	else printf("(unmapped)\n");
}

static void print_interrupt_table(snes_mapper &map, rom_header_t &rh, bool emu)
{
	snes_interrupt_t &i = emu ? rh.emu_exc : rh.norm_exc;
	
	print_one_vector("COP", map, i.cop);
	if (!emu) print_one_vector("BRK", map, i.brk);
	print_one_vector("ABORT", map, i.abort);
	print_one_vector("VBL/NMI", map, i.vbl);
	if (emu) print_one_vector("MAIN/RESET", map, i.main);
	print_one_vector(emu ? "IRQ/BRK" : "IRQ", map, i.irq);
}

static void print_vectors(snes_mapper &map, rom_header_t &rh)
{
	printf("Normal-mode exception handlers:\n");
	print_interrupt_table(map, rh, 0);
	printf("6502-mode exception handlers:\n");
	print_interrupt_table(map, rh, 1);
}

int main (int argc, char * const argv[]) {
	if (argc < 2) return 0;
	mapped_file rf(argv[1]);
    stream_reader smc(rf);
	
	rom_size = smc.size();
	smc_off = smc_header(smc);
	rom_header = find_rom_header(smc, &header_off);
	find_game_chips(rom_header, rom_info);
	dump_rom_header(rom_header);
	dump_chip_list(rom_info);
	
    printf("bit test %d %d\n",smc.read_sbits(4), smc.read_bits(4));
	smc.seek(0);
	snes_mapper map((char*)smc.cur());
	
	if (map_memory(map, smc, rom_info) == 0) {
		print_vectors(map, rom_header);
		printf("\n");
		disassemble(map, rom_header);
	}
	
    return 0;
}