#include "mapper1.h"
#include <stdlib.h>

void init_mapper1(cartrigde_t *cartrigde) {
    mapper1_t *mapper1 = malloc(sizeof(mapper1_t));
    if(!mapper1) return;

    mapper1->prg_bank1 = 0;
    mapper1->prg_bank2 = cartrigde->PRG_ROM_counter - 1;
    mapper1->chr_bank1 = 0;
    mapper1->chr_bank2 = cartrigde->CHR_ROM_counter - 1;

    mapper1->base.cartrigde = cartrigde;
    mapper1->base.read = read1;
    mapper1->base.write = write1;

    cartrigde->mapper = (mapper_t *)mapper1;
}

ubyte read1(uint16_t address, cartrigde_t *cartrigde) {
    mapper1_t *mapper1 = (mapper1_t *)cartrigde->mapper;

    return mapper1->prg_bank1;
}

void write1(uint16_t address, ubyte data, cartrigde_t *cartrigde) {
    // teoricamente o pego os dados vindos por aqui e trato eles por registradores
}