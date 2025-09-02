#include "mapper0.h"
#include <stdlib.h>
#include <stdio.h>

void init_mapper0(cartrigde_t *cartrigde) {
    mapper0_t *mapper0 = malloc(sizeof(mapper0_t));
    if(!mapper0) return;

    mapper0->base.cartrigde = cartrigde;
    mapper0->base.read = read0;
    mapper0->base.write = write0;

    cartrigde->mapper = (mapper_t *)mapper0;
}

ubyte read0(uint16_t address, cartrigde_t *cartrigde) {
    return cartrigde->prg_rom[address - cartrigde->prg_rom_size];
}

void write0(uint16_t address, ubyte data, cartrigde_t *cartrigde) {
    // theoretically this function takes the data coming from here and processes it using registers
}