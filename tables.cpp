/*
 *  tables.cpp
 *  smcdis
 *
 *  Created by Alexander Strange on 12/21/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "tables.h"
#include <stdio.h>

static const char *company_names[] = {
	"", "Nintendo", "", "Imagineer-Zoom", "", "Zamuse", "Falcom", "",
    "Capcom", "HOT-B", "Jaleco", "Coconuts", "Coconuts", "", "Technos",
    "Pack-In-Video", "", "Starfish", "Gremlin Graphics", "Electronic Arts", "", "Cobra Team",
    "Human", "Koei", "Hudson Soft", "Yanoman", "Yanoman", "", "Tecmo", "", "Open System",
    "Virgin Games", "KSS", "Sunsoft", "Pow", "Micro World", "", "", "Enix", "Electro Brain",
    "Kemco", "Seta", "", "", "", "Visit", "", "", "Viacom", "Carrozzeria", "Dynamic",
    "", "Magifact", "DTMC or Hect", "", "", "", "", "", "Arcade Zone", "Empire Software",
    "Loriciel", "", "", "Seika", "Ubisoft", "", "", "", "", "System 3", "Spectrum HoloByte", "", "Irem",
    "", "Raya Systems","Renovation Products", "Malibu Games", "", "U.S. Gold", "Absolute",
    "Acclaim", "Activision", "American Sammy", "GameTek", "Hi Tech Entertainment", "LJN", "", "",
    "", "Mindscape", "", "", "Tradewest", "", "American Softworks", "Titus", "Virgin Interactive Entertainment",
	"Maxis", "Origin/FCI/Pony Canyon", "", "", "", "Ocean", "", "Electronic Arts", "", "Laser Beam", "", "",
	"Elite", "Electro Brain", "Infogrames", "Interplay", "LucasArts", "Parker Brothers", "Konami", "STORM", "", "",
	"T*HQ", "Accolade", "Triffix", "", "Microprose", "", "", "Kemco", "Misawa", "Teichio", "Namco",
	"Lozc", "Koei", "", "T.I.M.", "", "DATAM-Polystar", "", "", "Bullet-Proof Software", "Vic Tokai", "", "Character Soft",
	"I'Max", "Takara", "Chun Soft", "Video System", "Interbec", "", "Varie", "", "Kaneco", "", "Pack-In-Video",
	"Nichibutsu", "Tecmo", "Imagineer", "", "", "", "Telenet", "Hori", "", "", "Konami", "K. Amusement Leasing Co.",
	"", "Takara", "", "Technos", "JVC", "", "Toei Animation", "Toho K.K.", "", "Namco", "", "ASCII",
	"Bandai Entertainment", "", "Enix", "", "Halken", "", "", "", "Culture Brain", "Sunsoft", "Toshiba EMI",
    "Sony Imagesoft", "", "Sammy", "Taito", "", "Kemco", "Square", "Tokuma Soft", "Data East", "Tonkinhouse", "",
    "Koei", "", "Konami", "NTVIC", "", "Meldac", "Pony Canyon", "Sunrise", "Disco/Taito", "Sofel",
    "Quest", "Sigma*", "", "", "Naxat", "", "Capcom", "Banpresto", "Tomy", "Acclaim", "", "NCS",
    "Human", "Altron", "Jaleco", "", "Yutaka", "", "T&Esoft", "Epoch", "", "Athena", "Asmik", "Natsume",
    "King Records", "Atlus", "Sony Music Entertainment", "", "IGS", "", "", "Motown Software",
    "Left Field Entertainment", "Beam Software", "TecMagik", "", "", "", "", "Cybersoft", "", "", "",
    "", "Davidson", ""
};

static const char *region_names[] = {
	"Japan", "U.S.A.", "Europe/Oceania", "Scandinavia", "", "France", "Holland", "Spain", "Germany/Austria/Switzerland",
	"Italian", "Hong Kong/China", "Korea", "Common (?)", "Canada", "Brazil", "Nintendo Gateway System", "Australia"
};

static const char *type_names[] = {
	"LoROM", "HiROM", "Extended LoROM", "Extended HiROM", "BS-C LoROM", "BS-C HiROM"
};

const char *get_rom_company_name(rom_header_t &h)
{
	uint8_t c = h.company2;
	if (c == 0x33) c = h.company1;
	
	const char *n = company_names[c];
	
	return *n ? n : "Unknown";
}

const char *get_rom_region_name(rom_header_t &h)
{
	if (h.region > (sizeof(region_names) / sizeof(const char *)) || h.region == 5) return "Unknown";
	return region_names[h.region];
}

const char *get_rom_type_name(rom_info_t &h)
{
	if (h.type > (sizeof(type_names) / sizeof(const char *))) return "Unknown ROM";
	return type_names[h.type];
}
