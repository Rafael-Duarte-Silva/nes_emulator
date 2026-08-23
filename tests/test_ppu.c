/*
 * ============================================================================
 * PPU TESTS
 * ============================================================================
 *
 * This file contains tests for the PPU module.
 *
 * These tests are self-contained and focus exclusively on PPU functionality.
 * They do not depend on or require interaction with other system components.
 *
 * ============================================================================
 */

#include "ppu.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// -----------------------------
// MOCKS
// -----------------------------

static uint8_t memory[0x10000];

void ppu_bus_write(uint16_t address, uint8_t data, console_t *console)
{
    (void)console;
    memory[address] = data;
}

uint8_t ppu_bus_read(uint16_t address, console_t *console)
{
    (void)console;
    return memory[address];
}

void ulogger_log(LOG_LEVEL level, const char *fmt, ...)
{
    (void)level;
    (void)fmt;
}

// -----------------------------
// HELPERS
// -----------------------------

static void write_test_memory(uint16_t address, uint8_t data)
{
    memory[address] = data;
}

static uint8_t read_test_memory(uint16_t address)
{
    return memory[address];
}

// -----------------------------
// SETUP
// -----------------------------

static void ppu_test_init(ppu_t *ppu)
{
    reset_ppu(ppu);
}

// -----------------------------
// TESTS
// -----------------------------

static void test_ppu_crtl(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu_ctrl(0xFF, &ppu);

    assert(ppu.ctrl.v_blank_nmi == true);
    assert(ppu.ctrl.slave_select == true);
    assert(ppu.ctrl.sprite_size == true);
    assert(ppu.ctrl.background_table == true);
    assert(ppu.ctrl.sprite_table == true);
    assert(ppu.ctrl.vram_address == true);
    assert(ppu.ctrl.base_nametable == 0x03);
    assert(ppu.T == 0x0C00);
}

static void test_ppu_mask(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu_mask(0xFF, &ppu);

    assert(ppu.mask.blue == true);
    assert(ppu.mask.green == true);
    assert(ppu.mask.red == true);
    assert(ppu.mask.sprite_rendering == true);
    assert(ppu.mask.background_rendering == true);
    assert(ppu.mask.show_sprites == true);
    assert(ppu.mask.show_background == true);
    assert(ppu.mask.greyscale == true);
}

static void test_ppu_status(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu.W = 1;
    ppu.status.v_blank = true;
    ppu.status.sprite_0_hit = true;
    ppu.status.sprite_overflow = true;

    uint8_t result = ppu_status(&ppu);

    assert(ppu.W == 0);
    assert(ppu.status.v_blank == false);
    assert(result == 0xE0);
}

static void test_oam_address(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    oam_addr(0xC0, &ppu);

    assert(ppu.OAMADDR == 0xC0);
}

static void test_oam_data_write(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu.OAMADDR = 0x01;

    oam_data_write(0xC0, &ppu);

    assert(ppu.OAM[0x01] == 0xC0);
    assert(ppu.OAMADDR == 0x02);
}

static void test_oam_data_read(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu.OAMADDR = 0x01;
    ppu.OAM[0x01] = 0xC0;

    uint8_t result = oam_data_read(&ppu);

    assert(result == 0xC0);
}

static void test_ppu_scroll_first_write(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu.T = 0x00;
    ppu.W = 0;

    ppu_scroll(0x7D, &ppu);

    assert(ppu.PPUSCROLL == 0x7D);
    assert(ppu.T == 0x0F);
    assert(ppu.X == 0x05);
    assert(ppu.W == 1);
}

static void test_ppu_scroll_second_write(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu.W = 1;
    ppu.X = 0x05;
    ppu.T = 0x0F;

    ppu_scroll(0x5E, &ppu);

    assert(ppu.PPUSCROLL == 0x5E);
    assert(ppu.T == 0x616F);
    assert(ppu.X == 0x05);
    assert(ppu.W == 0);
}

static void test_ppu_addr_first_write(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu.W = 0;
    ppu.X = 0x05;
    ppu.T = 0x616F;

    ppu_addr(0x3D, &ppu);

    assert(ppu.T == 0x3D6F);
    assert(ppu.X == 0x05);
    assert(ppu.W == 1);
}

static void test_ppu_addr_second_write(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu.W = 1;
    ppu.X = 0x05;
    ppu.T = 0x3D6F;

    ppu_addr(0xF0, &ppu);

    assert(ppu.T == 0x3DF0);
    assert(ppu.V == ppu.T);
    assert(ppu.X == 0x05);
    assert(ppu.W == 0);
}

static void test_ppu_data_write(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    ppu_data_write(0xC0, &ppu);

    assert(read_test_memory(0x00) == 0xC0);
    assert(ppu.V == 0x01);
}

static void test_ppu_data_read(void)
{
    ppu_t ppu = {0};
    ppu_test_init(&ppu);

    write_test_memory(0x00, 0xC0);

    uint8_t result = ppu_data_read(&ppu);

    assert(result == 0xC0);
    assert(ppu.V == 0x01);
}

int main(void)
{
    printf("============================================\n");
    printf("        PPU TESTS\n");
    printf("============================================\n\n");

    test_ppu_crtl();
    test_ppu_mask();
    test_ppu_status();
    test_oam_address();
    test_oam_data_write();
    test_oam_data_read();
    test_ppu_scroll_first_write();
    test_ppu_scroll_second_write();
    test_ppu_addr_first_write();
    test_ppu_addr_second_write();
    test_ppu_data_write();
    test_ppu_data_read();

    printf("\n============================================\n");
    printf("All PPU tests passed!\n");
    printf("============================================\n");

    return 0;
}