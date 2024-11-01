#include <stdlib.h>

#include "vm.h"

uint8_t _xor(uint8_t x, uint8_t y) {
    return (x || y) && !(x && y);
}

// Clear display
void cls(VM* vm) {
    for (int i = 0; i < DISPLAY_WIDTH*DISPLAY_HEIGHT; i++) vm->display[i] = 0;
}

// Return from subroutine
void ret(VM* vm) {
    // Pop address from top of stack
    vm->pc = vm->stack[vm->sp];
    vm->sp--;
}

// Jump
void jp(VM* vm, uint16_t addr) {
    vm->pc = addr;
}

// Load value into register
void ld(VM* vm, uint8_t dst, uint8_t val) {
    vm->reg[dst] = val;
}

// Add value to register, don't set carry flag
void addnc(VM* vm, uint8_t dst, uint8_t val) {
    vm->reg[dst] += val;
}

// Add value to register
void add(VM* vm, uint8_t dst, uint8_t val) {
    uint8_t x_init = vm->reg[dst];
    vm->reg[dst] += val;

    uint8_t flag = 0;
    if (vm->reg[dst] < x_init) flag = 1;
    vm->reg[0xf] = flag;
}

// Load value into I register
void ldi(VM* vm, uint16_t addr) {
    vm->i = addr;
}

// Draw n-byte sprite to screen
void drw(VM* vm, uint8_t x_reg, uint8_t y_reg, uint8_t n) {

    // Find drawing coordinates
    uint8_t x = vm->reg[x_reg] % DISPLAY_WIDTH;
    uint8_t y = vm->reg[y_reg] % DISPLAY_HEIGHT;

    // TODO: SET FLAG ON COLLISION
    // Read sprites, XOR to screen
    for (int j = 0; (j < n) && (y + j < 32); j++) {
        // Grab sprite byte
        uint8_t sprite_byte = vm->ram[vm->i + j];
        for (int i = 0; (i < 8) && (x + i < 64); i++) {
            uint8_t sprite_bit = (sprite_byte & (1 << (7-i))) >> (7-i);
            uint8_t curr_bit = vm->display[64*(y + j) + x + i];
            vm->display[64*(y + j) + x + i] = _xor(sprite_bit, curr_bit);
        }
    }

}

// Skip next instruction if reg equals immediate value
void se(VM* vm, uint8_t reg, uint8_t val) {
    if (vm->reg[reg] == val)
        vm->pc = (vm->pc + 2) % 0xFFF;

}

// Skip next instruction if reg equals immediate value
void sne(VM* vm, uint8_t reg, uint8_t val) {
    if (vm->reg[reg] != val)
        vm->pc = (vm->pc + 2) % 0xFFF;

}

// Call subroutine
void call(VM* vm, uint16_t addr) {
    vm->sp++;
    vm->stack[vm->sp] = vm->pc;
    vm->pc = addr;
}

// OR Value with destination register
void or(VM* vm, uint8_t dst, uint8_t val) {
    vm->reg[dst] = vm->reg[dst] | val;
}

void and(VM* vm, uint8_t dst, uint8_t val) {
    vm->reg[dst] = vm->reg[dst] & val;
}

void xor(VM* vm, uint8_t dst, uint8_t val) {
    uint8_t x = vm->reg[dst];
    uint8_t y = val;
    vm->reg[dst] = (x | y) & ~(x & y);
}

void sub(VM* vm, uint8_t dst, uint8_t val) {
    uint8_t flag = vm->reg[dst] > val;
    vm->reg[dst] = vm->reg[dst] - val;
    vm->reg[0xf] = flag;
}

void shr(VM* vm, uint8_t dst, uint8_t val) {
    (void) val;
    uint8_t flag = vm->reg[dst] & 1;
    vm->reg[dst] = vm->reg[dst] >> 1;
    vm->reg[0xf] = flag;
}

void subn(VM* vm, uint8_t dst, uint8_t val) {
    uint8_t flag = val > vm->reg[dst];
    vm->reg[dst] = val - vm->reg[dst];
    vm->reg[0xf] = flag;
}

void shl(VM* vm, uint8_t dst, uint8_t val) {
    (void) val;
    uint8_t flag = (vm->reg[dst] & (1 << 7)) >> 7;
    vm->reg[dst] = vm->reg[dst] << 1;
    vm->reg[0xf] = flag;
}

void rnd(VM* vm, uint8_t dst, uint8_t val) {
    uint8_t r = rand() % 256;
    vm->reg[dst] = r & val;
}



