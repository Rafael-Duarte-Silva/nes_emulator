#ifndef PPU_BUS_H
#define PPU_BUS_H

#include "types.h"
#include "console.h"
#include <stdbool.h>

void ppu_bus_write(uint16_t address, ubyte data, console_t *console);
ubyte ppu_bus_read(uint16_t address, console_t *console);

#endif