#include "cartrigde.h"
#include <stdlib.h>
#include <string.h>

void init_cartrigde(console_t *console, cartrigde_t *cartrigde, const char rom_name[])
{
    load_cartrigde(cartrigde, rom_name);

    cartrigde->console = console;
    console->cartrigde = cartrigde;
}

void load_cartrigde(cartrigde_t *cartrigde, const char rom_name[])
{
    FILE *rom = open_rom(rom_name);
    if (rom == NULL)
        return;

    long rom_size = get_rom_size(rom, rom_name);
    if (rom_size < 0)
    {
        fclose(rom);
        return;
    }

    if (!read_header(rom, cartrigde, rom_name))
    {
        fclose(rom);
        return;
    }

    if (!load_prg_rom(rom, cartrigde, rom_size))
    {
        cleanup_prg(cartrigde);
        fclose(rom);
        return;
    }

    if (!load_chr_rom(rom, cartrigde, rom_size))
    {
        cleanup_chr(cartrigde);
        cleanup_prg(cartrigde);
        fclose(rom);
        return;
    }

    fclose(rom);

    if (!load_prg_ram(cartrigde))
    {
        cleanup_chr(cartrigde);
        cleanup_prg(cartrigde);
        cleanup_prg_ram(cartrigde);
        return;
    }

    ubyte mapper_type =
        (((cartrigde->ROM_CONTROL_TWO >> 4) & 0x0F) << 4) |
        ((cartrigde->ROM_CONTROL_ONE >> 4) & 0x0F);

    return init_mapper(cartrigde, mapper_type);
}

FILE *open_rom(const char *rom_name)
{
    if (rom_name == NULL)
    {
        fprintf(stderr, "ROM filename is NULL\n");
        return NULL;
    }

    FILE *rom = fopen(rom_name, "rb");
    if (rom == NULL)
    {
        fprintf(stderr, "Could not open ROM '%s'\n", rom_name);
        return NULL;
    }

    return rom;
}

