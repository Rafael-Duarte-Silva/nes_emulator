#include "cpu.h"
#include "bus.h"
#include <string.h>

void init_cpu(console_t *console, cpu_t *cpu)
{
    // clang-format off
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
    // clang-format on
    memcpy(cpu->instructions, instructions, sizeof(instructions));

    // clang-format off
    uint8_t instructions_sizes[256] = {
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
    // clang-format on
    memcpy(cpu->instructions_sizes, instructions_sizes, sizeof(instructions_sizes));

    // clang-format off
    uint8_t instructions_modes[256] = {
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
    // clang-format on
    memcpy(cpu->instructions_modes, instructions_modes, sizeof(instructions_modes));

    // clang-format off
    uint8_t instructions_cycles[256] = {
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
    // clang-format on    
    memcpy(cpu->instructions_cycles, instructions_cycles, sizeof(instructions_cycles));

    console->CPU = cpu;
    cpu->console = console;

    cpu->read = bus_read;
    cpu->write = bus_write;
    reset_cpu(cpu);
}

void reset_cpu(cpu_t *cpu)
{
    cpu->PC = read_address(cpu, 0xFFFC);
    cpu->SP = 0xFD;
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

void set_address_modes(cpu_t *cpu)
{
    switch (cpu->mode)
    {
    case ABSOLUTE:
        LOG_DEBUG("ABSOLUTE");
        cpu->address = read_address(cpu, cpu->PC + 1);
        return;
    case ABSOLUTE_X:
        LOG_DEBUG("ABSOLUTE_X");
        cpu->address = page_crossed(cpu, read_address(cpu, cpu->PC + 1) + cpu->X);
        return;

    case ABSOLUTE_Y:
        LOG_DEBUG("ABSOLUTE_Y");
        cpu->address = page_crossed(cpu, read_address(cpu, cpu->PC + 1) + cpu->Y);
        return;

    case ACCUMULATOR:
        LOG_DEBUG("ACCUMULATOR");
        cpu->address = 0;
        return;

    case IMMEDIATE:
        LOG_DEBUG("IMMEDIATE");
        cpu->address = cpu->PC + 1;
        return;

    case IMPLIED:
        LOG_DEBUG("IMPLIED");
        cpu->address = 0;
        return;

    case INDEXED_INDIRECT:
        LOG_DEBUG("INDEXED_INDIRECT");
        cpu->address = read_address(cpu, cpu->read(cpu->PC + 1, cpu->console) + cpu->X);
        return;

    case INDIRECT:
        LOG_DEBUG("INDIRECT");
        cpu->address = read_address(cpu, read_address(cpu, cpu->PC + 1));
        break;

    case INDIRECT_INDEXED:
        LOG_DEBUG("INDIRECT_INDEXED");
        cpu->address = page_crossed(
            cpu,
            read_address(cpu, cpu->read(cpu->PC + 1, cpu->console)) + cpu->Y);
        return;

    case RELATIVE:
        LOG_DEBUG("RELATIVE");
        cpu->address = 0;
        return;

    case ZERO_PAGE:
        LOG_DEBUG("ZERO_PAGE");
        cpu->address = cpu->read(cpu->PC + 1, cpu->console);
        return;

    case ZERO_PAGE_X:
        LOG_DEBUG("ZERO_PAGE_X");
        cpu->address = (cpu->read(cpu->PC + 1, cpu->console) + cpu->X) % 0x00FF;
        return;

    case ZERO_PAGE_Y:
        LOG_DEBUG("ZERO_PAGE_Y");
        cpu->address = (cpu->read(cpu->PC + 1, cpu->console) + cpu->Y) % 0x00FF;
        return;

    default:
        break;
    }
}

void run_instructions(cpu_t *cpu)
{
    LOG_DEBUG("--- CPU START CYCLE ---", cpu->address);

    if (!cpu->delay_I)
    {
        cpu->I = cpu->temp_I;
    }
    else
    {
        cpu->delay_I = false;
    }

    cpu->opcode = cpu->read(cpu->PC, cpu->console);
    cpu->mode = cpu->instructions_modes[cpu->opcode];
    cpu->cycles = cpu->instructions_cycles[cpu->opcode];

    set_address_modes(cpu);

    cpu->PC += cpu->instructions_sizes[cpu->opcode];

    void (*instruction)(struct cpu *cpu) = cpu->instructions[cpu->opcode];
    if (instruction != NULL)
    {
        instruction(cpu);
    }

    LOG_DEBUG("address: %#X", cpu->address);
    LOG_DEBUG("opcode: %#X", cpu->opcode);
    LOG_DEBUG("size: %#X", cpu->instructions_sizes[cpu->opcode]);
    LOG_DEBUG("cycles: %#X", cpu->cycles);
    LOG_DEBUG("register-PC: %#X", cpu->PC);
    LOG_DEBUG("register-SP: %#X", cpu->SP);
    LOG_DEBUG("register-A: %#X", cpu->A);
    LOG_DEBUG("register-X: %#X", cpu->X);
    LOG_DEBUG("register-Y: %#X", cpu->Y);
    LOG_DEBUG("register(flag)-Z: %#X", cpu->Z);
    LOG_DEBUG("register(flag)-N: %#X", cpu->N);
    LOG_DEBUG("register(flag)-C: %#X", cpu->C);
    LOG_DEBUG("register(flag)-V: %#X", cpu->V);
    LOG_DEBUG("register(flag)-I: %#X", cpu->I);
    LOG_DEBUG("register(flag)-B: %#X", cpu->B);
    LOG_DEBUG("register(flag)-D: %#X", cpu->D);
    LOG_DEBUG("mem-%d: %#X", cpu->address, cpu->read(cpu->address, cpu->console));
    LOG_DEBUG("--- CPU END CYCLE ---\n", cpu->address);
}

// -----------------------------
// COMMON
// -----------------------------

uint16_t read_address(cpu_t *cpu, uint16_t address)
{
    return cpu->read(address + 1, cpu->console) << 8 | cpu->read(address, cpu->console);
}

uint8_t stack_pull(cpu_t *cpu)
{
    cpu->SP++;
    uint16_t address = cpu->SP + 0x0100;
    return cpu->read(address, cpu->console);
}

void stack_push(cpu_t *cpu, uint8_t data)
{
    uint16_t address = cpu->SP + 0x0100;
    cpu->write(address, data, cpu->console);
    cpu->SP--;
}

uint16_t page_crossed(cpu_t *cpu, uint16_t new_address)
{
    if ((new_address & 0xFF00) != (cpu->address & 0xFF00))
    {
        LOG_DEBUG("CROSSED PAGE");
        cpu->cycles++;
    }

    return new_address;
}

// -----------------------------
// DEBUG
// -----------------------------

void print_binary(uint8_t value)
{
    for (int i = 7; i >= 0; i--)
    {
        LOG_DEBUG("%d", value >> i & 0x01);
    }
}

// -----------------------------
// ACCESS
// -----------------------------

void lda(cpu_t *cpu)
{
    LOG_DEBUG("LDA");
    cpu->A = cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void sta(cpu_t *cpu)
{
    LOG_DEBUG("STA");
    cpu->write(cpu->address, cpu->A, cpu->console);
}

void ldx(cpu_t *cpu)
{
    LOG_DEBUG("LDX");
    cpu->X = cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->X == 0;
    cpu->N = cpu->X >> 7;
}

void stx(cpu_t *cpu)
{
    LOG_DEBUG("STX");
    cpu->write(cpu->address, cpu->X, cpu->console);
}

void ldy(cpu_t *cpu)
{
    LOG_DEBUG("LDY");
    cpu->Y = cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->Y == 0;
    cpu->N = cpu->Y >> 7;
}

void sty(cpu_t *cpu)
{
    LOG_DEBUG("STY");
    cpu->write(cpu->address, cpu->Y, cpu->console);
}

// -----------------------------
// TRANSFER
// -----------------------------

void tax(cpu_t *cpu)
{
    LOG_DEBUG("TAX");
    cpu->X = cpu->A;

    cpu->Z = cpu->X == 0;
    cpu->N = cpu->X >> 7;
}

void txa(cpu_t *cpu)
{
    LOG_DEBUG("TXA");
    cpu->A = cpu->X;

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void tay(cpu_t *cpu)
{
    LOG_DEBUG("TAY");
    cpu->Y = cpu->A;

    cpu->Z = cpu->Y == 0;
    cpu->N = cpu->Y >> 7;
}

void tya(cpu_t *cpu)
{
    LOG_DEBUG("TYA");
    cpu->A = cpu->Y;

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

// -----------------------------
// ARITHMETIC
// -----------------------------

void adc(cpu_t *cpu)
{
    LOG_DEBUG("ADC");
    int16_t result = cpu->A + cpu->read(cpu->address, cpu->console) + cpu->C;
    cpu->C = result > 0xFF;
    cpu->V = (result ^ cpu->A) & (result ^ cpu->read(cpu->address, cpu->console)) & 0x80;
    cpu->Z = (uint8_t)result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->A = result;
}

void sbc(cpu_t *cpu)
{
    LOG_DEBUG("SBC");
    int16_t result = cpu->A - cpu->read(cpu->address, cpu->console) - !cpu->C;
    cpu->C = !(result < 0);
    cpu->V = (result ^ cpu->A) & (result ^ ~cpu->read(cpu->address, cpu->console)) & 0x80;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->A = result;
}

void inc(cpu_t *cpu)
{
    LOG_DEBUG("INC");
    int16_t result = cpu->read(cpu->address, cpu->console) + 1;
    cpu->Z = (uint8_t)result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->write(cpu->address, result, cpu->console);
}

void dec(cpu_t *cpu)
{
    LOG_DEBUG("DEC");
    int16_t result = cpu->read(cpu->address, cpu->console) - 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->write(cpu->address, result, cpu->console);
}

void inx(cpu_t *cpu)
{
    LOG_DEBUG("INX");
    int16_t result = cpu->X + 1;
    cpu->Z = (uint8_t)result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->X = result;
}

void dex(cpu_t *cpu)
{
    LOG_DEBUG("DEX");
    int16_t result = cpu->X - 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->X = result;
}

void iny(cpu_t *cpu)
{
    LOG_DEBUG("INY");
    int16_t result = cpu->Y + 1;
    cpu->Z = (uint8_t)result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->Y = result;
}

void dey(cpu_t *cpu)
{
    LOG_DEBUG("DEY");
    int16_t result = cpu->Y - 1;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->Y = result;
}

// -----------------------------
// SHIFT
// -----------------------------

void asl(cpu_t *cpu)
{
    LOG_DEBUG("ASL");

    if (cpu->mode == ACCUMULATOR)
    {
        int16_t result = cpu->A << 1;
        cpu->C = cpu->A >> 7 & 0x01;
        cpu->Z = (uint8_t)result == 0;
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

void lsr(cpu_t *cpu)
{
    LOG_DEBUG("LSR");

    if (cpu->mode == ACCUMULATOR)
    {
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

void rol(cpu_t *cpu)
{
    LOG_DEBUG("ROL");

    if (cpu->mode == ACCUMULATOR)
    {
        uint8_t result = cpu->A << 1 | cpu->C;
        cpu->C = cpu->A >> 7 & 0x01;
        cpu->Z = result == 0;
        cpu->N = result >> 7 & 0x1;

        cpu->A = result;
        return;
    }

    uint8_t val = cpu->read(cpu->address, cpu->console);
    uint8_t result = val << 1 | cpu->C;
    cpu->C = val >> 7 & 0x01;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x1;

    cpu->write(cpu->address, result, cpu->console);
}

void ror(cpu_t *cpu)
{
    LOG_DEBUG("ROR");

    if (cpu->mode == ACCUMULATOR)
    {
        uint8_t result = cpu->A >> 1 | cpu->C << 7;
        cpu->C = cpu->A & 0x01;
        cpu->Z = result == 0;
        cpu->N = result >> 7 & 0x01;

        cpu->A = result;
        return;
    }

    uint8_t val = cpu->read(cpu->address, cpu->console);
    uint8_t result = val >> 1 | cpu->C << 7;
    cpu->C = val & 0x01;
    cpu->Z = result == 0;
    cpu->N = result >> 7 & 0x01;

    cpu->write(cpu->address, result, cpu->console);
}

// -----------------------------
// BITWISE
// -----------------------------

void and(cpu_t *cpu)
{
    LOG_DEBUG("AND");
    cpu->A &= cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void ora(cpu_t *cpu)
{
    LOG_DEBUG("ORA");
    cpu->A |= cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void eor(cpu_t *cpu)
{
    LOG_DEBUG("EOR");
    cpu->A ^= cpu->read(cpu->address, cpu->console);

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void bit(cpu_t *cpu)
{
    LOG_DEBUG("BIT");
    uint8_t result = cpu->A & cpu->read(cpu->address, cpu->console);

    cpu->Z = result == 0;
    cpu->V = cpu->read(cpu->address, cpu->console) >> 6 & 0x01;
    cpu->N = cpu->read(cpu->address, cpu->console) >> 7;
}

// -----------------------------
// COMPARE
// -----------------------------

void cmp(cpu_t *cpu)
{
    LOG_DEBUG("CMP");
    uint8_t result = cpu->A - cpu->read(cpu->address, cpu->console);

    cpu->C = cpu->A >= cpu->read(cpu->address, cpu->console);
    cpu->Z = cpu->A == cpu->read(cpu->address, cpu->console);
    cpu->N = result >> 7;
}

void cpx(cpu_t *cpu)
{
    LOG_DEBUG("CPX");
    uint8_t result = cpu->X - cpu->read(cpu->address, cpu->console);

    cpu->C = cpu->X >= cpu->read(cpu->address, cpu->console);
    cpu->Z = cpu->X == cpu->read(cpu->address, cpu->console);
    cpu->N = result >> 7;
}

void cpy(cpu_t *cpu)
{
    LOG_DEBUG("CPY");
    uint8_t result = cpu->Y - cpu->read(cpu->address, cpu->console);

    cpu->C = cpu->Y >= cpu->read(cpu->address, cpu->console);
    cpu->Z = cpu->Y == cpu->read(cpu->address, cpu->console);
    cpu->N = result >> 7;
}

// -----------------------------
// BRANCH
// -----------------------------

void bcc(cpu_t *cpu)
{
    LOG_DEBUG("BCC");

    if (!cpu->C)
    {
        cpu->cycles++;
        cpu->PC += (int8_t)cpu->read(cpu->address, cpu->console);
        page_crossed(cpu, cpu->PC);
        return;
    }
}

void bcs(cpu_t *cpu)
{
    LOG_DEBUG("BSC");

    if (cpu->C)
    {
        cpu->cycles++;
        cpu->PC += (int8_t)cpu->read(cpu->address, cpu->console);
        page_crossed(cpu, cpu->PC);
        return;
    }
}

void beq(cpu_t *cpu)
{
    LOG_DEBUG("BEQ");

    if (cpu->Z)
    {
        cpu->cycles++;
        cpu->PC += (int8_t)cpu->read(cpu->address, cpu->console);
        page_crossed(cpu, cpu->PC);
        return;
    }
}

void bne(cpu_t *cpu)
{
    LOG_DEBUG("BNE");

    if (!cpu->Z)
    {
        cpu->cycles++;
        cpu->PC += (int8_t)cpu->read(cpu->address, cpu->console);
        page_crossed(cpu, cpu->PC);
        return;
    }
}

void bpl(cpu_t *cpu)
{
    LOG_DEBUG("BPL");

    if (!cpu->N)
    {
        cpu->cycles++;
        cpu->PC += (int8_t)cpu->read(cpu->address, cpu->console);
        page_crossed(cpu, cpu->PC);
        return;
    }
}

void bmi(cpu_t *cpu)
{
    LOG_DEBUG("BMI");

    if (cpu->N)
    {
        cpu->cycles++;
        cpu->PC += (int8_t)cpu->read(cpu->address, cpu->console);
        page_crossed(cpu, cpu->PC);
        return;
    }
}

void bvc(cpu_t *cpu)
{
    LOG_DEBUG("BVC");

    if (!cpu->V)
    {
        cpu->cycles++;
        cpu->PC += (int8_t)cpu->read(cpu->address, cpu->console);
        page_crossed(cpu, cpu->PC);
        return;
    }
}

void bvs(cpu_t *cpu)
{
    LOG_DEBUG("BVS");

    if (cpu->V)
    {
        cpu->cycles++;
        cpu->PC += (int8_t)cpu->read(cpu->address, cpu->console);
        page_crossed(cpu, cpu->PC);
        return;
    }
}

// -----------------------------
// JUMP
// -----------------------------

void jmp(cpu_t *cpu)
{
    LOG_DEBUG("JUMP");
    cpu->PC = cpu->address;
}

void jsr(cpu_t *cpu)
{
    LOG_DEBUG("JSR");

    uint8_t low = (cpu->PC - 1) & 0xFF;
    uint8_t high = ((cpu->PC - 1) >> 8) & 0xFF;

    stack_push(cpu, low);
    stack_push(cpu, high);

    cpu->PC = cpu->address;
}

void rts(cpu_t *cpu)
{
    LOG_DEBUG("RTS");

    uint8_t low = stack_pull(cpu);
    uint8_t high = stack_pull(cpu);

    cpu->PC = high << 8 | low;
    cpu->PC++;
}

void brk(cpu_t *cpu)
{
    LOG_DEBUG("BRK");

    uint8_t high = (cpu->PC >> 8) & 0xFF;
    uint8_t low = (cpu->PC) & 0xFF;
    uint8_t P = cpu->N << 7 | cpu->V << 6 | 1 << 5 | 1 << 4 | cpu->D << 3 | cpu->I << 2 | cpu->Z << 1 | cpu->C;

    stack_push(cpu, high);
    stack_push(cpu, low);
    stack_push(cpu, P);

    LOG_DEBUG("register(flags)-P: ");
    print_binary(P);

    cpu->PC = read_address(cpu, 0xFFFE);

    cpu->temp_I = true;
}

void rti(cpu_t *cpu)
{
    LOG_DEBUG("RTI");

    uint8_t P = stack_pull(cpu);
    uint8_t low = stack_pull(cpu);
    uint8_t high = stack_pull(cpu);

    LOG_DEBUG("register(flags)-P: ");
    print_binary(P);

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

void pha(cpu_t *cpu)
{
    LOG_DEBUG("PHA");
    stack_push(cpu, cpu->A);
}

void pla(cpu_t *cpu)
{
    LOG_DEBUG("PLA");
    cpu->A = stack_pull(cpu);

    cpu->Z = cpu->A == 0;
    cpu->N = cpu->A >> 7;
}

void php(cpu_t *cpu)
{
    LOG_DEBUG("PHP");

    uint8_t P = cpu->N << 7 | cpu->V << 6 | 1 << 5 | 1 << 4 | cpu->D << 3 | cpu->I << 2 | cpu->Z << 1 | cpu->C;
    stack_push(cpu, P);

    LOG_DEBUG("register(flags)-P: ");
    print_binary(P);
}

void plp(cpu_t *cpu)
{
    LOG_DEBUG("PLP");

    uint8_t P = stack_pull(cpu);

    LOG_DEBUG("register(flags)-P: ");
    print_binary(P);

    cpu->N = P >> 7 & 0x01;
    cpu->V = P >> 6 & 0x01;
    cpu->D = P >> 3 & 0x01;
    cpu->temp_I = P >> 2 & 0x01;
    cpu->delay_I = true;
    cpu->Z = P >> 1 & 0x01;
    cpu->C = P & 0x01;
}

void txs(cpu_t *cpu)
{
    LOG_DEBUG("TXS");
    cpu->SP = cpu->X;
}

void tsx(cpu_t *cpu)
{
    LOG_DEBUG("TSX");
    cpu->X = cpu->SP;

    cpu->Z = cpu->X == 0;
    cpu->N = cpu->X >> 7;
}

// -----------------------------
// FLAGS
// -----------------------------

void clc(cpu_t *cpu)
{
    LOG_DEBUG("CLC");
    cpu->C = false;
}

void sec(cpu_t *cpu)
{
    LOG_DEBUG("SEC");
    cpu->C = true;
}

void cli(cpu_t *cpu)
{
    LOG_DEBUG("CLI");
    cpu->temp_I = false;
    cpu->delay_I = true;
}

void sei(cpu_t *cpu)
{
    LOG_DEBUG("SEI");
    cpu->temp_I = true;
    cpu->delay_I = true;
}

void cld(cpu_t *cpu)
{
    LOG_DEBUG("CLD");
    cpu->D = false;
}

void sed(cpu_t *cpu)
{
    LOG_DEBUG("SED");
    cpu->D = true;
}

void clv(cpu_t *cpu)
{
    LOG_DEBUG("CLV");
    cpu->V = false;
}

// -----------------------------
// OTHER
// -----------------------------

void nop(cpu_t *cpu)
{
    (void)cpu;
    LOG_DEBUG("NOP");
}