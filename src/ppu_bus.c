#include "ppu_bus.h"

uint8_t ppu_bus_read(uint16_t address, console_t *console)
{
    if (address < 0x2000)
    {
        return console->cartrigde->mapper->read(address, console->cartrigde);
    }

    if (address < 0x3F00)
    {
        return console->VRAM[address % 0x0800];
    }

    if (address >= 0x3F00)
    {
        console->PPU->pallete_RAM[address % 0x20];
    }
}

void ppu_bus_write(uint16_t address, uint8_t data, console_t *console)
{
    if (address < 0x2000)
    {
        console->cartrigde->mapper->write(address, data, console->cartrigde);
        return;
    }

    if (address < 0x3F00)
    {
        console->VRAM[address % 0x0800] = data;
        return;
    }

    if (address >= 0x3F00)
    {
        console->PPU->pallete_RAM[address % 0x20] = data;
    }
}