/*
 * ============================================================================
 * CPU TESTS
 * ============================================================================
 *
 * This file contains tests for the CPU module.
 *
 * These tests are self-contained and focus exclusively on CPU functionality.
 * They do not depend on or require interaction with other system components.
 *
 * ============================================================================
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"

// -----------------------------
// MOCKS
// -----------------------------

uint8_t memory[0x10000];

ubyte bus_read(uint16_t address, console_t *console)
{
    (void)console;
    return memory[address];
}

void bus_write(uint16_t address, ubyte data, console_t *console)
{
    (void)console;
    // This is the only mock bus write path: the data is stored at this address.
    memory[address] = data;
}

// -----------------------------
// HELPERS
// -----------------------------

static void write_test_memory(uint16_t address, ubyte data)
{
    memory[address] = data;
}

static ubyte read_test_memory(uint16_t address)
{
    return memory[address];
}

// Values used repeatedly across the tests.
// The "u" suffix makes it explicit that these are unsigned constants.
#define TEST_RAM_ADDR 0x1000u           // Arbitrary RAM address used by simple tests.
#define TEST_BRANCH_PC 0x0202u          // PC after the branch instruction/operand has been read.
#define TEST_ZERO 0x00u                 // Zero: should set Z and leave N clear.
#define TEST_MAX_BYTE 0xFFu             // Largest 8-bit value.
#define TEST_MAX_NEGATIVE 0x80u         // Bit 7 = 1: represents a negative value in 8-bit signed representation.
#define TEST_MAX_POSITIVE 0x7Fu         // Largest positive signed value in 8 bits.
#define TEST_STACK_PAGE 0x0100u         // Stack page on the 6502: $0100-$01FF.
#define TEST_STACK_POINTER 0x0Fu        // Convenient initial position within the stack page.
#define TEST_STACK_POINTER_DATA 0x0110u // Convenient starting position for data within the stack page.

/**
 * Configures the memory operand used by the instruction.
 *
 * Expected:
 *   - "address" is the effective address already calculated by the test.
 *   - "data" is the byte that the instruction is expected to read.
 */
static void set_test_operand(cpu_t *cpu, uint16_t address, ubyte data)
{
    cpu->address = address;
    write_test_memory(address, data);
}

/**
 * Checks the Z (zero) and N (negative) flags together.
 *
 * Use when the expected behavior depends only on the 8-bit result.
 */
static void assert_zn(const cpu_t *cpu, bool expected_zero, bool expected_negative)
{
    assert(cpu->Z == expected_zero);
    assert(cpu->N == expected_negative);
}

/**
 * Sets all flags to true
 */
static void set_all_flags(cpu_t *cpu)
{
    cpu->N = true;
    cpu->V = true;
    cpu->D = true;
    cpu->I = true;
    cpu->Z = true;
    cpu->C = true;
}

/**
 * Checks a memory location without repeating the expression "memory[address]".
 */
static void assert_memory_data(uint16_t address, ubyte expected)
{
    assert(read_test_memory(address) == expected);
}

// -----------------------------
// SETUP
// -----------------------------

static void cpu_test_init(cpu_t *cpu)
{
    cpu->read = bus_read;
    cpu->write = bus_write;
    cpu->console = NULL;

    reset_cpu(cpu);
}

// -----------------------------
// ACCESS
// -----------------------------

static void test_lda(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, 0x42);

    lda(&cpu);

    assert(cpu.A == 0x42);
    assert_zn(&cpu, false, false);
}

static void test_lda_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    lda(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_lda_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_NEGATIVE);

    lda(&cpu);

    assert(cpu.A == TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

static void test_sta(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x55;
    cpu.address = TEST_RAM_ADDR;

    sta(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 0x55);
}

static void test_ldx(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, 0x33);

    ldx(&cpu);

    assert(cpu.X == 0x33);
    assert_zn(&cpu, false, false);
}

static void test_ldx_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, TEST_ZERO);

    ldx(&cpu);

    assert(cpu.X == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_ldx_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, TEST_MAX_NEGATIVE);

    ldx(&cpu);

    assert(cpu.X == TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

static void test_stx(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = 0x33;
    cpu.address = TEST_RAM_ADDR;

    stx(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 0x33);
}

static void test_ldy(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, 0x77);

    ldy(&cpu);

    assert(cpu.Y == 0x77);
    assert_zn(&cpu, false, false);
}

