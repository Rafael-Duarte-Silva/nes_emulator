#include <stdio.h>
#include "console.h"
#include "cpu.h"

int main(int argc, char *argv[]) {
    printf("running\n");

    cpu_t cpu = {0};
    init_cpu(&cpu);

    console_t console = {0};
    init_console(&console, &cpu, argv[1]);

    while (1)
    {
        printf("\n-------------- STEP --------------\n");
        getchar();
        run_instructions(&cpu);
    }

    return 0;
}
