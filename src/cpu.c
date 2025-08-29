#include "cpu.h"
#include "bus.h"
#include "stdio.h"
#include "string.h"

void init_cpu(cpu_t *cpu){
    ubyte instructions_sizes[256] = {
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
        3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
        1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
        1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,
        2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    };
    memcpy(cpu->table_instructions_sizes, instructions_sizes, 256);

    ubyte instructions_modes[256] = {
        6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        1, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
        5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
        5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    };
    memcpy(cpu->table_instructions_modes, instructions_modes, 256);

    cpu->read = bus_read;
    cpu->write = bus_write;
    reset(cpu);
}

void reset(cpu_t *cpu){
    cpu->PC = read_address(cpu, 0xFFFC);
    cpu->SP = 0xFD;

    cpu->C = false;
    cpu->Z = false;
    cpu->I = false;
    cpu->D = false;
    cpu->B = false;
    cpu->V = false;
    cpu->N = false;
}

void run_instructions(cpu_t *cpu){
    ubyte opcode = cpu->read(cpu->PC, cpu->console);
    cpu->mode = cpu->table_instructions_modes[opcode];

    switch (cpu->mode)
    {
        case ABSOLUTE:
            cpu->address = read_address(cpu, cpu->PC  + 1);
            break;
        case ABSOLUTE_X:
            cpu->address = read_address(cpu, cpu->PC  + 1) + cpu->X;
            break;

        case ABSOLUTE_Y:
            cpu->address = read_address(cpu, cpu->PC  + 1) + cpu->Y;
            break;

        case ACCUMULATOR:
            cpu->address = 0;
            break;

        case IMMEDIATE:
            cpu->address = cpu->PC + 1;
            break;

        case IMPLIED:
            cpu->address = 0;
            break;

        case INDEXED_INDIRECT:
            cpu->address = read_address(cpu, cpu->read(cpu->PC + 1, cpu->console) + cpu->X);
            break;

        case INDIRECT:
            cpu->address = read_address(cpu, read_address(cpu, cpu->PC + 1));
            break;

        case INDIRECT_INDEXED:
            cpu->address = read_address(cpu, cpu->read(cpu->read(cpu->PC + 1, cpu->console), cpu->console)) + cpu->Y;
            break;

        case RELATIVE:
            // NOT IMPLEMENTED
            break;

        case ZERO_PAGE:
            cpu->address = cpu->read(cpu->PC + 1, cpu->console);
            break;

        case ZERO_PAGE_X:
            cpu->address = cpu->read(cpu->PC + 1, cpu->console) + cpu->X;
            break;

        case ZERO_PAGE_Y:
            cpu->address = cpu->read(cpu->PC + 1, cpu->console) + cpu->Y;
            break;
        
        default:
            break;
    }

    cpu->PC += cpu->table_instructions_sizes[opcode];
}

// -----------------------------
// COMMON
// -----------------------------

uint16_t read_address(cpu_t *cpu, uint16_t address){
    return cpu->read(address + 1, cpu->console) << 8 | cpu->read(address, cpu->console);
}

// -----------------------------
// ACESS
// -----------------------------

void lda(cpu_t *cpu){
    cpu->A = cpu->read(cpu->address, cpu->console);
}

// -----------------------------
// ORTHER
// -----------------------------

void nop(cpu_t *cpu){

}