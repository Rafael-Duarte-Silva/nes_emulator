#include <stdio.h>
#include "console.h"
#include "cpu.h"
#include "cartrigde.h"

int main(int argc, char *argv[]) {
    printf("running\n");

    cpu_t cpu = {0};
    init_cpu(&cpu);

    cartrigde_t cartrigde = {0};
    init_cartrigde(&cartrigde, argv[1]);

    console_t console = {0};
    init_console(&console, &cpu, &cartrigde);

    while (1)
    {
        printf("\n-------------- STEP --------------\n");
        run_instructions(&cpu);

        getchar();
    }

    return 0;
}
