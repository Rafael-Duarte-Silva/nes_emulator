#ifndef CONSOLE_H
#define CONSOLE_H

#include "types.h"
#include "cpu.h"

typedef struct console {
    cpu_t *CPU;
} console_t;

void init_console(console_t *console, cpu_t *cpu, const char rom_name[]);

#endif