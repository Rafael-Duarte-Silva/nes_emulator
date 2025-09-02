#include "mapper.h"
#include "mapper0.h"
#include <stdio.h>

void init_mapper(cartrigde_t *cartrigde, ubyte mapper_type) {
    switch (mapper_type)
    {
        case 0:
            printf("\nMAPPER-0\n\n");
            init_mapper0(cartrigde);
            break;
            
        default:
            break;
    }
}