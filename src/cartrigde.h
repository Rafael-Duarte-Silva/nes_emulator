#ifndef CARTRIGDE_H
#define CARTRIGDE_H

#include "types.h"
#include "mapper.h"
#include "console.h"

typedef struct cartrigde {
    char signature[3]; // Should contain the value 'NES' (identify the file as an iNES file)
    ubyte format; // Should contain the value $1A (also as an identifier)
    ubyte PRG_ROM_counter; // Number of 16 KB PRG-ROM banks
    ubyte CHR_ROM_counter; // Number of 8 KB CHR-ROM / VROM banks
    ubyte ROM_CONTROL_ONE;
    ubyte ROM_CONTROL_TWO;
    ubyte RAM_counter; // Number of 8 KB RAM banks (For compatibility)

    console_t *console;
    mapper_t *mapper;
    ubyte *prg_rom;
    uint16_t prg_rom_size;
    ubyte *chr_rom;
    uint16_t chr_rom_size;
} cartrigde_t;

void init_cartrigde(console_t *console, cartrigde_t *cartrigde, const char rom_name[]);

#endif