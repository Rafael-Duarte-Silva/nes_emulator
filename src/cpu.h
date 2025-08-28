#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "stdbool.h"

typedef struct console console_t;

typedef struct {
    // Registers
    uint16_t PC; // Program Counter
    ubyte SP; // Stack Pointer
    byte A; // Accumulator
    byte X; // Index Register
    byte Y; // Index Register

    //Flags (Processor Status(P))
    bool C; // Carry
    bool Z; // Zero
    bool I; // Interrupt Disable
    bool D; // Decimal Mode
    bool B; // Break Command
    bool V; // Overflow
    bool N; // Negative

    console_t *console;
} cpu_t;

void init_cpu(cpu_t *cpu);
void execute(cpu_t *cpu);

#endif