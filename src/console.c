#include "console.h"
#include "cpu.h"
#include "stdlib.h"
#include "stdio.h"

void init_console(console_t *console, cpu_t *cpu, const char rom_name[]){
    FILE *rom = fopen(rom_name, "rb");
    if (!rom) {
        perror("Rom file is invalid or does not exist\n");
        return;
    }

    fseek(rom, 0, SEEK_END);
    const size_t rom_size = ftell(rom);
    rewind(rom);
    
    printf("rom_name: %s, rom_size: %ld\n", rom_name, rom_size);

    if (fread(&console->RAM, rom_size, 1, rom) != 1) {
        perror("Could not read Rom file into CHIP8 memory\n");
        return;
    }
    fclose(rom);

    console->CPU = cpu;
    console->CPU->console = console;
}