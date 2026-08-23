#include "utils/log.h"
#include "mapper.h"
#include "mapper0.h"

void init_mapper(cartrigde_t *cartrigde, uint8_t mapper_type)
{
    switch (mapper_type)
    {
    case 0:
        LOG_DEBUG("MAPPER-0 has been initialized\n");
        init_mapper0(cartrigde);
        return;

    default:
        break;
    }
}