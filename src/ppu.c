#include "ppu.h"
#include "ppu_bus.h"
#include "console.h"

void init_ppu(console_t *console, ppu_t *ppu)
{
    console->PPU = ppu;
    ppu->console = console;

    reset_ppu(ppu);

    ppu->read_registers = read_registers;
    ppu->write_registers = write_registers;
}

void reset_ppu(ppu_t *ppu)
{
    ppu->ctrl.v_blank_nmi = false;
    ppu->ctrl.slave_select = false;
    ppu->ctrl.sprite_size = false;
    ppu->ctrl.background_table = false;
    ppu->ctrl.sprite_table = false;
    ppu->ctrl.vram_address = false;
    ppu->ctrl.base_nametable = 0x00;

    ppu->mask.blue = false;
    ppu->mask.green = false;
    ppu->mask.red = false;
    ppu->mask.sprite_rendering = false;
    ppu->mask.background_rendering = false;
    ppu->mask.show_sprites = false;
    ppu->mask.show_background = false;
    ppu->mask.greyscale = false;

    ppu->status.v_blank = false;
    ppu->status.sprite_0_hit = false;
    ppu->status.sprite_overflow = false;

    ppu->OAMADDR = 0;
    ppu->PPUSCROLL = 0;
}

uint8_t read_registers(uint16_t address, ppu_t *ppu)
{
    LOG_DEBUG("PPU-address(read): %#X", address);

    switch (address)
    {
    case 0x2002:
        return ppu_status(ppu);
    case 0x2004:
        return oam_data_read(ppu);
    case 0x2007:
        return ppu_data_read(ppu);

    default:
        break;
    }

    return 0x00;
}

void write_registers(uint16_t address, uint8_t data, ppu_t *ppu)
{
    LOG_DEBUG("PPU-address(write): %#X", address);

    switch (address)
    {
    case 0x2000:
        ppu_ctrl(data, ppu);
        return;

    case 0x2001:
        ppu_mask(data, ppu);
        return;

    case 0x2003:
        oam_addr(data, ppu);
        return;

    case 0x2004:
        oam_data_write(data, ppu);
        return;

    case 0x2005:
        ppu_scroll(data, ppu);
        return;

    case 0x2006:
        ppu_addr(data, ppu);
        return;

    case 0x2007:
        ppu_data_write(data, ppu);
        return;

    default:
        break;
    }
}

uint8_t get_increment_vram(uint8_t key)
{
    if (key > 1)
        return -1;

    uint8_t increment_vram[] = {0x01, 0x20};

    return increment_vram[key];
}

// 0x2000
void ppu_ctrl(uint8_t data, ppu_t *ppu)
{
    ppu->T &= 0xF3FF;
    ppu->T |= (data & 0x03) << 10;

    ppu->ctrl.v_blank_nmi = (data >> 7) & 0x01;
    ppu->ctrl.slave_select = (data >> 6) & 0x01;
    ppu->ctrl.sprite_size = (data >> 5) & 0x01;
    ppu->ctrl.background_table = (data >> 4) & 0x01;
    ppu->ctrl.sprite_table = (data >> 3) & 0x01;
    ppu->ctrl.vram_address = (data >> 2) & 0x01;
    ppu->ctrl.base_nametable = data & 0x03;
}

// 0x2001
void ppu_mask(uint8_t data, ppu_t *ppu)
{
    ppu->mask.blue = (data >> 7) & 0x01;
    ppu->mask.green = (data >> 6) & 0x01;
    ppu->mask.red = (data >> 5) & 0x01;
    ppu->mask.sprite_rendering = (data >> 4) & 0x01;
    ppu->mask.background_rendering = (data >> 3) & 0x01;
    ppu->mask.show_sprites = (data >> 2) & 0x01;
    ppu->mask.show_background = (data >> 1) & 0x01;
    ppu->mask.greyscale = data & 0x01;
}

// 0x2002
uint8_t ppu_status(ppu_t *ppu)
{
    ppu->W = 0;
    uint8_t result = ppu->status.v_blank << 7 | ppu->status.sprite_0_hit << 6 | ppu->status.sprite_overflow << 5;
    ppu->status.v_blank = false;
    return result;
}

// 0x2003
void oam_addr(uint8_t address, ppu_t *ppu)
{
    ppu->OAMADDR = address;
}

// 0x2004
void oam_data_write(uint8_t data, ppu_t *ppu)
{
    ppu->OAM[ppu->OAMADDR] = data;
    ppu->OAMADDR++;
}

// 0x2004
uint8_t oam_data_read(ppu_t *ppu)
{
    return ppu->OAM[ppu->OAMADDR];
}

// 0x2005
void ppu_scroll(uint8_t data, ppu_t *ppu)
{
    ppu->PPUSCROLL = data;

    if (ppu->W == 0)
    {
        ppu->T &= 0xFFE0;
        ppu->T |= data >> 3;
        ppu->X |= (data & 0x07);
        ppu->W = 1;
        return;
    }

    uint16_t first_bits = (data & 0xC0) << 2;
    uint16_t mid_bits = (data & 0x38) << 2;
    uint16_t last_bits = (data << 12) & 0x7FFF;

    ppu->T &= 0x0C1F;
    ppu->T |= last_bits | first_bits | mid_bits;
    ppu->W = 0;
}

// 0x2006
void ppu_addr(uint8_t address, ppu_t *ppu)
{
    if (ppu->W == 0)
    {
        ppu->T &= 0x00FF;
        ppu->T |= (address & 0x3F) << 8;
        ppu->W = 1;
        return;
    }

    ppu->T &= 0x7F00;
    ppu->T |= address;
    ppu->V = ppu->T;
    ppu->W = 0;
}

// 0x2007
void ppu_data_write(uint8_t data, ppu_t *ppu)
{
    ppu_bus_write(ppu->V, data, ppu->console);
    ppu->V += get_increment_vram(ppu->ctrl.vram_address);
}

// 0x2007
uint8_t ppu_data_read(ppu_t *ppu)
{
    const uint8_t data = ppu_bus_read(ppu->V, ppu->console);
    ppu->V += get_increment_vram(ppu->ctrl.vram_address);

    return data;
}