static void test_ldy_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, TEST_ZERO);

    ldy(&cpu);

    assert(cpu.Y == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_ldy_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, TEST_MAX_NEGATIVE);

    ldy(&cpu);

    assert(cpu.Y == TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

static void test_sty(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = 0x77;
    cpu.address = TEST_RAM_ADDR;

    sty(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 0x77);
}

// -----------------------------
// TRANSFER
// -----------------------------

static void test_tax(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x42;

    tax(&cpu);

    assert(cpu.X == 0x42);
    assert_zn(&cpu, false, false);
}

static void test_tax_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_ZERO;

    tax(&cpu);

    assert(cpu.X == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_tax_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_NEGATIVE;

    tax(&cpu);

    assert(cpu.X == TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

static void test_txa(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = 0x42;

    txa(&cpu);

    assert(cpu.A == 0x42);
    assert_zn(&cpu, false, false);
}

static void test_txa_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = TEST_ZERO;

    txa(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_txa_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = TEST_MAX_BYTE;

    txa(&cpu);

    assert(cpu.A == TEST_MAX_BYTE);
    assert_zn(&cpu, false, true);
}

static void test_tay(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x42;

    tay(&cpu);

    assert(cpu.Y == 0x42);
    assert_zn(&cpu, false, false);
}

static void test_tay_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_ZERO;

    tay(&cpu);

    assert(cpu.Y == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_tay_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_NEGATIVE;

    tay(&cpu);

    assert(cpu.Y == TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

static void test_tya(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = 0x42;

    tya(&cpu);

    assert(cpu.A == 0x42);
    assert_zn(&cpu, false, false);
}

static void test_tya_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = TEST_ZERO;

    tya(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_tya_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = TEST_MAX_BYTE;

    tya(&cpu);

    assert(cpu.A == TEST_MAX_BYTE);
    assert_zn(&cpu, false, true);
}

static void test_txs(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = 0x0A;

    txs(&cpu);

    assert(cpu.SP == 0x0A);
}

static void test_tsx(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = 0x0A;

    tsx(&cpu);

    assert(cpu.X == 0x0A);
    assert_zn(&cpu, false, false);
}

static void test_tsx_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_ZERO;

    tsx(&cpu);

    assert(cpu.X == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_tsx_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_MAX_NEGATIVE;

    tsx(&cpu);

    assert(cpu.X == TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

// -----------------------------
// ARITHMETIC
// -----------------------------

static void test_adc_wrap(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    cpu.C = false;
    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, 1);

    adc(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == true);
    assert(cpu.V == false);
    assert_zn(&cpu, true, false);
}

static void test_adc_overflow(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_POSITIVE;
    cpu.C = false;
    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, 1);

    adc(&cpu);

    assert(cpu.A == TEST_MAX_NEGATIVE);
    assert(cpu.C == false);
    assert(cpu.V == true);
    assert_zn(&cpu, false, true);
}

static void test_adc_no_carry(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 10;
    cpu.C = false;
    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, 20);

    adc(&cpu);

    assert(cpu.A == 30);
    assert(cpu.C == false);
    assert(cpu.V == false);
    assert_zn(&cpu, false, false);
}

static void test_adc_carry_in(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x10;
    cpu.C = true;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x20);

    adc(&cpu);

    assert(cpu.A == 0x31);
    assert(cpu.C == false);
    assert(cpu.V == false);
    assert_zn(&cpu, false, false);
}

static void test_adc_carry_in_wrap(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    cpu.C = true;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    adc(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == true);
    assert(cpu.V == false);
    assert_zn(&cpu, true, false);
}

static void test_adc_negative_plus_negative_overflow(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_NEGATIVE;
    cpu.C = false;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_NEGATIVE);

    adc(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == true);
    assert(cpu.V == true);
    assert_zn(&cpu, true, false);
}

static void test_adc_negative_plus_positive(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_NEGATIVE;
    cpu.C = false;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x01);

    adc(&cpu);

    assert(cpu.A == 0x81);
    assert(cpu.C == false);
    assert(cpu.V == false);
    assert_zn(&cpu, false, true);
}

static void test_adc_positive_boundary(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_POSITIVE;
    cpu.C = false;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    adc(&cpu);

    assert(cpu.A == TEST_MAX_POSITIVE);
    assert(cpu.C == false);
    assert(cpu.V == false);
    assert_zn(&cpu, false, false);
}

static void test_adc_byte_max_plus_byte_max(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    cpu.C = false;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_BYTE);

    adc(&cpu);

    assert(cpu.A == 0xFE);
    assert(cpu.C == true);
    assert(cpu.V == false);
    assert_zn(&cpu, false, true);
}

static void test_sbc(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 30;
    cpu.C = true;
    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, 10);

    sbc(&cpu);

    assert(cpu.A == 20);
    assert(cpu.C == true);
    assert(cpu.V == false);
    assert_zn(&cpu, false, false);
}

static void test_sbc_borrow(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 10;
    cpu.C = true;
    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, 20);

    sbc(&cpu);

    assert(cpu.A == 0xF6);
    assert(cpu.C == false);
    assert(cpu.V == false);
    assert_zn(&cpu, false, true);
}

static void test_sbc_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x42;
    cpu.C = true;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x42);

    sbc(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == true);
    assert(cpu.V == false);
    assert_zn(&cpu, true, false);
}

static void test_sbc_no_carry_equal_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x42;
    cpu.C = false;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x41);

    sbc(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == true);
    assert(cpu.V == false);
    assert_zn(&cpu, true, false);
}

static void test_sbc_zero_minus_one(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_ZERO;
    cpu.C = true;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x01);

    sbc(&cpu);

    assert(cpu.A == TEST_MAX_BYTE);
    assert(cpu.C == false);
    assert(cpu.V == false);
    assert_zn(&cpu, false, true);
}

static void test_sbc_negative_overflow(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_NEGATIVE;
    cpu.C = true;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x01);

    sbc(&cpu);

    assert(cpu.A == TEST_MAX_POSITIVE);
    assert(cpu.C == true);
    assert(cpu.V == true);
    assert_zn(&cpu, false, false);
}

static void test_sbc_positive_overflow(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_POSITIVE;
    cpu.C = true;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_BYTE);

    sbc(&cpu);

    assert(cpu.A == TEST_MAX_NEGATIVE);
    assert(cpu.C == false);
    assert(cpu.V == true);
    assert_zn(&cpu, false, true);
}

