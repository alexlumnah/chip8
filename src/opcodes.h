#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "vm.h"

// Clear display
void cls(VM* vm);

// Return from subroutine
void ret(VM* vm);

// Jump
void jp(VM* vm, uint16_t addr);

// Load value into register
void ld(VM* vm, uint8_t dst, uint8_t val);

// Add value to register
void add(VM* vm, uint8_t dst, uint8_t val);

// Load value into I register
void ldi(VM* vm, uint16_t addr);

// Draw n-byte sprite to screen
void drw(VM* vm, uint8_t x_reg, uint8_t y_reg, uint8_t n);

void se(VM* vm, uint8_t reg, uint8_t val);
void sne(VM* vm, uint8_t reg, uint8_t val);
void call(VM* vm, uint16_t addr);
void addnc(VM* vm, uint8_t dst, uint8_t val);
void or(VM* vm, uint8_t dst, uint8_t val);
void and(VM* vm, uint8_t dst, uint8_t val);
void xor(VM* vm, uint8_t dst, uint8_t val);
void sub(VM* vm, uint8_t dst, uint8_t val);
void shr(VM* vm, uint8_t dst, uint8_t val);
void subn(VM* vm, uint8_t dst, uint8_t val);
void shl(VM* vm, uint8_t dst, uint8_t val);
void rnd(VM* vm, uint8_t dst, uint8_t val);

#endif // INSTRUCTIONS_H

