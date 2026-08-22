/*
 * ============================================================================
 * CARTRIGDE TESTS
 * ============================================================================
 *
 * This file contains tests for the CARTRIGDE module.
 *
 * These tests are self-contained and focus exclusively on CARTRIGDE functionality.
 * They do not depend on or require interaction with other system components.
 *
 * ============================================================================
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cartrigde.h"

/* ============================================================================
 * MOCKS
 * ========================================================================== */

void init_mapper(cartrigde_t *cartrigde, ubyte mapper_type)
{
    (void)cartrigde;
    (void)mapper_type;
}

/* ============================================================================
 * CONSTANTS
 * ========================================================================== */

#define ROM_DIRECTORY "tests/roms"

#define HEADER_SIZE 0x10
#define PRG_BANK_SIZE 0x4000
#define CHR_BANK_SIZE 0x2000
#define PRG_RAM_BANK_SIZE 0x2000

#define TRAINER_DATA 0xFF
#define PRG_DATA 0xAA
#define CHR_DATA 0x55

#define ROM_SIGNATURE_0 'N'
#define ROM_SIGNATURE_1 'E'
#define ROM_SIGNATURE_2 'S'
#define ROM_SIGNATURE_3 0x1A

/* ============================================================================
 * TYPES
 * ========================================================================== */

typedef enum
{
    ROM_VALID,
    ROM_INVALID
} rom_expectation_t;

typedef struct
{
    const char *name;
    const char *filename;

    rom_expectation_t expectation;

    uint8_t signature[4];

    uint8_t prg_counter;
    uint8_t chr_counter;

    uint8_t control_one;
    uint8_t control_two;

    uint8_t prg_ram_counter;

    /*
     * If true, the file is deliberately truncated after the header/PRG.
     */
    int truncate_prg;
    int truncate_chr;

    /*
     * If true, the file will contain fewer than 16 bytes.
     */
    int invalid_size;

} rom_test_case_t;

/* ============================================================================
 * HELPERS
 * ========================================================================== */

static void create_directory(void)
{
    int result = system("mkdir -p " ROM_DIRECTORY);
    assert(result == 0);
}

static void fill_data(
    FILE *file,
    size_t size,
    uint8_t data)
{
    for (size_t i = 0; i < size; i++)
    {
        uint8_t byte = data;
        assert(fwrite(&byte, 1, 1, file) == 1);
    }
}

static void create_rom(const rom_test_case_t *test)
{
    FILE *rom = fopen(test->filename, "wb");
    assert(rom != NULL);

    /*
     * ------------------------------------------------------------
     * Invalid-size ROM
     * ------------------------------------------------------------
     */
    if (test->invalid_size)
    {
        uint8_t data[8] = {
            0x00, 0x01, 0x02, 0x03,
            0x04, 0x05, 0x06, 0x07};
        assert(fwrite(data, 1, sizeof(data), rom) == sizeof(data));
        fclose(rom);
        return;
    }

    /*
     * ------------------------------------------------------------
     * iNES header
     * ------------------------------------------------------------
     */

    uint8_t header[HEADER_SIZE] = {0};
    header[0] = test->signature[0];
    header[1] = test->signature[1];
    header[2] = test->signature[2];
    header[3] = test->signature[3];
    header[4] = test->prg_counter;
    header[5] = test->chr_counter;
    header[6] = test->control_one;
    header[7] = test->control_two;
    header[8] = test->prg_ram_counter;

    assert(fwrite(header, 1, HEADER_SIZE, rom) == HEADER_SIZE);

    /*
     * ------------------------------------------------------------
     * Trainer
     * ------------------------------------------------------------
     */

    if (test->control_one & 0x04)
    {
        fill_data(rom, 512, TRAINER_DATA);
    }

    /*
     * ------------------------------------------------------------
     * PRG-ROM
     * ------------------------------------------------------------
     */

    size_t prg_size = (size_t)test->prg_counter * PRG_BANK_SIZE;
    if (test->truncate_prg)
    {
        if (prg_size > 0)
            prg_size /= 2;
    }

    fill_data(rom, prg_size, PRG_DATA);

    if (test->truncate_prg)
    {
        fclose(rom);
        return;
    }

    /*
     * ------------------------------------------------------------
     * CHR-ROM
     * ------------------------------------------------------------
     */

    size_t chr_size = (size_t)test->chr_counter * CHR_BANK_SIZE;
    if (test->truncate_chr)
    {
        if (chr_size > 0)
            chr_size /= 2;
    }

    fill_data(rom, chr_size, CHR_DATA);

    fclose(rom);
}

static void cleanup(cartrigde_t *cartrigde)
{
    free(cartrigde->PRG_ROM);
    free(cartrigde->CHR_ROM);

    cartrigde->PRG_ROM = NULL;
    cartrigde->CHR_ROM = NULL;
}

