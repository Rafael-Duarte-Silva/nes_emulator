#ifndef BUS_H
#define BUS_H

#include "console.h"

uint8_t bus_read(uint16_t address, console_t *console);
void bus_write(uint16_t address, uint8_t data, console_t *console);

#endif