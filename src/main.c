#include <stdio.h>
#include <stdlib.h>
#include "console.h"
#include "cpu.h"
#include "cartrigde.h"

int main(int argc, char *argv[]) {
    printf("running\n");

    console_t console = {0};
    init_console(&console);

    cpu_t cpu = {0};
    cartrigde_t cartrigde = {0};
    init_cartrigde(&console, &cartrigde, argv[1]);
    init_cpu(&console, &cpu);

    while (1)
    {
        printf("\n-------------- STEP --------------\n");
        run_instructions(&cpu);

        getchar();
    }

    free(cartrigde.mapper);
    free(cartrigde.PRG_ROM);
    free(cartrigde.CHR_ROM);

    return 0;
}
