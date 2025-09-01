#ifndef MAPPER1_H
#define MAPPER1_H

#include "types.h"
#include "cartrigde.h"
#include "mapper.h"

typedef struct {
    mapper_t base;

    ubyte prg_bank1;
    ubyte prg_bank2;
    ubyte chr_bank1;
    ubyte chr_bank2;
} mapper1_t;

void init_mapper1(cartrigde_t *cartrigde);

ubyte read1(uint16_t address, cartrigde_t *cartrigde);
void write1(uint16_t address, ubyte data, cartrigde_t *cartrigde);

#endif