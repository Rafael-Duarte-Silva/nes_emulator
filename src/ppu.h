#ifndef PPU_H
#define PPU_H

#include "types.h"
#include <stdbool.h>

typedef struct console console_t;

typedef struct ppu {
    console_t *console;

    uint16_t V; // Current VRAM address
    uint16_t T; // Temporary VRAM address
    ubyte X; // Fine X scroll
    bool W; // First or second write toggle


    bool vblank;
    bool sprite0_hit;
    bool sprite_overflow;

    uint16_t base_name_table;
    uint8_t increment_vram;
    uint16_t sprite_pattern_table;
    uint16_t background_pattern_table;
    uint8_t sprite_size;
    bool master_slave_select;
    bool nmi;

    bool gray_scale;
    

    ubyte (*read_registers)(uint16_t address, struct ppu *ppu);
    void (*write_registers)(uint16_t address, ubyte data, struct ppu *ppu);
} ppu_t;

void init_ppu(console_t *console, ppu_t *ppu);
void reset_ppu(ppu_t *ppu);

ubyte read_registers(uint16_t address, ppu_t *ppu);
void write_registers(uint16_t address, ubyte data, ppu_t *ppu);

ubyte ppu_status(ppu_t *ppu);
void ppu_ctrl(ubyte data, ppu_t *ppu);
void ppu_addr(ubyte data, ppu_t *ppu);

#endif