long get_rom_size(FILE *rom, const char *rom_name)
{
    if (fseek(rom, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Could not seek to end of ROM '%s'\n", rom_name);
        return -1;
    }

    long size = ftell(rom);
    if (size < 0)
    {
        fprintf(stderr, "Could not determine size of ROM '%s'\n", rom_name);
        return -1;
    }

    if (fseek(rom, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "Could not seek to start of ROM\n");
        return false;
    }

    return size;
}

bool read_header(FILE *rom, cartrigde_t *cartrigde, const char *rom_name)
{
    const size_t HEADER_BLOCK_SIZE = 0x10;
    uint8_t header[HEADER_BLOCK_SIZE];
    if (fseek(rom, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "Could not seek to ROM header\n");
        return false;
    }

    if (fread(header, 1, HEADER_BLOCK_SIZE, rom) != HEADER_BLOCK_SIZE)
    {
        fprintf(stderr, "Could not read complete ROM header\n");
        return false;
    }

    memcpy(cartrigde->signature, header, 4);
    if (cartrigde->signature[0] != 'N' ||
        cartrigde->signature[1] != 'E' ||
        cartrigde->signature[2] != 'S' ||
        cartrigde->signature[3] != 0x1A)
    {
        fprintf(stderr, "Invalid iNES signature in ROM '%s'\n", rom_name);
        return false;
    }

    cartrigde->PRG_ROM_counter = header[0x04];
    cartrigde->CHR_ROM_counter = header[0x05];
    cartrigde->ROM_CONTROL_ONE = header[0x06];
    cartrigde->ROM_CONTROL_TWO = header[0x07];
    cartrigde->PRG_RAM_counter = header[0x08];

    return true;
}

size_t get_rom_offset(const cartrigde_t *cartrigde)
{
    const size_t HEADER_BLOCK_SIZE = 0x10;
    const size_t TRAINER_SIZE = 512;

    size_t offset = HEADER_BLOCK_SIZE;

    if (cartrigde->ROM_CONTROL_ONE & 0x04)
        offset += TRAINER_SIZE;

    return offset;
}

bool load_prg_rom(FILE *rom, cartrigde_t *cartrigde, long rom_size)
{
    size_t rom_offset = get_rom_offset(cartrigde);
    const size_t PRG_BANK_SIZE = 0x4000;
    if ((size_t)cartrigde->PRG_ROM_counter > SIZE_MAX / PRG_BANK_SIZE)
    {
        fprintf(stderr, "PRG-ROM size overflow\n");
        return false;
    }

    cartrigde->PRG_ROM_size = cartrigde->PRG_ROM_counter * PRG_BANK_SIZE;
    if (cartrigde->PRG_ROM_size == 0)
    {
        fprintf(stderr, "ROM contains zero PRG-ROM banks\n");
        return false;
    }

    size_t minimum_prg_end = rom_offset + cartrigde->PRG_ROM_size;
    if ((size_t)rom_size < minimum_prg_end)
    {
        fprintf(stderr, "ROM is truncated: PRG-ROM is incomplete\n");
        return false;
    }

    cartrigde->PRG_ROM = malloc(cartrigde->PRG_ROM_size);
    if (cartrigde->PRG_ROM == NULL)
    {
        fprintf(stderr, "Could not allocate PRG-ROM (%u bytes)\n", cartrigde->PRG_ROM_size);
        cartrigde->PRG_ROM_size = 0;
        return false;
    }

    if (fseek(rom, rom_offset, SEEK_SET) != 0)
    {
        fprintf(stderr, "Could not seek to PRG-ROM\n");
        cleanup_prg(cartrigde);
        return false;
    }

    size_t prg_read = fread(cartrigde->PRG_ROM, 1, cartrigde->PRG_ROM_size, rom);
    if (prg_read != cartrigde->PRG_ROM_size)
    {
        fprintf(stderr, "Failed to read complete PRG-ROM\n");
        cleanup_prg(cartrigde);
        return false;
    }

    return true;
}

bool load_chr_rom(FILE *rom, cartrigde_t *cartrigde, long rom_size)
{
    size_t rom_offset = get_rom_offset(cartrigde);
    const size_t CHR_BANK_SIZE = 0x2000;
    if ((size_t)cartrigde->CHR_ROM_counter > SIZE_MAX / CHR_BANK_SIZE)
    {
        fprintf(stderr, "CHR-ROM size overflow\n");
        return false;
    }

    cartrigde->CHR_ROM_size = cartrigde->CHR_ROM_counter * CHR_BANK_SIZE;
    if (cartrigde->CHR_ROM_size == 0)
    {
        cartrigde->CHR_ROM = NULL;
        return true;
    }

    size_t minimum_chr_end = rom_offset + cartrigde->PRG_ROM_size + cartrigde->CHR_ROM_size;
    if ((size_t)rom_size < minimum_chr_end)
    {
        fprintf(stderr, "ROM is truncated: CHR-ROM is incomplete\n");
        cartrigde->CHR_ROM_size = 0;
        return false;
    }

    cartrigde->CHR_ROM = malloc(cartrigde->CHR_ROM_size);
    if (cartrigde->CHR_ROM == NULL)
    {
        fprintf(stderr, "Could not allocate CHR-ROM (%u bytes)\n", cartrigde->CHR_ROM_size);
        cartrigde->CHR_ROM_size = 0;
        return false;
    }

    size_t chr_offset = rom_offset + cartrigde->PRG_ROM_size;
    if (fseek(rom, (long)chr_offset, SEEK_SET) != 0)
    {
        fprintf(stderr, "Could not seek to CHR-ROM\n");
        cleanup_chr(cartrigde);
        return false;
    }

    if (fread(cartrigde->CHR_ROM, 1, cartrigde->CHR_ROM_size, rom) != cartrigde->CHR_ROM_size)
    {
        fprintf(stderr, "Failed to read complete CHR-ROM\n");
        cleanup_chr(cartrigde);
        return false;
    }

    return true;
}

bool load_prg_ram(cartrigde_t *cartrigde)
{
    const size_t PRG_RAM_BANK_SIZE = 0x2000;
    if ((size_t)cartrigde->PRG_RAM_counter > SIZE_MAX / PRG_RAM_BANK_SIZE)
    {
        fprintf(stderr, "PRG-RAM size overflow\n");
        return false;
    }

    // For compatibility with old versions
    if (cartrigde->PRG_RAM_counter == 0)
    {
        cartrigde->PRG_RAM_counter = 1;
    }
    cartrigde->PRG_RAM_size = cartrigde->PRG_RAM_counter * PRG_RAM_BANK_SIZE;

    cartrigde->PRG_RAM = malloc(cartrigde->PRG_RAM_size);
    if (cartrigde->PRG_RAM == NULL)
    {
        fprintf(stderr, "Could not allocate PRG-RAM (%u bytes)\n", cartrigde->PRG_RAM_size);
        cartrigde->PRG_RAM_size = 0;
        return false;
    }

    return true;
}

void cleanup_prg(cartrigde_t *cartrigde)
{
    free(cartrigde->PRG_ROM);
    cartrigde->PRG_ROM = NULL;
    cartrigde->PRG_ROM_size = 0;
}

void cleanup_chr(cartrigde_t *cartrigde)
{
    free(cartrigde->CHR_ROM);
    cartrigde->CHR_ROM = NULL;
    cartrigde->CHR_ROM_size = 0;
}

void cleanup_prg_ram(cartrigde_t *cartrigde)
{
    free(cartrigde->PRG_RAM);
    cartrigde->PRG_RAM = NULL;
    cartrigde->PRG_RAM_size = 0;
}