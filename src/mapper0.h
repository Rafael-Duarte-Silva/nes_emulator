#ifndef MAPPER0_H
#define MAPPER0_H

#include "types.h"
#include "cartrigde.h"
#include "mapper.h"

typedef struct {
    mapper_t base;
} mapper0_t;

void init_mapper0(cartrigde_t *cartrigde);

ubyte read0(uint16_t address, cartrigde_t *cartrigde);
void write0(uint16_t address, ubyte data, cartrigde_t *cartrigde);

#endif