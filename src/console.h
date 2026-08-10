#ifndef CONSOLE_H
#define CONSOLE_H

#include "types.h"
#include "cpu.h"
#include "ppu.h"
#include "cartrigde.h"

typedef struct console {
    byte RAM[0x0800]; // WRAM (SRAM/motherboard)
    byte VRAM[0x0800]; // VRAM (motherboard)
    cpu_t *CPU;
    ppu_t *PPU;
    cartrigde_t *cartrigde;
} console_t;

void init_console(console_t *console);

#endif