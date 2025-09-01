#include "console.h"
#include "cpu.h"

void init_console(console_t *console, cpu_t *cpu, cartrigde_t *cartrigde){
    console->CPU = cpu;
    console->CARTRIGDE = cartrigde;
    console->CPU->console = console;
}