static void remove_rom(const rom_test_case_t *test)
{
    remove(test->filename);
}

/* ============================================================================
 * TEST CASES
 * ========================================================================== */

static const rom_test_case_t tests[] =
    {
        /*
         * ------------------------------------------------------------------------
         * Normal ROM
         * ------------------------------------------------------------------------
         */

        {
            .name = "valid ROM",
            .filename = ROM_DIRECTORY "/valid.nes",

            .expectation = ROM_VALID,

            .signature = {
                ROM_SIGNATURE_0,
                ROM_SIGNATURE_1,
                ROM_SIGNATURE_2,
                ROM_SIGNATURE_3},

            .prg_counter = 1,
            .chr_counter = 1,

            .control_one = 0x00,
            .control_two = 0x00,

            .prg_ram_counter = 0,

            .truncate_prg = 0,
            .truncate_chr = 0,
            .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Two PRG banks, no CHR
         * ------------------------------------------------------------------------
         */

        {.name = "PRG only", .filename = ROM_DIRECTORY "/prg_only.nes",

         .expectation = ROM_VALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 2,
         .chr_counter = 0,

         .control_one = 0x00,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Multiple PRG and CHR banks
         * ------------------------------------------------------------------------
         */

        {.name = "multiple PRG and CHR banks", .filename = ROM_DIRECTORY "/multiple_banks.nes",

         .expectation = ROM_VALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 4,
         .chr_counter = 2,

         .control_one = 0x00,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * PRG_RAM in older version compatibility
         * ------------------------------------------------------------------------
         */

        {.name = "PRG_RAM in older version compatibility", .filename = ROM_DIRECTORY "/pgr_ram.nes",

         .expectation = ROM_VALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 1,
         .chr_counter = 1,

         .control_one = 0x00,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Battery
         * ------------------------------------------------------------------------
         */

        {.name = "battery backed RAM", .filename = ROM_DIRECTORY "/battery.nes",

         .expectation = ROM_VALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 1,
         .chr_counter = 1,

         .control_one = 0x02,
         .control_two = 0x00,

         .prg_ram_counter = 2,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Mapper 1
         *
         * Mapper:
         *
         * control_one  = 0x10
         * control_two  = 0x10
         *
         * mapper = 0x11
         * ------------------------------------------------------------------------
         */

        {.name = "mapper 0x11", .filename = ROM_DIRECTORY "/mapper_11.nes",

         .expectation = ROM_VALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 1,
         .chr_counter = 1,

         .control_one = 0x10,
         .control_two = 0x10,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Vertical mirroring
         * ------------------------------------------------------------------------
         */

        {.name = "vertical mirroring", .filename = ROM_DIRECTORY "/vertical_mirroring.nes",

         .expectation = ROM_VALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 1,
         .chr_counter = 1,

         .control_one = 0x01,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Trainer
         * ------------------------------------------------------------------------
         */

        {.name = "trainer", .filename = ROM_DIRECTORY "/trainer.nes",

         .expectation = ROM_VALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 1,
         .chr_counter = 1,

         .control_one = 0x04,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Four screen
         * ------------------------------------------------------------------------
         */

        {.name = "four screen mirroring", .filename = ROM_DIRECTORY "/four_screen.nes",

         .expectation = ROM_VALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 1,
         .chr_counter = 1,

         .control_one = 0x08,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Invalid signature
         * ------------------------------------------------------------------------
         */

        {.name = "invalid signature", .filename = ROM_DIRECTORY "/invalid_signature.nes",

         .expectation = ROM_INVALID,

         .signature = {'B', 'A', 'D', '!'},

         .prg_counter = 1,
         .chr_counter = 1,

         .control_one = 0x00,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Header too small
         * ------------------------------------------------------------------------
         */

        {.name = "header smaller than 16 bytes", .filename = ROM_DIRECTORY "/small_header.nes",

         .expectation = ROM_INVALID,

         .signature = {0, 0, 0, 0},

         .prg_counter = 0,
         .chr_counter = 0,

         .control_one = 0,
         .control_two = 0,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 1},

        /*
         * ------------------------------------------------------------------------
         * PRG declared but file is truncated
         * ------------------------------------------------------------------------
         */

        {.name = "truncated PRG-ROM", .filename = ROM_DIRECTORY "/truncated_prg.nes",

         .expectation = ROM_INVALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 2,
         .chr_counter = 0,

         .control_one = 0x00,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 1,
         .truncate_chr = 0,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * CHR declared but file is truncated
         * ------------------------------------------------------------------------
         */

        {.name = "truncated CHR-ROM", .filename = ROM_DIRECTORY "/truncated_chr.nes",

         .expectation = ROM_INVALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 1,
         .chr_counter = 2,

         .control_one = 0x00,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 1,
         .invalid_size = 0},

        /*
         * ------------------------------------------------------------------------
         * Zero PRG
         * ------------------------------------------------------------------------
         */

        {.name = "zero PRG banks", .filename = ROM_DIRECTORY "/zero_prg.nes",

         .expectation = ROM_INVALID,

         .signature = {ROM_SIGNATURE_0, ROM_SIGNATURE_1, ROM_SIGNATURE_2, ROM_SIGNATURE_3},

         .prg_counter = 0,
         .chr_counter = 1,

         .control_one = 0x00,
         .control_two = 0x00,

         .prg_ram_counter = 0,

         .truncate_prg = 0,
         .truncate_chr = 0,
         .invalid_size = 0}};

#define TEST_COUNT (sizeof(tests) / sizeof(tests[0]))

/* ============================================================================
 * TEST RUNNER
 * ========================================================================== */

static void run_test(const rom_test_case_t *test)
{
    printf("  %-35s ", test->name);
    fflush(stdout);

    create_rom(test);

    cartrigde_t cartrigde = {0};
    load_cartrigde(&cartrigde, test->filename);
    if (test->expectation == ROM_INVALID)
    {
        assert(cartrigde.PRG_ROM == NULL);
        assert(cartrigde.CHR_ROM == NULL);

        cleanup(&cartrigde);
        remove_rom(test);

        printf("[PASS]\n");
        return;
    }

    /*
     * ------------------------------------------------------------------------
     * Basic loading
     * ------------------------------------------------------------------------
     */

    size_t expected_prg_ram_counter = test->prg_ram_counter;
    if (expected_prg_ram_counter == 0)
        expected_prg_ram_counter = 1;

    assert(memcmp(cartrigde.signature, test->signature, sizeof(test->signature)) == 0);
    assert(cartrigde.PRG_ROM_counter == test->prg_counter);
    assert(cartrigde.CHR_ROM_counter == test->chr_counter);
    assert(cartrigde.ROM_CONTROL_ONE == test->control_one);
    assert(cartrigde.ROM_CONTROL_TWO == test->control_two);
    assert(cartrigde.PRG_RAM_counter == expected_prg_ram_counter);

    /*
     * ------------------------------------------------------------------------
     * PRG
     * ------------------------------------------------------------------------
     */

    size_t expected_prg_size = (size_t)test->prg_counter * PRG_BANK_SIZE;
    assert(cartrigde.PRG_ROM_size == expected_prg_size);

    if (expected_prg_size > 0)
    {
        assert(cartrigde.PRG_ROM != NULL);
        assert(cartrigde.PRG_ROM[0] == PRG_DATA);
        assert(cartrigde.PRG_ROM[expected_prg_size - 1] == PRG_DATA);
    }

    /*
     * ------------------------------------------------------------------------
     * CHR
     * ------------------------------------------------------------------------
     */

    size_t expected_chr_size = (size_t)test->chr_counter * CHR_BANK_SIZE;
    assert(cartrigde.CHR_ROM_size == expected_chr_size);

    if (expected_chr_size > 0)
    {
        assert(cartrigde.CHR_ROM != NULL);
        assert(cartrigde.CHR_ROM[0] == CHR_DATA);
        assert(cartrigde.CHR_ROM[expected_chr_size - 1] == CHR_DATA);
    }

    /*
     * ------------------------------------------------------------------------
     * PRG_RAM
     * ------------------------------------------------------------------------
     */

    size_t expected_prg_ram_size = expected_prg_ram_counter * PRG_RAM_BANK_SIZE;
    assert(cartrigde.PRG_RAM_size == expected_prg_ram_size);

    if (expected_prg_ram_size > 0)
    {
        assert(cartrigde.PRG_RAM != NULL);
    }

    /*
     * ------------------------------------------------------------------------
     * Mapper
     * ------------------------------------------------------------------------
     */

    uint16_t expected_mapper =
        ((test->control_two >> 4) & 0x0F) << 4 |
        ((test->control_one >> 4) & 0x0F);

    uint16_t mapper =
        ((cartrigde.ROM_CONTROL_TWO >> 4) & 0x0F) << 4 |
        ((cartrigde.ROM_CONTROL_ONE >> 4) & 0x0F);

    assert(mapper == expected_mapper);
    assert(mapper < 256);

    /*
     * ------------------------------------------------------------------------
     * Cleanup
     * ------------------------------------------------------------------------
     */

    cleanup(&cartrigde);
    remove_rom(test);

    printf("[PASS]\n");
}

/* ============================================================================
 * MAIN
 * ========================================================================== */

int main(void)
{
    printf("============================================\n");
    printf("        CARTRIGDE TESTS\n");
    printf("============================================\n\n");

    create_directory();

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        run_test(&tests[i]);
    }

    int result = system("rmdir " ROM_DIRECTORY " 2>/dev/null");
    (void)result;

    printf("\n============================================\n");
    printf("All CARTRIGDE tests passed!\n");
    printf("============================================\n");

    return EXIT_SUCCESS;
}