static void test_sbc_byte_max_plus_zero_plus_carry_in(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    cpu.C = true;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    sbc(&cpu);

    assert(cpu.A == TEST_MAX_BYTE);
    assert(cpu.C == true);
    assert(cpu.V == false);
    assert_zn(&cpu, false, true);
}

static void test_inc(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, 10);

    inc(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 11);
    assert_zn(&cpu, false, false);
}

static void test_inc_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_BYTE);

    inc(&cpu);

    assert_memory_data(TEST_RAM_ADDR, TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_inc_wrap(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = true;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_BYTE);

    inc(&cpu);

    assert_memory_data(TEST_RAM_ADDR, TEST_ZERO);
    assert(cpu.C == true);
    assert_zn(&cpu, true, false);
}

static void test_inc_positive_to_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_POSITIVE);

    inc(&cpu);

    assert_memory_data(TEST_RAM_ADDR, TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

static void test_dec(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, 10);

    dec(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 9);
    assert_zn(&cpu, false, false);
}

static void test_dec_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = TEST_RAM_ADDR;
    write_test_memory(TEST_RAM_ADDR, 1);

    dec(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 0);
    assert_zn(&cpu, true, false);
}

static void test_dec_wrap(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    dec(&cpu);

    assert_memory_data(TEST_RAM_ADDR, TEST_MAX_BYTE);
    assert_zn(&cpu, false, true);
}

static void test_dec_negative_to_positive(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_NEGATIVE);

    dec(&cpu);

    assert_memory_data(TEST_RAM_ADDR, TEST_MAX_POSITIVE);
    assert_zn(&cpu, false, false);
}

static void test_inx(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = 10;

    inx(&cpu);

    assert(cpu.X == 11);
    assert_zn(&cpu, false, false);
}

static void test_inx_wrap(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = TEST_MAX_BYTE;

    inx(&cpu);

    assert(cpu.X == 0);
    assert_zn(&cpu, true, false);
}

static void test_inx_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = TEST_MAX_BYTE;
    inx(&cpu);

    assert(cpu.X == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_dex(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = 10;

    dex(&cpu);

    assert(cpu.X == 9);
    assert_zn(&cpu, false, false);
}

static void test_dex_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = 1;

    dex(&cpu);

    assert(cpu.X == 0);
    assert_zn(&cpu, true, false);
}

static void test_dex_wrap(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = TEST_ZERO;
    dex(&cpu);

    assert(cpu.X == TEST_MAX_BYTE);
    assert_zn(&cpu, false, true);
}

static void test_iny(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = 10;

    iny(&cpu);

    assert(cpu.Y == 11);
    assert_zn(&cpu, false, false);
}

static void test_iny_wrap(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = TEST_MAX_BYTE;
    iny(&cpu);

    assert(cpu.Y == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_dey(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = 10;

    dey(&cpu);

    assert(cpu.Y == 9);
    assert_zn(&cpu, false, false);
}

static void test_dey_wrap(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = TEST_ZERO;
    dey(&cpu);

    assert(cpu.Y == TEST_MAX_BYTE);
    assert_zn(&cpu, false, true);
}

// -----------------------------
// SHIFT
// -----------------------------

static void test_asl_accumulator(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = 0x40;

    asl(&cpu);

    assert(cpu.A == 0x80);
    assert(cpu.C == false);
    assert_zn(&cpu, false, true);
}

static void test_asl_carry(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_MAX_NEGATIVE;

    asl(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == true);
    assert_zn(&cpu, true, false);
}

static void test_asl_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_ZERO;
    cpu.C = true;

    asl(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == false);
    assert_zn(&cpu, true, false);
}

static void test_asl_byte_max(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_MAX_BYTE;

    asl(&cpu);

    assert(cpu.A == 0xFE);
    assert(cpu.C == true);
    assert_zn(&cpu, false, true);
}

static void test_asl_memory(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, 0x81);

    asl(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 0x02);
    assert(cpu.C == true);
    assert_zn(&cpu, false, false);
}

static void test_lsr_accumulator(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = 0x02;

    lsr(&cpu);

    assert(cpu.A == 0x01);
    assert(cpu.C == false);
    assert_zn(&cpu, false, false);
}

static void test_lsr_carry(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = 0x01;

    lsr(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == true);
    assert_zn(&cpu, true, false);
}

static void test_lsr_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_ZERO;
    cpu.C = true;

    lsr(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.C == false);
    assert_zn(&cpu, true, false);
}

static void test_lsr_byte_max(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_MAX_BYTE;

    lsr(&cpu);

    assert(cpu.A == TEST_MAX_POSITIVE);
    assert(cpu.C == true);
    assert_zn(&cpu, false, false);
}

static void test_lsr_memory(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, 0x81);

    lsr(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 0x40);
    assert(cpu.C == true);
    assert_zn(&cpu, false, false);
}

static void test_rol_accumulator(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_MAX_NEGATIVE;
    cpu.C = true;

    rol(&cpu);

    assert(cpu.A == 0x01);
    assert(cpu.C == true);
    assert_zn(&cpu, false, false);
}

static void test_rol_memory(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, 0x80);
    cpu.C = true;

    rol(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 0x01);
    assert(cpu.C == true);
    assert_zn(&cpu, false, false);
}

static void test_rol_zero_carry_in(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_ZERO;
    cpu.C = true;

    rol(&cpu);

    assert(cpu.A == 0x01);
    assert(cpu.C == false);
    assert_zn(&cpu, false, false);
}

