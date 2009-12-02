/*
 *  tables.h
 *  smcdis
 *
 *  Created by Alexander Strange on 12/21/07.
 *
 */

#ifndef __TABLES_H__
#define __TABLES_H__
#include <stdint.h>
#include "smc.h"

extern const char *get_rom_company_name(rom_header_t &h);
extern const char *get_rom_region_name(rom_header_t &h);
extern const char *get_rom_type_name(rom_info_t &h);
#endif