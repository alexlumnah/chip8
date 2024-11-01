#include <stdlib.h>

#include "chip8.h"

uint8_t _xor(uint8_t x, uint8_t y) {
    return (x || y) && !(x && y);
}

// Clear display
void cls(Chip8* chip) {
    for (int i = 0; i < VID_WIDTH*VID_HEIGHT; i++) chip->vid[i] = 0;
}

// Return from subroutine
void ret(Chip8* chip) {
    // Pop address from top of stack
    chip->pc = chip->stack[chip->sp];
    chip->sp--;
}

// Jump
void jp(Chip8* chip, uint16_t addr) {
    chip->pc = addr;
}

// Load value into register
void ld(Chip8* chip, uint8_t dst, uint8_t val) {
    chip->reg[dst] = val;
}

// Add value to register, don't set carry flag
void addnc(Chip8* chip, uint8_t dst, uint8_t val) {
    chip->reg[dst] += val;
}

// Add value to register
void add(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t x_init = chip->reg[dst];
    chip->reg[dst] += val;

    uint8_t flag = 0;
    if (chip->reg[dst] < x_init) flag = 1;
    chip->reg[0xf] = flag;
}

// Load value into I register
void ldi(Chip8* chip, uint16_t addr) {
    chip->i = addr;
}

// Draw n-byte sprite to screen
void drw(Chip8* chip, uint8_t x_reg, uint8_t y_reg, uint8_t n) {

    // Find drawing coordinates
    uint8_t x = chip->reg[x_reg] % VID_WIDTH;
    uint8_t y = chip->reg[y_reg] % VID_HEIGHT;

    chip->reg[0xf] = 0;

    // TODO: SET FLAG ON COLLISION
    // Read sprites, XOR to screen
    for (int j = 0; (j < n) && (y + j < 32); j++) {
        // Grab sprite byte
        uint8_t sprite_byte = chip->ram[chip->i + j];
        for (int i = 0; (i < 8) && (x + i < 64); i++) {
            uint8_t sprite_bit = (sprite_byte & (1 << (7-i))) >> (7-i);
            uint8_t curr_bit = chip->vid[64*(y + j) + x + i];
            chip->vid[64*(y + j) + x + i] = _xor(sprite_bit, curr_bit);
            if (sprite_bit == 1 && curr_bit == 1) chip->reg[0xf] = 1;
        }
    }

}

// Skip next instruction if reg equals immediate value
void se(Chip8* chip, uint8_t reg, uint8_t val) {
    if (chip->reg[reg] == val)
        chip->pc = (chip->pc + 2) % 0xFFF;

}

// Skip next instruction if reg equals immediate value
void sne(Chip8* chip, uint8_t reg, uint8_t val) {
    if (chip->reg[reg] != val)
        chip->pc = (chip->pc + 2) % 0xFFF;

}

// Call subroutine
void call(Chip8* chip, uint16_t addr) {
    chip->sp++;
    chip->stack[chip->sp] = chip->pc;
    chip->pc = addr;
}

// OR Value with destination register
void or(Chip8* chip, uint8_t dst, uint8_t val) {
    chip->reg[dst] = chip->reg[dst] | val;
    if (chip->quirk_vf_reset) chip->reg[0xf] = 0;
}

void and(Chip8* chip, uint8_t dst, uint8_t val) {
    chip->reg[dst] = chip->reg[dst] & val;
    if (chip->quirk_vf_reset) chip->reg[0xf] = 0;
}

void xor(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t x = chip->reg[dst];
    uint8_t y = val;
    chip->reg[dst] = (x | y) & ~(x & y);
    if (chip->quirk_vf_reset) chip->reg[0xf] = 0;
}

void sub(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t flag = chip->reg[dst] >= val;
    chip->reg[dst] = chip->reg[dst] - val;
    chip->reg[0xf] = flag;
}

void shr(Chip8* chip, uint8_t dst, uint8_t val) {
    (void) val;
    uint8_t flag = chip->reg[dst] & 1;
    chip->reg[dst] = chip->reg[dst] >> 1;
    chip->reg[0xf] = flag;
}

void subn(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t flag = val >= chip->reg[dst];
    chip->reg[dst] = val - chip->reg[dst];
    chip->reg[0xf] = flag;
}

void shl(Chip8* chip, uint8_t dst, uint8_t val) {
    (void) val;
    uint8_t flag = (chip->reg[dst] & (1 << 7)) >> 7;
    chip->reg[dst] = chip->reg[dst] << 1;
    chip->reg[0xf] = flag;
}

void rnd(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t r = rand() % 256;
    chip->reg[dst] = r & val;
}



