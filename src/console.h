#ifndef CONSOLE_H
#define CONSOLE_H

#include "cpu.h"
#include "ppu.h"
#include "cartrigde.h"

typedef struct console
{
    int8_t RAM[0x0800];  // WRAM (SRAM/motherboard)
    int8_t VRAM[0x0800]; // VRAM (motherboard)
    cpu_t *CPU;
    ppu_t *PPU;
    cartrigde_t *cartrigde;
} console_t;

void init_console(console_t *console);

#endif