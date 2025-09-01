#include "cpu.h"
#include "bus.h"
#include <stdio.h>
#include <string.h>

void init_cpu(console_t *console, cpu_t *cpu){
    void (*instructions[256])(cpu_t *cpu) = {
		brk, ora, NULL, NULL, nop, ora, asl, NULL,
		php, ora, asl, NULL, nop, ora, asl, NULL,
		bpl, ora, NULL, NULL, nop, ora, asl, NULL,
		clc, ora, nop, NULL, nop, ora, asl, NULL,
		jsr, and, NULL, NULL, bit, and, rol, NULL,
		plp, and, rol, NULL, bit, and, rol, NULL,
		bmi, and, NULL, NULL, nop, and, rol, NULL,
		sec, and, nop, NULL, nop, and, rol, NULL,
		rti, eor, NULL, NULL, nop, eor, lsr, NULL,
		pha, eor, lsr, NULL, jmp, eor, lsr, NULL,
		bvc, eor, NULL, NULL, nop, eor, lsr, NULL,
		cli, eor, nop, NULL, nop, eor, lsr, NULL,
		rts, adc, NULL, NULL, nop, adc, ror, NULL,
		pla, adc, ror, NULL, jmp, adc, ror, NULL,
		bvs, adc, NULL, NULL, nop, adc, ror, NULL,
		sei, adc, nop, NULL, nop, adc, ror, NULL,
		nop, sta, nop, NULL, sty, sta, stx, NULL,
		dey, nop, txa, NULL, sty, sta, stx, NULL,
		bcc, sta, NULL, NULL, sty, sta, stx, NULL,
		tya, sta, txs, NULL, NULL, sta, NULL, NULL,
		ldy, lda, ldx, NULL, ldy, lda, ldx, NULL,
		tay, lda, tax, NULL, ldy, lda, ldx, NULL,
		bcs, lda, NULL, NULL, ldy, lda, ldx, NULL,
		clv, lda, tsx, NULL, ldy, lda, ldx, NULL,
		cpy, cmp, nop, NULL, cpy, cmp, dec, NULL,
		iny, cmp, dex, NULL, cpy, cmp, dec, NULL,
		bne, cmp, NULL, NULL, nop, cmp, dec, NULL,
		cld, cmp, nop, NULL, nop, cmp, dec, NULL,
		cpx, sbc, nop, NULL, cpx, sbc, inc, NULL,
		inx, sbc, nop, sbc, cpx, sbc, inc, NULL,
		beq, sbc, NULL, NULL, nop, sbc, inc, NULL,
		sed, sbc, nop, NULL, nop, sbc, inc, NULL,
	};
    memcpy(cpu->instructions, instructions, sizeof(instructions));

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
    memcpy(cpu->instructions_sizes, instructions_sizes, sizeof(instructions_sizes));

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
    memcpy(cpu->instructions_modes, instructions_modes, sizeof(instructions_modes));

    ubyte instructions_cycles[256] = {
        7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    };
    memcpy(cpu->instructions_cycles, instructions_cycles, sizeof(instructions_cycles));

    ubyte instructions_pages_cycles[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    };
    memcpy(cpu->instructions_pages_cycles, instructions_pages_cycles, sizeof(instructions_pages_cycles));

    console->CPU = cpu;
    cpu->console = console;
    
    cpu->read = bus_read;
    cpu->write = bus_write;
    reset(cpu);
}

void reset(cpu_t *cpu){
    cpu->PC = read_address(cpu, 0xFFFC);
    cpu->SP = 0xFD;

    printf("PC: %#X\n", cpu->PC);

    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;

    cpu->C = false;
    cpu->Z = false;
    cpu->temp_I = true;
    cpu->I = cpu->temp_I;
    cpu->delay_I = false;
    cpu->D = false;
    cpu->B = false;
    cpu->V = false;
    cpu->N = false;
}