static void test_rol_byte_max_no_carry(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_MAX_BYTE;
    cpu.C = false;

    rol(&cpu);

    assert(cpu.A == 0xFE);
    assert(cpu.C == true);
    assert_zn(&cpu, false, true);
}

static void test_ror_accumulator(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = 0x01;
    cpu.C = true;

    ror(&cpu);

    assert(cpu.A == 0x80);
    assert(cpu.C == true);
    assert_zn(&cpu, false, true);
}

static void test_ror_memory(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    set_test_operand(&cpu, TEST_RAM_ADDR, 0x01);
    cpu.C = true;

    ror(&cpu);

    assert_memory_data(TEST_RAM_ADDR, 0x80);
    assert(cpu.C == true);
    assert_zn(&cpu, false, true);
}

static void test_ror_zero_carry_in(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_ZERO;
    cpu.C = true;

    ror(&cpu);

    assert(cpu.A == 0x80);
    assert(cpu.C == false);
    assert_zn(&cpu, false, true);
}

static void test_ror_byte_max_no_carry(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;
    cpu.A = TEST_MAX_BYTE;
    cpu.C = false;

    ror(&cpu);

    assert(cpu.A == TEST_MAX_POSITIVE);
    assert(cpu.C == true);
    assert_zn(&cpu, false, false);
}

// -----------------------------
// BITWISE
// -----------------------------

static void test_and(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0xF0;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x0F);

    and(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_and_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    and(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_and_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_NEGATIVE);

    and(&cpu);

    assert(cpu.A == TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

static void test_ora(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0xF0;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x0F);

    ora(&cpu);

    assert(cpu.A == TEST_MAX_BYTE);
    assert_zn(&cpu, false, true);
}

static void test_ora_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_ZERO;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    ora(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_eor(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_BYTE);

    eor(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert_zn(&cpu, true, false);
}

static void test_eor_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_ZERO;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_NEGATIVE);

    eor(&cpu);

    assert(cpu.A == TEST_MAX_NEGATIVE);
    assert_zn(&cpu, false, true);
}

static void test_bit(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x0F;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0xC0);

    bit(&cpu);

    assert(cpu.V == true);
    assert_zn(&cpu, true, true);
}

static void test_bit_zero_clear(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0xC0);

    bit(&cpu);

    assert(cpu.V == true);
    assert_zn(&cpu, false, true);
}

static void test_bit_zero_set(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x0F;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0xC0);

    bit(&cpu);

    assert(cpu.V == true);
    assert_zn(&cpu, true, true);
}

// -----------------------------
// COMPARE
// -----------------------------

static void test_cmp_equal(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x42;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x42);

    cmp(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, true, false);
}

static void test_cmp_less(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x10;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x20);

    cmp(&cpu);

    assert(cpu.C == false);
    assert_zn(&cpu, false, true);
}

static void test_cmp_greater(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x20;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x10);

    cmp(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, false, false);
}

static void test_cmp_zero_against_byte_max(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_ZERO;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_MAX_BYTE);

    cmp(&cpu);

    assert(cpu.C == false);
    assert_zn(&cpu, false, false);
}

static void test_cmp_byte_max_against_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = TEST_MAX_BYTE;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    cmp(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, false, true);
}

static void test_cpx(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = 0x20;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x10);

    cpx(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, false, false);
}

static void test_cpx_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = TEST_ZERO;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    cpx(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, true, false);
}

static void test_cpx_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.X = TEST_MAX_BYTE;
    write_test_memory(TEST_RAM_ADDR, TEST_ZERO);

    cpx(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, false, true);
}

static void test_cpy(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = 0x20;
    set_test_operand(&cpu, TEST_RAM_ADDR, 0x20);

    cpy(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, true, false);
}

static void test_cpy_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = TEST_ZERO;
    set_test_operand(&cpu, TEST_RAM_ADDR, TEST_ZERO);

    cpy(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, true, false);
}

static void test_cpy_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Y = TEST_MAX_BYTE;
    write_test_memory(TEST_RAM_ADDR, TEST_ZERO);

    cpy(&cpu);

    assert(cpu.C == true);
    assert_zn(&cpu, false, true);
}

// -----------------------------
// BRANCH
// -----------------------------

static void test_bcc_taken(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = false;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;
    cpu.cycles = 2;

    write_test_memory(0x0201, 2);

    bcc(&cpu);

    assert(cpu.PC == 0x0204);
    assert(cpu.cycles == 3);
}

static void test_bcc_not_taken(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = true;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;
    cpu.cycles = 2;

    write_test_memory(0x0201, 2);

    bcc(&cpu);

    assert(cpu.PC == 0x0202);
    assert(cpu.cycles == 2);
}

static void test_bcc_negative_offset(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = false;
    cpu.PC = TEST_BRANCH_PC;
    set_test_operand(&cpu, 0x0201, 0xFE);

    bcc(&cpu);

    assert(cpu.PC == 0x0200);
}

static void test_bcs(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = true;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;

    write_test_memory(0x0201, 2);

    bcs(&cpu);

    assert(cpu.PC == 0x0204);
}

static void test_bcs_negative_offset(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = true;
    cpu.PC = TEST_BRANCH_PC;
    set_test_operand(&cpu, 0x0201, TEST_MAX_NEGATIVE);

    bcs(&cpu);

    assert(cpu.PC == 0x0182);
}

static void test_beq(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Z = true;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;

    write_test_memory(0x0201, 2);

    beq(&cpu);

    assert(cpu.PC == 0x0204);
}

