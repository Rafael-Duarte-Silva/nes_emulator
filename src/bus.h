#ifndef BUS_H
#define BUS_H

#include "console.h"
#include "types.h"

byte bus_read(uint16_t address, console_t *console);
void bus_write(uint16_t address, byte data, console_t *console);

#endif