void run_instructions(cpu_t *cpu){
    if(cpu->delay_I == false){
        cpu->I = cpu->temp_I;
    } else {
        cpu->delay_I = false;
    }

    cpu->opcode = cpu->read(cpu->PC, cpu->console);
    cpu->mode = cpu->instructions_modes[cpu->opcode];
    cpu->cycles = cpu->instructions_cycles[cpu->opcode];

    printf("opcode: %#X\n", cpu->opcode);

    switch (cpu->mode)
    {
        case ABSOLUTE:
            printf("ABSOLUTE\n");
            cpu->address = read_address(cpu, cpu->PC + 1);
            break;
        case ABSOLUTE_X:
            printf("ABSOLUTE_X\n");
            cpu->address = page_crossed(cpu, read_address(cpu, cpu->PC + 1) + cpu->X);
            break;

        case ABSOLUTE_Y:
            printf("ABSOLUTE_Y\n");
            cpu->address = page_crossed(cpu, read_address(cpu, cpu->PC + 1) + cpu->Y);
            break;

        case ACCUMULATOR:
            printf("ACCUMULATOR\n");
            cpu->address = 0;
            break;

        case IMMEDIATE:
            printf("IMMEDIATE\n");
            cpu->address = cpu->PC + 1;
            break;

        case IMPLIED:
            printf("IMPLIED\n");
            cpu->address = 0;
            break;

        case INDEXED_INDIRECT:
            printf("INDEXED_INDIRECT\n");
            cpu->address = read_address(cpu, cpu->read(cpu->PC + 1, cpu->console) + cpu->X);
            break;

        case INDIRECT:
            printf("INDIRECT\n");
            cpu->address = read_address(cpu, read_address(cpu, cpu->PC + 1));
            break;

        case INDIRECT_INDEXED:
            printf("INDIRECT_INDEXED\n");
            cpu->address = page_crossed(
                cpu,
                read_address(cpu, cpu->read(cpu->PC + 1, cpu->console)) + cpu->Y
            );
            break;

        case RELATIVE:
            printf("RELATIVE\n");
            cpu->address = 0;
            break;

        case ZERO_PAGE:
            printf("ZERO_PAGE\n");
            cpu->address = cpu->read(cpu->PC + 1, cpu->console);
            break;

        case ZERO_PAGE_X:
            printf("ZERO_PAGE_X\n");
            cpu->address = (cpu->read(cpu->PC + 1, cpu->console) + cpu->X) % 0x00FF;
            break;

        case ZERO_PAGE_Y:
            printf("ZERO_PAGE_Y\n");
            cpu->address = (cpu->read(cpu->PC + 1, cpu->console) + cpu->Y) % 0x00FF;
            break;
        
        default:
            break;
    }

    printf("address: %#X\n", cpu->address);
    printf("size: %#X\n", cpu->instructions_sizes[cpu->opcode]);

    cpu->PC += cpu->instructions_sizes[cpu->opcode];

    if(cpu->instructions[cpu->opcode] != NULL){
        cpu->instructions[cpu->opcode](cpu);
    }

    printf("cycles: %#X\n", cpu->cycles);

    printf("register-PC: %#X\n", cpu->PC);
    printf("register-SP: %#X\n", cpu->SP);
    printf("register-A: %#X\n", cpu->A);
    printf("register-X: %#X\n", cpu->X);
    printf("register-Y: %#X\n", cpu->Y);
    printf("register(flag)-Z: %#X\n", cpu->Z);
    printf("register(flag)-N: %#X\n", cpu->N);
    printf("register(flag)-C: %#X\n", cpu->C);
    printf("register(flag)-V: %#X\n", cpu->V);
    printf("register(flag)-I: %#X\n", cpu->I);
    printf("register(flag)-B: %#X\n", cpu->B);
    printf("register(flag)-D: %#X\n", cpu->D);
    printf("mem-%d: %#X\n", cpu->address, cpu->read(cpu->address, cpu->console));
}

// -----------------------------
// COMMON
// -----------------------------