static void test_beq_zero_offset(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Z = true;
    cpu.PC = TEST_BRANCH_PC;
    set_test_operand(&cpu, 0x0201, TEST_ZERO);

    beq(&cpu);

    assert(cpu.PC == 0x0202);
}

static void test_bne(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Z = false;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;

    write_test_memory(0x0201, 2);

    bne(&cpu);

    assert(cpu.PC == 0x0204);
}

static void test_bne_max_positive_offset(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Z = false;
    cpu.PC = TEST_BRANCH_PC;
    set_test_operand(&cpu, 0x0201, TEST_MAX_POSITIVE);

    bne(&cpu);

    assert(cpu.PC == 0x0281);
}

static void test_bpl(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.N = false;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;

    write_test_memory(0x0201, 2);

    bpl(&cpu);

    assert(cpu.PC == 0x0204);
}

static void test_bpl_not_taken(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.N = true;
    cpu.PC = 0x2002;
    set_test_operand(&cpu, 0x2001, TEST_MAX_POSITIVE);
    cpu.cycles = 2;

    bpl(&cpu);

    assert(cpu.PC == 0x2002);
    assert(cpu.cycles == 2);
}

static void test_bmi(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.N = true;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;

    write_test_memory(0x0201, 2);

    bmi(&cpu);

    assert(cpu.PC == 0x0204);
}

static void test_bmi_negative_offset(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.N = true;
    cpu.PC = 0x1002;
    set_test_operand(&cpu, 0x1001, 0xFE);

    bmi(&cpu);

    assert(cpu.PC == TEST_RAM_ADDR);
}

static void test_bvc(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.V = false;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;

    write_test_memory(0x0201, 2);

    bvc(&cpu);

    assert(cpu.PC == 0x0204);
}

static void test_bvc_not_taken(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.V = true;
    cpu.PC = 0x2002;
    set_test_operand(&cpu, 0x2001, 0x01);

    bvc(&cpu);

    assert(cpu.PC == 0x2002);
}

static void test_bvs(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.V = true;
    cpu.PC = TEST_BRANCH_PC;
    cpu.address = 0x0201;

    write_test_memory(0x0201, 2);

    bvs(&cpu);

    assert(cpu.PC == 0x0204);
}

static void test_bvs_not_taken(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.V = false;
    cpu.PC = 0x2002;
    set_test_operand(&cpu, 0x2001, 0x01);

    bvs(&cpu);

    assert(cpu.PC == 0x2002);
}

static void test_branch_page_crossing(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.Z = true;
    cpu.PC = 0x02FE;
    cpu.address = 0x02FD;
    cpu.cycles = 2;
    write_test_memory(0x02FD, 0x02);

    beq(&cpu);

    assert(cpu.PC == 0x0300);
    assert(cpu.cycles == 4);
}

// -----------------------------
// JUMP
// -----------------------------

static void test_jmp(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = 0x8000;

    jmp(&cpu);

    assert(cpu.PC == 0x8000);
}

static void test_jmp_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x1234;
    cpu.address = 0x0000;

    jmp(&cpu);

    assert(cpu.PC == 0x0000);
}

static void test_jmp_max_address(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.address = 0xFFFF;

    jmp(&cpu);

    assert(cpu.PC == 0xFFFF);
}

static void test_jsr(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_STACK_POINTER;
    cpu.PC = 0x1235;
    cpu.address = 0x8000;

    jsr(&cpu);

    assert(cpu.PC == 0x8000);
    assert(cpu.SP == 0x0D);
    assert_memory_data(0x010F, 0x34);
    assert_memory_data(0x010E, 0x12);
}

static void test_rts(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = 0x0D;
    write_test_memory(0x010E, 0x34);
    write_test_memory(0x010F, 0x12);

    rts(&cpu);

    assert(cpu.PC == 0x1235);
    assert(cpu.SP == 0x0F);
}

static void test_brk(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_STACK_POINTER;
    cpu.PC = 0x7080;
    cpu.N = true;
    cpu.V = false;
    cpu.D = true;
    cpu.I = false;
    cpu.Z = true;
    cpu.C = false;
    write_test_memory(0xFFFE, 0x34);
    write_test_memory(0xFFFF, 0x12);

    brk(&cpu);

    assert(cpu.SP == 0x0C);
    assert_memory_data(0x010E, 0x80);
    assert_memory_data(0x010F, 0x70);
    assert(cpu.PC == 0x1234);
    assert(cpu.temp_I == true);
}

static void test_rti(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = 0x0C;
    write_test_memory(0x010D, 0xC5); // status flags
    write_test_memory(0x010E, 0x34); // low
    write_test_memory(0x010F, 0x12); // high

    rti(&cpu);

    assert(cpu.PC == 0x1234);

    assert(cpu.N == true);
    assert(cpu.V == true);
    assert(cpu.Z == false);
    assert(cpu.C == true);
}

static void test_rti_restores_zero_and_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = 0x0C;
    // Status byte: N=1, V=0, D=0, I=0, Z=1, C=0
    write_test_memory(0x010D, 0x82);
    write_test_memory(0x010E, 0xFE);
    write_test_memory(0x010F, 0xCA);

    rti(&cpu);

    assert(cpu.PC == 0xCAFE);
    assert(cpu.N == true);
    assert(cpu.V == false);
    assert(cpu.Z == true);
    assert(cpu.C == false);
}

