#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "stdbool.h"

typedef struct console console_t;

typedef enum {
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    ACCUMULATOR,
    IMMEDIATE,
    IMPLIED,
    INDEXED_INDIRECT,
    INDIRECT,
    INDIRECT_INDEXED,
    RELATIVE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
} modes_t;

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
    uint16_t address;
    modes_t mode;
    ubyte table_instructions_sizes[256];
    ubyte table_instructions_modes[256];
    byte (*read)(uint16_t address, console_t *console);
    void (*write)(uint16_t address, byte data, console_t *console);
} cpu_t;

void init_cpu(cpu_t *cpu);
void run_instructions(cpu_t *cpu);
void reset(cpu_t *cpu);

uint16_t read_address(cpu_t *cpu, uint16_t address);

void lda(cpu_t *cpu);

void nop(cpu_t *cpu);

#endif