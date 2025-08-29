#ifndef BUS_H
#define BUS_H

#include "console.h"
#include "types.h"

ubyte bus_read(uint16_t address, console_t *console);
void bus_write(uint16_t address, ubyte data, console_t *console);

#endif