static void test_rti_restores_all_flags(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = 0x0C;
    write_test_memory(0x010D, 0xFF); // status flags (all true)
    write_test_memory(0x010E, 0x60); // low
    write_test_memory(0x010F, 0x20); // high

    rti(&cpu);

    assert(cpu.PC == 0x2060);
    assert(cpu.N == true);
    assert(cpu.V == true);
    assert(cpu.D == true);
    assert(cpu.temp_I == true);
    assert(cpu.Z == true);
    assert(cpu.C == true);
}

// -----------------------------
// STACK
// -----------------------------

static void test_pha(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_STACK_POINTER;
    cpu.A = 0x42;

    pha(&cpu);

    assert(cpu.A == 0x42);
    assert(cpu.SP == 0x0E);
    assert_memory_data(TEST_STACK_PAGE + TEST_STACK_POINTER, 0x42);
}

static void test_pla(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_STACK_POINTER;
    write_test_memory(TEST_STACK_POINTER_DATA, 0x42);

    pla(&cpu);

    assert(cpu.A == 0x42);
    assert(cpu.SP == 0x10);
    assert_zn(&cpu, false, false);
}

static void test_pla_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_STACK_POINTER;
    write_test_memory(TEST_STACK_POINTER_DATA, TEST_ZERO);

    pla(&cpu);

    assert(cpu.A == TEST_ZERO);
    assert(cpu.SP == 0x10);
    assert_zn(&cpu, true, false);
}

static void test_pla_negative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_STACK_POINTER;
    write_test_memory(TEST_STACK_POINTER_DATA, TEST_MAX_NEGATIVE);

    pla(&cpu);

    assert(cpu.A == TEST_MAX_NEGATIVE);
    assert(cpu.SP == 0x10);
    assert_zn(&cpu, false, true);
}

static void test_php(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_STACK_POINTER;
    set_all_flags(&cpu);

    php(&cpu);

    assert(cpu.SP == 0x0E);
    assert_memory_data(TEST_STACK_PAGE + TEST_STACK_POINTER, 0xFF);
}

static void test_plp(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_STACK_POINTER;
    write_test_memory(TEST_STACK_POINTER_DATA, 0xFF);

    plp(&cpu);

    assert(cpu.SP == 0x10);
    assert(cpu.N == true);
    assert(cpu.V == true);
    assert(cpu.D == true);
    assert(cpu.Z == true);
    assert(cpu.C == true);
}

// -----------------------------
// FLAGS
// -----------------------------

static void test_clc(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = true;

    clc(&cpu);

    assert(cpu.C == false);
}

static void test_clc_idempotent(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = false;
    clc(&cpu);
    clc(&cpu);

    assert(cpu.C == false);
}

static void test_sec(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = false;

    sec(&cpu);

    assert(cpu.C == true);
}

static void test_sec_idempotent(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.C = true;
    sec(&cpu);
    sec(&cpu);

    assert(cpu.C == true);
}

static void test_cli(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cli(&cpu);

    assert(cpu.temp_I == false);
    assert(cpu.delay_I == true);
}

static void test_cli_idempotent(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cli(&cpu);
    cli(&cpu);

    assert(cpu.temp_I == false);
    assert(cpu.delay_I == true);
}

static void test_sei(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    sei(&cpu);

    assert(cpu.temp_I == true);
    assert(cpu.delay_I == true);
}

static void test_sei_idempotent(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    sei(&cpu);
    sei(&cpu);

    assert(cpu.temp_I == true);
    assert(cpu.delay_I == true);
}

static void test_cld(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.D = true;

    cld(&cpu);

    assert(cpu.D == false);
}

static void test_cld_idempotent(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.D = false;
    cld(&cpu);
    cld(&cpu);

    assert(cpu.D == false);
}

static void test_sed(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.D = false;

    sed(&cpu);

    assert(cpu.D == true);
}

static void test_sed_idempotent(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.D = true;
    sed(&cpu);
    sed(&cpu);

    assert(cpu.D == true);
}

static void test_clv(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.V = true;

    clv(&cpu);
    assert(cpu.V == false);
}

static void test_clv_idempotent(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.V = false;
    clv(&cpu);
    clv(&cpu);

    assert(cpu.V == false);
}

// -----------------------------
// OTHER
// -----------------------------

static void test_nop_preserves_state(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.A = 0x01;
    cpu.X = 0x02;
    cpu.Y = 0x03;
    cpu.SP = 0x04;
    cpu.PC = 0x0506;
    set_all_flags(&cpu);

    nop(&cpu);

    assert(cpu.A == 0x01);
    assert(cpu.X == 0x02);
    assert(cpu.Y == 0x03);
    assert(cpu.SP == 0x04);
    assert(cpu.PC == 0x0506);
    assert(cpu.N == true);
    assert(cpu.V == true);
    assert(cpu.D == true);
    assert(cpu.I == true);
    assert(cpu.Z == true);
    assert(cpu.C == true);
}

// -----------------------------
// COMMON
// -----------------------------

static void test_read_address(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    write_test_memory(TEST_RAM_ADDR, 0x34);
    write_test_memory(TEST_RAM_ADDR + 1, 0x12);

    assert(read_address(&cpu, TEST_RAM_ADDR) == 0x1234);
}

static void test_read_address_zero(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    write_test_memory(0x0000, TEST_ZERO);
    write_test_memory(0x0001, TEST_ZERO);

    assert(read_address(&cpu, 0x0000) == 0x0000);
}

static void test_read_address_max(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    write_test_memory(0xFFFE, 0x34);
    write_test_memory(0xFFFF, 0x12);

    assert(read_address(&cpu, 0xFFFE) == 0x1234);
}

