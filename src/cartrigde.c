#include "cartrigde.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void init_cartrigde(console_t *console, cartrigde_t *cartrigde, const char rom_name[]) {
    FILE *rom = fopen(rom_name, "rb");
    if (!rom) {
        perror("Rom file is invalid or does not exist\n");
        return;
    }

    fseek(rom, 0, SEEK_END);
    const size_t rom_size = ftell(rom);
    rewind(rom);

    const ubyte HEADER_BLOCK_ZIZE = 0x10;
    if (rom_size < HEADER_BLOCK_ZIZE) {
        perror("Could not read Rom file into memory\n");
        fclose(rom);
        return;
    }
    
    ubyte *header_block = malloc(HEADER_BLOCK_ZIZE);
    if(!header_block) return;

    fseek(rom, 0X00, SEEK_SET);
    fread(header_block, 0x01, HEADER_BLOCK_ZIZE, rom);
    memcpy(&cartrigde->signature, &header_block[0x00], 0x03);
    memcpy(&cartrigde->format, &header_block[0x03], 0x01);
    memcpy(&cartrigde->PRG_ROM_counter, &header_block[0x04], 0x01);
    memcpy(&cartrigde->CHR_ROM_counter, &header_block[0x05], 0x01);
    memcpy(&cartrigde->ROM_CONTROL_ONE, &header_block[0x06], 0x01);
    memcpy(&cartrigde->ROM_CONTROL_TWO, &header_block[0x07], 0x01);
    memcpy(&cartrigde->RAM_counter, &header_block[0x08], 0x01);
    free(header_block);
    
    cartrigde->prg_rom_size = cartrigde->PRG_ROM_counter * 0x4000;
    cartrigde->prg_rom = malloc(cartrigde->prg_rom_size);
    if(!cartrigde->prg_rom) return;
    fseek(rom, 0X10, SEEK_SET);
    if (fread(cartrigde->prg_rom, 0x01, cartrigde->prg_rom_size, rom) != cartrigde->prg_rom_size) {
        perror("Failed to read PRG-ROM\n");
        free(cartrigde->prg_rom);
        fclose(rom);
        return;
    }

    cartrigde->chr_rom_size = cartrigde->CHR_ROM_counter * 0x2000;
    cartrigde->chr_rom = malloc(cartrigde->chr_rom_size);
    if(!cartrigde->chr_rom) return;
    fseek(rom, 0X10 + cartrigde->prg_rom_size, SEEK_SET);
    if (fread(cartrigde->chr_rom, 0x01, cartrigde->chr_rom_size, rom) != cartrigde->chr_rom_size) {
        perror("Failed to read PRG-ROM\n");
        free(cartrigde->prg_rom);
        fclose(rom);
        return;
    }

    printf("\n\nfirst byte: %#X\n\n", cartrigde->prg_rom[0]);

    fclose(rom);

    ubyte mapper_type = (cartrigde->ROM_CONTROL_TWO >> 4 & 0x0F) << 4 | (cartrigde->ROM_CONTROL_ONE >> 4 & 0x0F);
    init_mapper(cartrigde, mapper_type);

    cartrigde->console = console;
    console->cartrigde = cartrigde;

    printf("rom_name: %s, rom_size: %ld\n", rom_name, rom_size);

    printf("signature: %s\n", cartrigde->signature);
    printf("format: %#X\n", cartrigde->format);
    printf("PRG_ROM_counter: %#X\n", cartrigde->PRG_ROM_counter);
    printf("CHR_ROM_counter: %#X\n", cartrigde->CHR_ROM_counter);
    printf("ROM_CONTROL_ONE: %#X\n", cartrigde->ROM_CONTROL_ONE);
    printf("ROM_CONTROL_TWO: %#X\n", cartrigde->ROM_CONTROL_TWO);
    printf("RAM_counter: %#X\n", cartrigde->RAM_counter);

    printf("ROM_CONTROL(byte-1): \n");
    printf("    ROM_CONTROL(bit-0): %d\n", cartrigde->ROM_CONTROL_ONE & 0x01);
    printf("    ROM_CONTROL(bit-1): %d\n", cartrigde->ROM_CONTROL_ONE >> 1 & 0x01);
    printf("    ROM_CONTROL(bit-2): %d\n", cartrigde->ROM_CONTROL_ONE >> 2 & 0x01);
    printf("    ROM_CONTROL(bit-3): %d\n", cartrigde->ROM_CONTROL_ONE >> 3 & 0x01);
    printf("    ROM_CONTROL(bit-4-7): %d\n", cartrigde->ROM_CONTROL_ONE >> 4 & 0x0F);

    printf("ROM_CONTROL(byte-2): \n");
    printf("    ROM_CONTROL(bit-0-3): %d\n", cartrigde->ROM_CONTROL_TWO & 0x07);
    printf("    ROM_CONTROL(bit-4-7): %d\n", cartrigde->ROM_CONTROL_TWO >> 4 & 0x0F);
    printf("    MAPPER: %d\n", mapper_type);
}