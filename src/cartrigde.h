#ifndef CARTRIGDE_H
#define CARTRIGDE_H

#include "types.h"

typedef struct {
    char signature[3]; // Should contain the value 'NES' (identify the file as an iNES file)
    ubyte format; // Should contain the value $1A (also as an identifier)
    ubyte PRG_ROM_counter; // Number of 16 KB PRG-ROM banks
    ubyte CHR_ROM_counter; // Number of 8 KB CHR-ROM / VROM banks
    uint16_t ROM_CONTROL;
    ubyte RAM_counter; // Number of 8 KB RAM banks (For compatibility)
} cartrigde_t;

void init_cartrigde(cartrigde_t *cartrigde, const char rom_name[]);

#endif