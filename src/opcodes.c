#include <stdlib.h>

#include "chip8.h"

// XOR two values together
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
void drw(Chip8* chip, uint8_t xreg, uint8_t yreg, uint8_t n) {

    // Find drawing coordinates
    uint8_t x = chip->reg[xreg] % VID_WIDTH;
    uint8_t y = chip->reg[yreg] % VID_HEIGHT;

    chip->reg[0xf] = 0;

    // Read sprites, XOR to screen
    for (int j = 0; (j < n) && (y + j < VID_HEIGHT); j++) {
        // Grab sprite byte from memory
        uint8_t sprite_byte = chip->ram[chip->i + j];
        for (int i = 0; (i < 8) && (x + i < VID_WIDTH); i++) {
            // Pick out sprite bit and current display bit, then xor
            uint8_t sbit = (sprite_byte & (1 << (7 - i))) >> (7 - i);
            uint8_t dbit = chip->vid[VID_WIDTH * (y + j) + x + i];
            chip->vid[VID_WIDTH * (y + j) + x + i] = _xor(sbit, dbit);
            // Set flag register if there was a collision
            if (sbit && dbit) chip->reg[0xf] = 1;
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

// Return true if key is pressed
static bool key_is_pressed(Chip8* chip, uint8_t key) {
    bool keypress = (chip->keypad & (1 << key)) >> key;
    return keypress;
}

// Skip next instruction if key is pressed
void skp(Chip8* chip, uint8_t key) {
    if (key_is_pressed(chip, key))
        chip->pc = (chip->pc + 2) % 0xFFF;
}

// Skip next instruction if key is pressed
void sknp(Chip8* chip, uint8_t key) {
    if (!key_is_pressed(chip, key))
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

// AND value with destination register
void and(Chip8* chip, uint8_t dst, uint8_t val) {
    chip->reg[dst] = chip->reg[dst] & val;
    if (chip->quirk_vf_reset) chip->reg[0xf] = 0;
}

// XOR value with destination register
void xor(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t x = chip->reg[dst];
    uint8_t y = val;
    chip->reg[dst] = (x | y) & ~(x & y);
    if (chip->quirk_vf_reset) chip->reg[0xf] = 0;
}

// Subtract value from destination register
void sub(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t flag = chip->reg[dst] >= val;
    chip->reg[dst] = chip->reg[dst] - val;
    chip->reg[0xf] = flag;
}

// Shift register right
void shr(Chip8* chip, uint8_t dst, uint8_t val) {
    (void) val;
    uint8_t flag = chip->reg[dst] & 1;
    chip->reg[dst] = chip->reg[dst] >> 1;
    chip->reg[0xf] = flag;
}

// Subtract destination value from value
void subn(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t flag = val >= chip->reg[dst];
    chip->reg[dst] = val - chip->reg[dst];
    chip->reg[0xf] = flag;
}

// Shift register left
void shl(Chip8* chip, uint8_t dst, uint8_t val) {
    (void) val;
    uint8_t flag = (chip->reg[dst] & (1 << 7)) >> 7;
    chip->reg[dst] = chip->reg[dst] << 1;
    chip->reg[0xf] = flag;
}

// Generate random number, ANDed with value
void rnd(Chip8* chip, uint8_t dst, uint8_t val) {
    uint8_t r = rand() % 256;
    chip->reg[dst] = r & val;
}

// Load value into delay timer
void ldd(Chip8* chip, uint8_t val) {
    chip->delay = val;
}

// Load value into delay timer
void lds(Chip8* chip, uint8_t val) {
    chip->sound = val;
}

// Load value into I register
void addi(Chip8* chip, uint8_t val) {
    chip->i += val;
}

// Load char pointer into i
void ld_sprite(Chip8* chip, uint8_t val) {
    chip->i = FONT_VECTOR + 5 * val;
}

// Load binary coded decimal value into i, i+1, i+2
void ld_bcd(Chip8* chip, uint8_t val) {
    chip->ram[chip->i] = val / 100;
    chip->ram[chip->i + 1] = val % 100 / 10;
    chip->ram[chip->i + 2] = val % 10;
}

// Store registers 0-x in memory starting at i
void str(Chip8* chip, uint8_t xreg) {
    for (uint8_t i = 0; i <= xreg; i++) {
        chip->ram[chip->i + i] = chip->reg[i];
    }
    if (chip->quirk_memory) chip->i += xreg;
}

// Load registers 0-x from memory starting at i
void ldr(Chip8* chip, uint8_t xreg) {
    for (uint8_t i = 0; i <= xreg; i++) {
        chip->reg[i] = chip->ram[chip->i + i];
    }
    if (chip->quirk_memory) chip->i += xreg;
}

