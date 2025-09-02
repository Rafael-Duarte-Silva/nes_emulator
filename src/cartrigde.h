#ifndef CARTRIGDE_H
#define CARTRIGDE_H

#include "types.h"
#include "mapper.h"
#include "console.h"

typedef struct cartrigde {
    char signature[4]; // Should contain the value 'NES($1A)' (identify the file as an iNES file)
    ubyte PRG_ROM_counter; // Number of 16 KB PRG-ROM banks
    ubyte CHR_ROM_counter; // Number of 8 KB CHR-ROM / VROM banks
    ubyte ROM_CONTROL_ONE;
    ubyte ROM_CONTROL_TWO;
    ubyte RAM_counter; // Number of 8 KB RAM banks (For compatibility)

    console_t *console;
    mapper_t *mapper;
    ubyte *PRG_ROM;
    uint16_t PRG_ROM_size;
    ubyte *CHR_ROM;
    uint16_t CHR_ROM_size;
} cartrigde_t;

void init_cartrigde(console_t *console, cartrigde_t *cartrigde, const char rom_name[]);

#endif