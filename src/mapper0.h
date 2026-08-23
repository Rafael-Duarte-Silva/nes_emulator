#ifndef MAPPER0_H
#define MAPPER0_H

#include "cartrigde.h"
#include "mapper.h"

typedef struct
{
    mapper_t base;
    uint16_t suffix; // 128 or 256 kilobits (NES-NROM-128/NES-NROM-256)
} mapper0_t;

void init_mapper0(cartrigde_t *cartrigde);

uint8_t read0(uint16_t address, cartrigde_t *cartrigde);
void write0(uint16_t address, uint8_t data, cartrigde_t *cartrigde);

#endif