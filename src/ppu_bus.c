#include "ppu_bus.h"

void ppu_bus_write(uint16_t address, ubyte data, console_t *console)
{
    if (address < 0x2000)
    {
        return console->cartrigde->mapper->write(address, data, console->cartrigde);
    }

    if (address < 0x3F00)
    {
        console->VRAM[address % 0x3000] = data;
    }
}

ubyte ppu_bus_read(uint16_t address, console_t *console)
{
    if (address < 0x2000)
    {
        return console->cartrigde->mapper->read(address, console->cartrigde);
    }

    if (address < 0x3F00)
    {
        return console->VRAM[address % 0x3000];
    }
}