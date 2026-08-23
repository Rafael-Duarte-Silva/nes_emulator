#include <stdlib.h>
#include "utils/log.h"
#include "console.h"
#include "cpu.h"
#include "ppu.h"
#include "cartrigde.h"

int main(int argc, char *argv[])
{
    LOG_SUCCESS("Console started\n");

    console_t console = {0};
    init_console(&console);

    cartrigde_t cartrigde = {0};
    init_cartrigde(&console, &cartrigde, argv[1]);

    cpu_t cpu = {0};
    init_cpu(&console, &cpu);

    ppu_t ppu = {0};
    init_ppu(&console, &ppu);

    LOG_SUCCESS("Console startup is complete\n");

    while (1)
    {
        run_instructions(&cpu);

        getchar();
    }

    free(cartrigde.mapper);
    free(cartrigde.PRG_ROM);
    free(cartrigde.CHR_ROM);

    return 0;
}
