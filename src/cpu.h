#ifndef CPU_H
#define CPU_H

#include "utils/log.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct console console_t;

typedef enum
{
    ABSOLUTE = 1,
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
} ADDRESS_MODE;

typedef struct cpu
{
    // Registers
    uint16_t PC; // Program Counter
    uint8_t SP;  // Stack Pointer
    uint8_t A;   // Accumulator
    uint8_t X;   // Index Register
    uint8_t Y;   // Index Register

    // Flags (Processor Status(P))
    bool C;      // Carry
    bool Z;      // Zero
    bool I;      // Interrupt Disable
    bool temp_I; //
    bool delay_I;
    bool D; // Decimal Mode
    bool B; // Break Command
    bool V; // Overflow
    bool N; // Negative

    console_t *console;
    uint8_t opcode;
    uint16_t address;
    ADDRESS_MODE mode;
    uint8_t cycles;

    void (*instructions[256])(struct cpu *cpu);
    uint8_t instructions_sizes[256];
    uint8_t instructions_modes[256];
    uint8_t instructions_cycles[256];

    uint8_t (*read)(uint16_t address, console_t *console);
    void (*write)(uint16_t address, uint8_t data, console_t *console);
} cpu_t;

void init_cpu(console_t *console, cpu_t *cpu);
void reset_cpu(cpu_t *cpu);
void set_address_modes(cpu_t *cpu);
void run_instructions(cpu_t *cpu);

// -----------------------------
// COMMON
// -----------------------------

uint16_t read_address(cpu_t *cpu, uint16_t address);
uint8_t stack_pull(cpu_t *cpu);
void stack_push(cpu_t *cpu, uint8_t data);
uint16_t page_crossed(cpu_t *cpu, uint16_t new_address);

// -----------------------------
// DEBUG
// -----------------------------

void print_binary(uint8_t value);

// -----------------------------
// ACESS
// -----------------------------

void lda(cpu_t *cpu);
void sta(cpu_t *cpu);
void ldx(cpu_t *cpu);
void stx(cpu_t *cpu);
void ldy(cpu_t *cpu);
void sty(cpu_t *cpu);

// -----------------------------
// TRANSFER
// -----------------------------

void tax(cpu_t *cpu);
void txa(cpu_t *cpu);
void tay(cpu_t *cpu);
void tya(cpu_t *cpu);

// -----------------------------
// ARITHMETIC
// -----------------------------

void adc(cpu_t *cpu);
void sbc(cpu_t *cpu);
void inc(cpu_t *cpu);
void dec(cpu_t *cpu);
void inx(cpu_t *cpu);
void dex(cpu_t *cpu);
void iny(cpu_t *cpu);
void dey(cpu_t *cpu);

// -----------------------------
// SHIFT
// -----------------------------

void asl(cpu_t *cpu);
void lsr(cpu_t *cpu);
void rol(cpu_t *cpu);
void ror(cpu_t *cpu);

// -----------------------------
// BITWISE
// -----------------------------

void and(cpu_t *cpu);
void ora(cpu_t *cpu);
void eor(cpu_t *cpu);
void bit(cpu_t *cpu);

// -----------------------------
// COMPARE
// -----------------------------

void cmp(cpu_t *cpu);
void cpx(cpu_t *cpu);
void cpy(cpu_t *cpu);

// -----------------------------
// BRANCH
// -----------------------------

void bcc(cpu_t *cpu);
void bcs(cpu_t *cpu);
void beq(cpu_t *cpu);
void bne(cpu_t *cpu);
void bpl(cpu_t *cpu);
void bmi(cpu_t *cpu);
void bvc(cpu_t *cpu);
void bvs(cpu_t *cpu);

// -----------------------------
// JUMP
// -----------------------------

void jmp(cpu_t *cpu);
void jsr(cpu_t *cpu);
void rts(cpu_t *cpu);
void brk(cpu_t *cpu);
void rti(cpu_t *cpu);

// -----------------------------
// STACK
// -----------------------------

void pha(cpu_t *cpu);
void pla(cpu_t *cpu);
void php(cpu_t *cpu);
void plp(cpu_t *cpu);
void txs(cpu_t *cpu);
void tsx(cpu_t *cpu);

// -----------------------------
// FLAGS
// -----------------------------

void clc(cpu_t *cpu);
void sec(cpu_t *cpu);
void cli(cpu_t *cpu);
void sei(cpu_t *cpu);
void cld(cpu_t *cpu);
void sed(cpu_t *cpu);
void clv(cpu_t *cpu);

// -----------------------------
// OTHER
// -----------------------------

void nop(cpu_t *cpu);

#endif