#include "cartrigde.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void init_cartrigde(cartrigde_t *cartrigde, const char rom_name[]) {
    FILE *rom = fopen(rom_name, "rb");
    if (!rom) {
        perror("Rom file is invalid or does not exist\n");
        return;
    }

    fseek(rom, 0, SEEK_END);
    const size_t rom_size = ftell(rom);
    rewind(rom);

    const ubyte HEADER_BLOCK_ZIZE = 0x09;
    if (rom_size < HEADER_BLOCK_ZIZE) {
        perror("Could not read Rom file into memory\n");
        fclose(rom);
        return;
    }
    
    ubyte *header_block = malloc(HEADER_BLOCK_ZIZE);
    if(!header_block) return;

    fseek(rom, 0X00, SEEK_SET);
    fread(header_block, 0x01, HEADER_BLOCK_ZIZE, rom);

    printf("rom_name: %s, rom_size: %ld\n", rom_name, rom_size);
    
    memcpy(&cartrigde->signature, &header_block[0x00], 0x03);
    printf("signature: %s\n", cartrigde->signature);

    memcpy(&cartrigde->format, &header_block[0x03], 0x01);
    printf("format: %#X\n", cartrigde->format);

    memcpy(&cartrigde->PRG_ROM_counter, &header_block[0x04], 0x01);
    printf("PRG_ROM_counter: %#X\n", cartrigde->PRG_ROM_counter);

    memcpy(&cartrigde->CHR_ROM_counter, &header_block[0x05], 0x01);
    printf("CHR_ROM_counter: %#X\n", cartrigde->CHR_ROM_counter);

    memcpy(&cartrigde->ROM_CONTROL, &header_block[0x06], 0x02);
    printf("ROM_CONTROL: %#X\n", cartrigde->ROM_CONTROL);

    memcpy(&cartrigde->RAM_counter, &header_block[0x07], 0x01);
    printf("RAM_counter: %#X\n", cartrigde->RAM_counter);

    free(header_block);
    fclose(rom);
}