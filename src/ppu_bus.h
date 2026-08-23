#ifndef PPU_BUS_H
#define PPU_BUS_H

#include "console.h"
#include <stdbool.h>

void ppu_bus_write(uint16_t address, uint8_t data, console_t *console);
uint8_t ppu_bus_read(uint16_t address, console_t *console);

#endif