#include "ppu.h"
#include "console.h"
#include <stdio.h>

void init_ppu(console_t *console, ppu_t *ppu) {
    console->PPU = ppu;
    ppu->console = console;

    reset_ppu(ppu);

    ppu->read_registers = read_registers;
    ppu->write_registers = write_registers;
}

void reset_ppu(ppu_t *ppu) {
    ppu->vblank = false;
    ppu->sprite0_hit = false;
    ppu->sprite_overflow = false;
    ppu->base_name_table = 0x0000;
    ppu->increment_vram = 0x00;
    ppu->sprite_pattern_table = 0x0000;
    ppu->background_pattern_table = 0x0000;
    ppu->sprite_size = 0x0000;
     ppu->master_slave_select = false;
    ppu->nmi = false;
}

ubyte ppu_read(uint16_t address, console_t *console){
    if(address < 0x2000){
        return console->cartrigde->mapper->read(address, console->cartrigde);
    }

    if(address < 0x3F00){
        return console->VRAM[address % 0x3000 - 0x3000];
    }
}

ubyte read_registers(uint16_t address, ppu_t *ppu){
    printf("\n\nPPU-address(read): %#X\n\n", address);

    switch (address)
    {
        case 0x2002:
            return ppu_status(ppu);

        case 0x2004:
            
            break;

        case 0x2007:
            
            break;
        
        default:
            break;
    }

    return 0x00;
}

void write_registers(uint16_t address, ubyte data, ppu_t *ppu){
    printf("\nPPU-address(write): %#X\n\n", address);

    switch (address)
    {
        case 0x2000:
            ppu_ctrl(data, ppu);
            return;

        case 0x2001:
            
            break;

        case 0x2003:
            
            break;

        case 0x2004:
            
            break;

        case 0x2005:
            
            break;

        case 0x2006:
            ppu_addr(data, ppu);
            break;

        case 0x2007:
            
            break;
        
        default:
            break;
    }
}

ubyte ppu_status(ppu_t *ppu){
    ubyte result = ppu->vblank << 7 | ppu->sprite0_hit << 6 | ppu->sprite_overflow << 5 & 0xFF;
    ppu->vblank = false;
    return result;
}

void ppu_ctrl(ubyte data, ppu_t *ppu){
    uint16_t base_name_table[] = {0x2000, 0x2400, 0x2800, 0x2C00};
    uint8_t increment_vram[] = {0x01, 0x20};
    uint16_t pattern_table [] = {0x0000, 0x1000};

    ppu->base_name_table = base_name_table[data & 0x03];
    ppu->increment_vram = increment_vram[data >> 2 & 0x01];
    ppu->sprite_pattern_table = pattern_table[data >> 3 & 0x01]; 
    ppu->background_pattern_table = pattern_table[data >> 4 & 0x01];
    ppu->sprite_size = data >> 5 & 0x01;
    ppu->master_slave_select = data >> 6 & 0x01;
    ppu->nmi = data >> 7 & 0x01;

    printf("base_name_table : %#X\n", ppu->base_name_table);
    printf("increment_vram : %#X\n", ppu->increment_vram);
    printf("background_pattern_table : %#X\n", ppu->background_pattern_table);
    printf("sprite_pattern_table : %#X\n", ppu->sprite_pattern_table);
    printf("sprite_size : %#X\n", ppu->sprite_size);
    printf("nmi : %#X\n\n", ppu->nmi);
}

void ppu_addr(ubyte data, ppu_t *ppu){
    if(ppu->W == 0){
        ppu->T = data;
        ppu->W = 1;
        return;
    }

    ppu->V = ppu->T << 8 | data;
    ppu->W = 0;
}