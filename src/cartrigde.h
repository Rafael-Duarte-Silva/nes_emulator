#ifndef CARTRIGDE_H
#define CARTRIGDE_H

#include "types.h"
#include "mapper.h"
#include "console.h"
#include <stdio.h>

typedef struct cartrigde
{
    char signature[4];     // Should contain the value 'NES($1A)' (identify the file as an iNES file)
    ubyte PRG_ROM_counter; // Number of 16 KB PRG-ROM banks
    ubyte CHR_ROM_counter; // Number of 8 KB CHR-ROM/CHR-RAM/VROM banks
    ubyte ROM_CONTROL_ONE;
    ubyte ROM_CONTROL_TWO;
    ubyte PRG_RAM_counter; // Number of 8 KB RAM (SRAM/WRAM extend) banks

    console_t *console;
    mapper_t *mapper;
    ubyte *PRG_ROM;
    uint32_t PRG_ROM_size;
    ubyte *CHR_ROM;
    uint16_t CHR_ROM_size;
    ubyte *PRG_RAM; // (SRAM/WRAM extend) which may or may not be battery backed
    uint16_t PRG_RAM_size;
} cartrigde_t;

void init_cartrigde(console_t *console, cartrigde_t *cartrigde, const char rom_name[]);

void load_cartrigde(cartrigde_t *cartrigde, const char rom_name[]);

FILE *open_rom(const char *rom_name);

long get_rom_size(FILE *rom, const char *rom_name);

bool read_header(FILE *rom, cartrigde_t *cartrigde, const char *rom_name);

size_t get_rom_offset(const cartrigde_t *cartrigde);
bool load_prg_rom(FILE *rom, cartrigde_t *cartrigde, long rom_size);
bool load_chr_rom(FILE *rom, cartrigde_t *cartrigde, long rom_size);
bool load_prg_ram(cartrigde_t *cartrigde);

void cleanup_prg(cartrigde_t *cartrigde);
void cleanup_chr(cartrigde_t *cartrigde);
void cleanup_prg_ram(cartrigde_t *cartrigde);

#endif