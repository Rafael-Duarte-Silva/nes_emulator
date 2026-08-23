#include "mapper0.h"
#include <stdlib.h>

void init_mapper0(cartrigde_t *cartrigde)
{
    mapper0_t *mapper0 = malloc(sizeof(mapper0_t));
    if (mapper0 == NULL)
        return;

    mapper0->base.cartrigde = cartrigde;
    mapper0->base.read = read0;
    mapper0->base.write = write0;

    if (cartrigde->PRG_ROM_counter == 1)
    {
        mapper0->suffix = 128;
    }
    else if (cartrigde->PRG_ROM_counter == 2)
    {
        mapper0->suffix = 256;
    }

    cartrigde->mapper = (mapper_t *)mapper0;
}

uint8_t read0(uint16_t address, cartrigde_t *cartrigde)
{
    if (address < 0x2000)
    {
        return cartrigde->CHR_ROM[address];
    }

    if (address >= 0x8000)
    {
        mapper0_t *mapper0 = (mapper0_t *)cartrigde->mapper;

        if (mapper0->suffix == 128)
        {
            return cartrigde->PRG_ROM[address % 0x4000];
        }
        else if (mapper0->suffix == 256)
        {
            return cartrigde->PRG_ROM[address % 0x8000];
        }
    }
}

void write0(uint16_t address, uint8_t data, cartrigde_t *cartrigde)
{
    if (address < 0x2000)
    {
        cartrigde->CHR_ROM[address] = data;
        return;
    }

    if (address >= 0x8000)
    {
        mapper0_t *mapper0 = (mapper0_t *)cartrigde->mapper;

        if (mapper0->suffix == 128)
        {
            cartrigde->PRG_ROM[address % 0x4000] = data;
            return;
        }
        else if (mapper0->suffix == 256)
        {
            cartrigde->PRG_ROM[address % 0x8000] = data;
            return;
        }
    }
}