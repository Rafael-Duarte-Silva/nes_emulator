#include "bus.h"

uint8_t bus_read(uint16_t address, console_t *console)
{
    if (address < 0x2000)
    {
        return console->RAM[address % 0X0800];
    }

    if (address < 0x4000)
    {
        return console->PPU->read_registers(address % 8 + 0x2000, console->PPU);
    }

    if (address >= 0x4020)
    {
        return console->cartrigde->mapper->read(address, console->cartrigde);
    }
}

void bus_write(uint16_t address, uint8_t data, console_t *console)
{
    if (address < 0x2000)
    {
        console->RAM[address % 0X0800] = data;
        return;
    }

    if (address < 0x4000)
    {
        console->PPU->write_registers(address % 8 + 0x2000, data, console->PPU);
        return;
    }

    if (address >= 0x4020)
    {
        console->cartrigde->mapper->write(address, data, console->cartrigde);
        return;
    }
}