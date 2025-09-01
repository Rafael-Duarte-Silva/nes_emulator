#include "bus.h"
#include <stdio.h>

ubyte bus_read(uint16_t address, console_t *console) {
    if(address < 0x2000) {
        return console->RAM[address % 0X0800];
    }
}

void bus_write(uint16_t address, ubyte data, console_t *console){
   if(address < 0x2000) {
        console->RAM[address % 0X0800] = data;
        return;
    }
}