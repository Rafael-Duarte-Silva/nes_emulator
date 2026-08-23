#ifndef PPU_H
#define PPU_H

#include "utils/log.h"
#include "types.h"
#include <stdbool.h>

typedef struct console console_t;

typedef struct
{
    bool v_blank_nmi;
    bool slave_select;
    bool sprite_size;
    bool background_table;
    bool sprite_table;
    bool vram_address;
    uint8_t base_nametable;
} ppu_ctrl_t;

typedef struct
{
    bool blue;
    bool green; // red on PAL/Dendy
    bool red;   // green on PAL/Dendy
    bool sprite_rendering;
    bool background_rendering;
    bool show_sprites;
    bool show_background;
    bool greyscale;
} ppu_mask_t;

typedef struct
{
    bool v_blank;
    bool sprite_0_hit;
    bool sprite_overflow;
} ppu_status_t;

typedef struct ppu
{
    console_t *console;

    uint16_t V; // Current VRAM address (15bits)
    uint16_t T; // Temporary VRAM address (15bits)
    ubyte X;    // Fine X scroll (3bits)
    bool W;     // First or second write toggle (1bit)

    ppu_ctrl_t ctrl;
    ppu_mask_t mask;
    ppu_status_t status;
    ubyte OAMADDR;
    ubyte PPUSCROLL;

    ubyte OAM[0xFF]; // sprite ram

    ubyte (*read_registers)(uint16_t address, struct ppu *ppu);
    void (*write_registers)(uint16_t address, ubyte data, struct ppu *ppu);
} ppu_t;

void init_ppu(console_t *console, ppu_t *ppu);
void reset_ppu(ppu_t *ppu);

ubyte read_registers(uint16_t address, ppu_t *ppu);
void write_registers(uint16_t address, ubyte data, ppu_t *ppu);

ubyte get_increment_vram(ubyte key);

void ppu_ctrl(ubyte data, ppu_t *ppu);
void ppu_mask(ubyte data, ppu_t *ppu);
ubyte ppu_status(ppu_t *ppu);
void oam_addr(ubyte address, ppu_t *ppu);
void oam_data_write(ubyte data, ppu_t *ppu);
ubyte oam_data_read(ppu_t *ppu);
void ppu_scroll(ubyte data, ppu_t *ppu);
void ppu_addr(ubyte address, ppu_t *ppu);
void ppu_data_write(ubyte data, ppu_t *ppu);
ubyte ppu_data_read(ppu_t *ppu);

#endif