static void test_read_address_little_endian(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    write_test_memory(0x1234, 0x78);
    write_test_memory(0x1235, 0x56);

    assert(read_address(&cpu, 0x1234) == 0x5678);
}

static void test_set_address_modes_absolute(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = ABSOLUTE;
    write_test_memory(0x8001, 0x34);
    write_test_memory(0x8002, 0x12);

    set_address_modes(&cpu);

    assert(cpu.address == 0x1234);
}

static void test_set_address_modes_absolute_x(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = ABSOLUTE_X;
    cpu.X = 0x10;
    write_test_memory(0x8001, 0x34);
    write_test_memory(0x8002, 0x12);

    set_address_modes(&cpu);

    assert(cpu.address == 0x1244);
}

static void test_set_address_modes_absolute_y(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = ABSOLUTE_Y;
    cpu.Y = 0x10;
    write_test_memory(0x8001, 0x34);
    write_test_memory(0x8002, 0x12);

    set_address_modes(&cpu);

    assert(cpu.address == 0x1244);
}

static void test_set_address_modes_accumulator(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = ACCUMULATOR;

    set_address_modes(&cpu);

    assert(cpu.address == 0);
}

static void test_set_address_modes_immediate(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = IMMEDIATE;

    set_address_modes(&cpu);

    assert(cpu.address == 0x8001);
}

static void test_set_address_modes_implied(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = IMPLIED;

    set_address_modes(&cpu);

    assert(cpu.address == 0);
}

static void test_set_address_modes_indexed_indirect(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = INDEXED_INDIRECT;
    cpu.X = 0x04;
    write_test_memory(0x8001, 0x20);
    write_test_memory(0x0024, 0x78);
    write_test_memory(0x0025, 0x56);

    set_address_modes(&cpu);

    assert(cpu.address == 0x5678);
}

static void test_set_address_modes_indirect(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = INDIRECT;
    write_test_memory(0x8001, 0x34);
    write_test_memory(0x8002, 0x12);
    write_test_memory(0x1234, 0x78);
    write_test_memory(0x1235, 0x56);

    set_address_modes(&cpu);

    assert(cpu.address == 0x5678);
}

static void test_set_address_modes_indirect_indexed(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = INDIRECT_INDEXED;
    cpu.Y = 0x05;
    write_test_memory(0x8001, 0x20);
    write_test_memory(0x0020, 0x78);
    write_test_memory(0x0021, 0x56);

    set_address_modes(&cpu);

    assert(cpu.address == 0x567D);
}

static void test_set_address_modes_relative(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.mode = RELATIVE;

    set_address_modes(&cpu);

    assert(cpu.address == 0);
}

static void test_set_address_modes_zero_page(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = ZERO_PAGE;
    write_test_memory(0x8001, 0x34);

    set_address_modes(&cpu);

    assert(cpu.address == 0x34);
}

static void test_set_address_modes_zero_page_x(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = ZERO_PAGE_X;
    cpu.X = 0x10;
    write_test_memory(0x8001, 0x34);

    set_address_modes(&cpu);

    assert(cpu.address == 0x44);
}

static void test_set_address_modes_zero_page_y(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.PC = 0x8000;
    cpu.mode = ZERO_PAGE_Y;
    cpu.Y = 0x10;
    write_test_memory(0x8001, 0x34);

    set_address_modes(&cpu);

    assert(cpu.address == 0x44);
}

static void test_stack_pull(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_MAX_BYTE;
    write_test_memory(TEST_STACK_PAGE, 0x12);
    ubyte data = stack_pull(&cpu);

    assert(cpu.SP == TEST_ZERO);
    assert(data == 0x12);
}

static void test_stack_push(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.SP = TEST_ZERO;
    stack_push(&cpu, 0x12);

    assert(cpu.SP == TEST_MAX_BYTE);
    assert_memory_data(TEST_STACK_PAGE, 0x12);
}

static void test_page_crossed_taken(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.cycles = TEST_ZERO;
    cpu.address = 0x12FF;
    uint16_t result = page_crossed(&cpu, 0x1300);

    assert(result == 0x1300);
    assert(cpu.cycles == 1);
}

static void test_page_crossed_not_taken(void)
{
    cpu_t cpu = {0};
    cpu_test_init(&cpu);

    cpu.cycles = TEST_ZERO;
    cpu.address = 0x1201;
    uint16_t result = page_crossed(&cpu, 0x12FE);

    assert(result == 0x12FE);
    assert(cpu.cycles == TEST_ZERO);
}

// -----------------------------
// MAIN
// -----------------------------

/** Runs all tests in the access category. */
static void run_access_tests(void)
{
    test_lda();
    test_lda_zero();
    test_lda_negative();

    test_sta();

    test_ldx();
    test_ldx_zero();
    test_ldx_negative();

    test_stx();

    test_ldy();
    test_ldy_zero();
    test_ldy_negative();

    test_sty();
}

/** Runs all tests in the transfer category. */
static void run_transfer_tests(void)
{
    test_tax();
    test_tax_zero();
    test_tax_negative();

    test_txa();
    test_txa_zero();
    test_txa_negative();

    test_tay();
    test_tay_zero();
    test_tay_negative();

    test_tya();
    test_tya_zero();
    test_tya_negative();

    test_txs();

    test_tsx();
    test_tsx_zero();
    test_tsx_negative();
}

