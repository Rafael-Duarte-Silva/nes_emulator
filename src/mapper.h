#ifndef MAPPER_H
#define MAPPER_H

#include "types.h"

typedef struct cartrigde cartrigde_t;

typedef struct {
    cartrigde_t *cartrigde;
    ubyte (*read)(uint16_t address, cartrigde_t *cartrigde);
    void (*write)(uint16_t address, ubyte data, cartrigde_t *cartrigde);
} mapper_t;

void init_mapper(cartrigde_t *cartrigde, ubyte mapper_type);

#endif