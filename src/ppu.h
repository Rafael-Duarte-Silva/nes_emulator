#ifndef PPU_H
#define PPU_H

#include "utils/log.h"
#include <stdbool.h>
#include <stdint.h>

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
    uint8_t io; // Open bus state

    uint16_t V; // Current VRAM address (15bits)
    uint16_t T; // Temporary VRAM address (15bits)
    uint8_t X;  // Fine X scroll (3bits)
    bool W;     // First or second write toggle (1bit)

    ppu_ctrl_t ctrl;
    ppu_mask_t mask;
    ppu_status_t status;
    uint8_t OAMADDR;
    uint8_t PPUSCROLL;
    uint8_t PPUDATA; // Read buffer

    uint8_t pallete_RAM[0x20];
    uint8_t OAM[0xFF]; // Sprite RAM

    uint8_t (*read_registers)(uint16_t address, struct ppu *ppu);
    void (*write_registers)(uint16_t address, uint8_t data, struct ppu *ppu);
    uint8_t (*read)(uint16_t address, console_t *console);
    void (*write)(uint16_t address, uint8_t data, console_t *console);
} ppu_t;

void init_ppu(console_t *console, ppu_t *ppu);
void reset_ppu(ppu_t *ppu);

uint8_t read_registers(uint16_t address, ppu_t *ppu);
void write_registers(uint16_t address, uint8_t data, ppu_t *ppu);

uint8_t get_increment_vram(uint8_t key);

void ppu_ctrl(uint8_t data, ppu_t *ppu);
void ppu_mask(uint8_t data, ppu_t *ppu);
uint8_t ppu_status(ppu_t *ppu);
void oam_addr(uint8_t address, ppu_t *ppu);
void oam_data_write(uint8_t data, ppu_t *ppu);
uint8_t oam_data_read(ppu_t *ppu);
void ppu_scroll(uint8_t data, ppu_t *ppu);
void ppu_addr(uint8_t address, ppu_t *ppu);
void ppu_data_write(uint8_t data, ppu_t *ppu);
uint8_t ppu_data_read(ppu_t *ppu);

#endif