/** Runs all tests in the arithmetic category. */
static void run_arithmetic_tests(void)
{
    test_adc_wrap();
    test_adc_overflow();
    test_adc_no_carry();
    test_adc_carry_in();
    test_adc_carry_in_wrap();
    test_adc_negative_plus_negative_overflow();
    test_adc_negative_plus_positive();
    test_adc_positive_boundary();
    test_adc_byte_max_plus_byte_max();

    test_sbc();
    test_sbc_borrow();
    test_sbc_zero();
    test_sbc_no_carry_equal_zero();
    test_sbc_zero_minus_one();
    test_sbc_negative_overflow();
    test_sbc_positive_overflow();
    test_sbc_byte_max_plus_zero_plus_carry_in();

    test_inc();
    test_inc_wrap();
    test_inc_positive_to_negative();
    test_inc_zero();

    test_dec();
    test_dec_zero();
    test_dec_wrap();
    test_dec_negative_to_positive();

    test_inx();
    test_inx_wrap();
    test_inx_zero();

    test_dex();
    test_dex_zero();
    test_dex_wrap();

    test_iny();
    test_iny_wrap();

    test_dey();
    test_dey_wrap();
}

/** Runs all tests in the shift category. */
static void run_shift_tests(void)
{
    test_asl_accumulator();
    test_asl_carry();
    test_asl_zero();
    test_asl_byte_max();
    test_asl_memory();

    test_lsr_accumulator();
    test_lsr_carry();
    test_lsr_zero();
    test_lsr_byte_max();
    test_lsr_memory();

    test_rol_accumulator();
    test_rol_memory();
    test_rol_zero_carry_in();
    test_rol_byte_max_no_carry();

    test_ror_accumulator();
    test_ror_memory();
    test_ror_zero_carry_in();
    test_ror_byte_max_no_carry();
}

/** Runs all tests in the bitwise category. */
static void run_bitwise_tests(void)
{
    test_and();
    test_and_zero();
    test_and_negative();

    test_ora();
    test_ora_zero();

    test_eor();
    test_eor_negative();

    test_bit();
    test_bit_zero_clear();
    test_bit_zero_set();
}

/** Runs all tests in the compare category. */
static void run_compare_tests(void)
{
    test_cmp_equal();
    test_cmp_less();
    test_cmp_greater();
    test_cmp_zero_against_byte_max();
    test_cmp_byte_max_against_zero();

    test_cpx();
    test_cpx_zero();
    test_cpx_negative();

    test_cpy();
    test_cpy_zero();
    test_cpy_negative();
}

/** Runs all tests in the branch category. */
static void run_branch_tests(void)
{
    test_bcc_taken();
    test_bcc_not_taken();
    test_bcc_negative_offset();

    test_bcs();
    test_bcs_negative_offset();

    test_beq();
    test_beq_zero_offset();

    test_bne();
    test_bne_max_positive_offset();

    test_bpl();
    test_bpl_not_taken();

    test_bmi();
    test_bmi_negative_offset();

    test_bvc();
    test_bvc_not_taken();

    test_bvs();
    test_bvs_not_taken();

    test_branch_page_crossing();
}

/** Runs all tests in the jump category. */
static void run_jump_tests(void)
{
    test_jmp();
    test_jmp_zero();
    test_jmp_max_address();

    test_jsr();

    test_rts();

    test_brk();

    test_rti();
    test_rti_restores_zero_and_negative();
    test_rti_restores_all_flags();
}

/** Runs all tests in the stack category. */
static void run_stack_tests(void)
{
    test_pha();

    test_pla();
    test_pla_zero();
    test_pla_negative();

    test_php();

    test_plp();
}

/** Runs all tests in the flags category. */
static void run_flags_tests(void)
{
    test_clc();
    test_clc_idempotent();

    test_sec();
    test_sec_idempotent();

    test_cli();
    test_cli_idempotent();

    test_sei();
    test_sei_idempotent();

    test_cld();
    test_cld_idempotent();

    test_sed();
    test_sed_idempotent();

    test_clv();
    test_clv_idempotent();
}

/** Runs all tests in the other category. */
static void run_other_tests(void)
{
    test_nop_preserves_state();
}

/** Runs all tests in the common category. */
static void run_common_tests(void)
{
    test_read_address();
    test_read_address_zero();
    test_read_address_max();
    test_read_address_little_endian();

    test_set_address_modes_absolute();
    test_set_address_modes_absolute_x();
    test_set_address_modes_absolute_y();
    test_set_address_modes_accumulator();
    test_set_address_modes_immediate();
    test_set_address_modes_implied();
    test_set_address_modes_indexed_indirect();
    test_set_address_modes_indirect();
    test_set_address_modes_indirect_indexed();
    test_set_address_modes_relative();
    test_set_address_modes_zero_page();
    test_set_address_modes_zero_page_x();
    test_set_address_modes_zero_page_y();

    test_stack_push();

    test_stack_pull();

    test_page_crossed_taken();
    test_page_crossed_not_taken();
}

int main(void)
{
    printf("=========================================\n");
    printf("CPU TESTS\n");
    printf("=========================================\n\n");

    run_access_tests();
    run_transfer_tests();
    run_arithmetic_tests();
    run_shift_tests();
    run_bitwise_tests();
    run_compare_tests();
    run_branch_tests();
    run_jump_tests();
    run_stack_tests();
    run_flags_tests();
    run_other_tests();
    run_common_tests();

    printf("\n=========================================\n");
    printf("All CPU tests passed!\n");
    printf("=========================================\n");
    return 0;
}