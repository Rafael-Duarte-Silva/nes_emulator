#include "cpu.h"
#include "bus.h"
#include "stdio.h"
#include "string.h"

void init_cpu(cpu_t *cpu){
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
    memcpy(cpu->table_instructions, instructions, sizeof(instructions));

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
    memcpy(cpu->table_instructions_sizes, instructions_sizes, sizeof(instructions_sizes));

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
    memcpy(cpu->table_instructions_modes, instructions_modes, sizeof(instructions_modes));

    cpu->read = bus_read;
    cpu->write = bus_write;
    reset(cpu);
}

void reset(cpu_t *cpu){
    cpu->PC = 0x00;
    //cpu->PC = read_address(cpu, 0xFFFC);
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

    printf("opcode: %#X\n", opcode);

    switch (cpu->mode)
    {
        case ABSOLUTE:
            printf("ABSOLUTE\n");
            cpu->address = read_address(cpu, cpu->PC  + 1);
            break;
        case ABSOLUTE_X:
            printf("ABSOLUTE_X\n");
            cpu->address = read_address(cpu, cpu->PC  + 1) + cpu->X;
            break;

        case ABSOLUTE_Y:
            printf("ABSOLUTE_Y\n");
            cpu->address = read_address(cpu, cpu->PC  + 1) + cpu->Y;
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
            cpu->address = read_address(cpu, cpu->read(cpu->read(cpu->PC + 1, cpu->console), cpu->console)) + cpu->Y;
            break;

        case RELATIVE:
            printf("RELATIVE\n");
            // NOT IMPLEMENTED
            break;

        case ZERO_PAGE:
            printf("ZERO_PAGE\n");
            cpu->address = cpu->read(cpu->PC + 1, cpu->console);
            break;

        case ZERO_PAGE_X:
            printf("ZERO_PAGE_X\n");
            cpu->address = cpu->read(cpu->PC + 1, cpu->console) + cpu->X;
            break;

        case ZERO_PAGE_Y:
            printf("ZERO_PAGE_Y\n");
            cpu->address = cpu->read(cpu->PC + 1, cpu->console) + cpu->Y;
            break;
        
        default:
            break;
    }

    printf("address: %#X\n", cpu->address);

    if(cpu->table_instructions[opcode] != NULL){
        cpu->table_instructions[opcode](cpu);
    }

    printf("mem-%d: %#X\n", cpu->address, cpu->read(cpu->address, cpu->console));
    printf("register-A: %#X\n", cpu->A);
    printf("register-X: %#X\n", cpu->X);
    printf("register-Y: %#X\n", cpu->Y);
    printf("register(flag)-Z: %#X\n", cpu->Z);
    printf("register(flag)-N: %#X\n", cpu->N);
    printf("register(flag)-C: %#X\n", cpu->C);
    printf("register(flag)-V: %#X\n", cpu->V);

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
    // NOT IMPLEMENTED
}

void cpx(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void cpy(cpu_t *cpu){
    // NOT IMPLEMENTED
}

// -----------------------------
// BRANCH
// -----------------------------

void bcc(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void bcs(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void beq(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void bne(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void bpl(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void bmi(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void bvc(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void bvs(cpu_t *cpu){
    // NOT IMPLEMENTED
}

// -----------------------------
// JUMP
// -----------------------------

void jmp(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void jsr(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void rts(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void brk(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void rti(cpu_t *cpu){
    // NOT IMPLEMENTED
}

// -----------------------------
// STACK
// -----------------------------

void pha(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void pla(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void php(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void plp(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void txs(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void tsx(cpu_t *cpu){
    // NOT IMPLEMENTED
}

// -----------------------------
// FLAGS
// -----------------------------

void clc(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void sec(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void cli(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void sei(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void cld(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void sed(cpu_t *cpu){
    // NOT IMPLEMENTED
}

void clv(cpu_t *cpu){
    // NOT IMPLEMENTED
}

// -----------------------------
// ORTHER
// -----------------------------

void nop(cpu_t *cpu){

}