uint16_t read_address(cpu_t *cpu, uint16_t address){
    return cpu->read(address + 1, cpu->console) << 8 | cpu->read(address, cpu->console);
}

ubyte stack_pull(cpu_t *cpu) {
    cpu->SP++;
    uint16_t address = (cpu->SP % 0x10) + 0x0100;
    return cpu->read(address, cpu->console);
}

void stack_push(cpu_t *cpu, ubyte data) {
    uint16_t address = (cpu->SP % 0x10) + 0x0100;
    cpu->write(address, data, cpu->console);
    cpu->SP--;
}

uint16_t page_crossed(cpu_t *cpu, uint16_t new_address){
    if((new_address & 0xFF00) != (cpu->address & 0xFF00)) {
        printf("\n--- CROSSED PAGE ---\n\n");
        cpu->cycles += cpu->instructions_pages_cycles[cpu->opcode];
    }

    return new_address;
}

// -----------------------------
// DEBUG
// -----------------------------

void print_binary(ubyte value){
    for (int i = 7; i >= 0; i--) {
        printf("%d", value >> i & 0x01);
    }
}

// -----------------------------
// ACESS
// -----------------------------

void lda(cpu_t *cpu){
    printf("LDA\n");
    cpu->A = cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void sta(cpu_t *cpu){
    printf("STA\n");
    cpu->write(cpu->address, cpu->A, cpu->console);

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void ldx(cpu_t *cpu){
    printf("LDX\n");
    cpu->X = cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->X == 0;
    cpu->N = cpu->X >> 7;
}

void stx(cpu_t *cpu){
    printf("STX\n");
    cpu->write(cpu->address, cpu->X, cpu->console);

    cpu->Z = cpu->X == 0;
    cpu->N = cpu->X >> 7;
}

void ldy(cpu_t *cpu){
    printf("LDY\n");
    cpu->Y = cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->Y == 0;
    cpu->N = cpu->Y >> 7;
}

void sty(cpu_t *cpu){
    printf("STY\n");
    cpu->write(cpu->address, cpu->Y, cpu->console);

    cpu->Z = cpu->Y == 0;
    cpu->N = cpu->Y >> 7;
}

// -----------------------------
// TRANSFER
// -----------------------------

void tax(cpu_t *cpu){
    printf("TAX\n");
    cpu->X = cpu->A;

    cpu->Z = cpu->X == 0;
    cpu->N = cpu->X >> 7;
}

void txa(cpu_t *cpu){
    printf("TXA\n");
    cpu->A = cpu->X;

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void tay(cpu_t *cpu){
    printf("TAY\n");
    cpu->Y = cpu->A;

    cpu->Z = cpu->Y == 0;
    cpu->N = cpu->Y >> 7;
}

void tya(cpu_t *cpu){
    printf("TYA\n");
    cpu->A = cpu->Y;

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

// -----------------------------
// ARITHMETIC
// -----------------------------

void adc(cpu_t *cpu){
    printf("ADC\n");
    int16_t result = cpu->A + cpu->read(cpu->address, cpu->console) + cpu->C;
    cpu->C = result > 0xFF;
    cpu->V = (result ^ cpu->A) & (result ^ cpu->read(cpu->address, cpu->console)) & 0x80;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->A = result;
}

void sbc(cpu_t *cpu){
    printf("SBC\n");
    int16_t result = cpu->A - cpu->read(cpu->address, cpu->console) - !cpu->C;
    cpu->C = !(result < 0);
    cpu->V = (result ^ cpu->A) & (result ^ ~cpu->read(cpu->address, cpu->console)) & 0x80;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->A = result;
}

void inc(cpu_t *cpu){
    printf("INC\n");
    int16_t result = cpu->read(cpu->address, cpu->console) + 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->write(cpu->address, result, cpu->console);
}

void dec(cpu_t *cpu){
    printf("DEC\n");
    int16_t result = cpu->read(cpu->address, cpu->console) - 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->write(cpu->address, result, cpu->console);
}

void inx(cpu_t *cpu){
    printf("INX\n");
    int16_t result = cpu->X + 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->X = result;
}

void dex(cpu_t *cpu){
    printf("DEX\n");
    int16_t result = cpu->X - 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->X = result;
}

void iny(cpu_t *cpu){
    printf("INY\n");
    int16_t result = cpu->Y + 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->Y = result;
}

void dey(cpu_t *cpu){
    printf("DEY\n");
    int16_t result = cpu->Y - 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->Y = result;
}

// -----------------------------
// SHIFT
// -----------------------------

void asl(cpu_t *cpu){
    printf("ASL\n");

    if(cpu->mode == ACCUMULATOR){
        int16_t result = cpu->A << 1;
        cpu->C = cpu->A >> 7 & 0x01;
        cpu->Z = result == 0;
        cpu->N = result >> 7 & 0x1;

        cpu->A = result;
        return;
    }

    int16_t result = cpu->read(cpu->address, cpu->console) << 1;
    cpu->C = cpu->read(cpu->address, cpu->console) >> 7 & 0x01;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->write(cpu->address, result, cpu->console);
}

void lsr(cpu_t *cpu){
    printf("LSR\n");

    if(cpu->mode == ACCUMULATOR){
        int16_t result = cpu->A >> 1;
        cpu->C = cpu->A & 0x01;
        cpu->Z = result == 0;
        cpu->N = 0;

        cpu->A = result;
        return;
    }

    int16_t result = cpu->read(cpu->address, cpu->console) >> 1;
    cpu->C = cpu->read(cpu->address, cpu->console) & 0x01;
    cpu->Z = result == 0;
    cpu->N = 0;

    cpu->write(cpu->address, result, cpu->console);
}

void rol(cpu_t *cpu){
    printf("ROL\n");

    if(cpu->mode == ACCUMULATOR){
        int16_t result = cpu->A << 1 | cpu->C;
        cpu->C = cpu->A >> 7 & 0x01;
        cpu->Z = result == 0;
        cpu->N = result >> 7 & 0x1;

        cpu->A = result;
        return;
    }

    int16_t result = cpu->read(cpu->address, cpu->console) << 1 | cpu->C;
    cpu->C = cpu->read(cpu->address, cpu->console) >> 7 & 0x01;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->write(cpu->address, result, cpu->console);
}

void ror(cpu_t *cpu){
    printf("ROR\n");

    if(cpu->mode == ACCUMULATOR){
        int16_t result = cpu->A >> 1 | cpu->C;
        cpu->C = cpu->A & 0x01;
        cpu->Z = result == 0;
        cpu->N = result >> 7 & 0x1;

        cpu->A = result;
        return;
    }

    int16_t result = cpu->read(cpu->address, cpu->console) >> 1 | cpu->C;
    cpu->C = cpu->read(cpu->address, cpu->console) & 0x01;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->write(cpu->address, result, cpu->console);
}

// -----------------------------
// BITWISE
// -----------------------------

void and(cpu_t *cpu){
    printf("AND\n");
    cpu->A &= cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void ora(cpu_t *cpu){
    printf("ORA\n");
    cpu->A |= cpu->read(cpu->address, cpu->console);
    
    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void eor(cpu_t *cpu){
    printf("EOR\n");
    cpu->A ^= cpu->read(cpu->address, cpu->console);
    
    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void bit(cpu_t *cpu){
    printf("BIT\n");
    ubyte result = cpu->A & cpu->read(cpu->address, cpu->console);
    
    cpu->Z = result == 0;
    cpu->V = cpu->read(cpu->address, cpu->console) >> 6 & 0x02;
    cpu->N = cpu->read(cpu->address, cpu->console) >> 7;
}

// -----------------------------
// COMPARE
// -----------------------------

void cmp(cpu_t *cpu){
    printf("CMP\n");
    ubyte result = cpu->A - cpu->read(cpu->address, cpu->console);
    
    cpu->C = cpu->A >= cpu->read(cpu->address, cpu->console);
    cpu->Z = cpu->A == cpu->read(cpu->address, cpu->console);
    cpu->N = result >> 7;
}

void cpx(cpu_t *cpu){
    printf("CPX\n");
    ubyte result = cpu->X - cpu->read(cpu->address, cpu->console);
    
    cpu->C = cpu->X >= cpu->read(cpu->address, cpu->console);
    cpu->Z = cpu->X == cpu->read(cpu->address, cpu->console);
    cpu->N = result >> 7;
}

void cpy(cpu_t *cpu){
    printf("CPY\n");
    ubyte result = cpu->Y - cpu->read(cpu->address, cpu->console);
    
    cpu->C = cpu->Y >= cpu->read(cpu->address, cpu->console);
    cpu->Z = cpu->Y == cpu->read(cpu->address, cpu->console);
    cpu->N = result >> 7;
}

// -----------------------------
// BRANCH
// -----------------------------

void bcc(cpu_t *cpu){
    printf("BCC\n");
    
    if(!cpu->C){
        cpu->cycles++;
        cpu->address = page_crossed(cpu, cpu->PC - 1);

        cpu->PC += (byte)cpu->read(cpu->address, cpu->console);
        return;
    }
}

void bcs(cpu_t *cpu){
    printf("BSC\n");
    
    if(cpu->C){
        cpu->cycles++;
        cpu->address = page_crossed(cpu, cpu->PC - 1);

        cpu->PC += (byte)cpu->read(cpu->address, cpu->console); 
        return;
    }
}

void beq(cpu_t *cpu){
    printf("BEQ\n");
    
    if(cpu->Z){
        cpu->cycles++;
        cpu->address = page_crossed(cpu, cpu->PC - 1);

        cpu->PC += (byte)cpu->read(cpu->address, cpu->console); 
        return;
    }
}

void bne(cpu_t *cpu){
    printf("BNE\n");
    
    if(!cpu->Z){
        cpu->cycles++;
        cpu->address = page_crossed(cpu, cpu->PC - 1);

        cpu->PC += (byte)cpu->read(cpu->address, cpu->console); 
        return;
    }
}

void bpl(cpu_t *cpu){
    printf("BPL\n");
    
    if(!cpu->N){
        cpu->cycles++;
        cpu->address = page_crossed(cpu, cpu->PC - 1);

        cpu->PC += (byte)cpu->read(cpu->address, cpu->console); 
        return;
    }
}

void bmi(cpu_t *cpu){
    printf("BMI\n");
    
    if(cpu->N){
        cpu->cycles++;
        cpu->address = page_crossed(cpu, cpu->PC - 1);

        cpu->PC += (byte)cpu->read(cpu->address, cpu->console); 
        return;
    }
}

void bvc(cpu_t *cpu){
    printf("BVC\n");
    
    if(!cpu->V){
        cpu->cycles++;
        cpu->address = page_crossed(cpu, cpu->PC - 1);

        cpu->PC += (byte)cpu->read(cpu->address, cpu->console); 
        return;
    }
}

void bvs(cpu_t *cpu){
    printf("BVS\n");
    
    if(cpu->V){
        cpu->cycles++;
        cpu->address = page_crossed(cpu, cpu->PC - 1);

        cpu->PC += (byte)cpu->read(cpu->address, cpu->console); 
        return;
    }
}

// -----------------------------
// JUMP
// -----------------------------

void jmp(cpu_t *cpu){
    printf("JUMP\n");
    cpu->PC = cpu->address;
}

void jsr(cpu_t *cpu){
    printf("JSR\n");

    ubyte low = (cpu->PC - 1) & 0xFF;
    ubyte high = (cpu->PC - 1 >> 8) & 0xFF;

    stack_push(cpu, low);
    stack_push(cpu, high);

    cpu->PC = cpu->address;
}

void rts(cpu_t *cpu){
    printf("RTS\n");

    ubyte high = stack_pull(cpu);
    ubyte low = stack_pull(cpu);

    cpu->PC = high << 8 | low;
    cpu->PC++;
}

void brk(cpu_t *cpu){
    printf("BRK\n");

    ubyte low = (cpu->PC) & 0xFF;
    ubyte high = (cpu->PC >> 8) & 0xFF;

    stack_push(cpu, low);
    stack_push(cpu, high);

    cpu->PC = cpu->read(0xFFFE, cpu->console);

    ubyte P = cpu->N << 7 | cpu->V << 6 | 1 << 5 | 1 << 4 | cpu->D << 3 | cpu->I << 2 | cpu->Z << 1 | cpu->C;
    stack_push(cpu, P);

    printf("register(flags)-P: ");
    print_binary(P);
    printf("\n");

    cpu->temp_I = 1;
}

void rti(cpu_t *cpu){
    printf("RTI\n");

    ubyte P = stack_pull(cpu);
    ubyte high = stack_pull(cpu);
    ubyte low = stack_pull(cpu);

    printf("register(flags)-P: ");
    print_binary(P);
    printf("\n");

    cpu->PC = high << 8 | low;

    cpu->N = P >> 7 & 0x01;
    cpu->V = P >> 6 & 0x01;
    cpu->D = P >> 3 & 0x01;
    cpu->temp_I = P >> 2 & 0x01;
    cpu->Z = P >> 1 & 0x01;
    cpu->C = P & 0x01;
}

// -----------------------------
// STACK
// -----------------------------

void pha(cpu_t *cpu){
    printf("PHA\n");
    stack_push(cpu, cpu->A);
}

void pla(cpu_t *cpu){
    printf("PLA\n");
    cpu->A = stack_pull(cpu);
}

void php(cpu_t *cpu){
    printf("PHP\n");

    ubyte P = cpu->N << 7 | cpu->V << 6 | 1 << 5 | 1 << 4 | cpu->D << 3 | cpu->I << 2 | cpu->Z << 1 | cpu->C;
    stack_push(cpu, P);

    printf("register(flags)-P: ");
    print_binary(P);
    printf("\n");
}

void plp(cpu_t *cpu){
    printf("PLP\n");

    ubyte P = stack_pull(cpu);

    printf("register(flags)-P: ");
    print_binary(P);
    printf("\n");

    cpu->N = P >> 7 & 0x01;
    cpu->V = P >> 6 & 0x01;
    cpu->D = P >> 3 & 0x01;
    cpu->temp_I = P >> 2 & 0x01;
    cpu->delay_I = true;
    cpu->Z = P >> 1 & 0x01;
    cpu->C = P & 0x01;
}

void txs(cpu_t *cpu){
    printf("TXS\n");
    cpu->SP = cpu->X;
}

void tsx(cpu_t *cpu){
    printf("TSX\n");
    cpu->X = cpu->SP;
}

// -----------------------------
// FLAGS
// -----------------------------

void clc(cpu_t *cpu){
    printf("CLC\n");
    cpu->C = 0;
}

void sec(cpu_t *cpu){
    printf("SEC\n");
    cpu->C = 1;
}

void cli(cpu_t *cpu){
    printf("CLI\n");
    cpu->temp_I = 0;
    cpu->delay_I = true;
}

void sei(cpu_t *cpu){
    printf("SEI\n");
    cpu->temp_I = 1;
    cpu->delay_I = true;
}

void cld(cpu_t *cpu){
    printf("CLD\n");
    cpu->D = 0;
}

void sed(cpu_t *cpu){
    printf("SED\n");
    cpu->D = 1;
}

void clv(cpu_t *cpu){
    printf("CLV\n");
    cpu->V = 1;
}

// -----------------------------
// ORTHER
// -----------------------------

void nop(cpu_t *cpu){
    printf("NOP\n");
}