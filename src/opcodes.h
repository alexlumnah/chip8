#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "chip8.h"

// Clear display
void cls(Chip8* chip);

// Return from subroutine
void ret(Chip8* chip);

// Jump
void jp(Chip8* chip, uint16_t addr);

// Load value into register
void ld(Chip8* chip, uint8_t dst, uint8_t val);

// Add value to register
void add(Chip8* chip, uint8_t dst, uint8_t val);

// Load value into I register
void ldi(Chip8* chip, uint16_t addr);

// Draw n-byte sprite to screen
void drw(Chip8* chip, uint8_t x_reg, uint8_t y_reg, uint8_t n);

void se(Chip8* chip, uint8_t reg, uint8_t val);
void sne(Chip8* chip, uint8_t reg, uint8_t val);
void call(Chip8* chip, uint16_t addr);
void addnc(Chip8* chip, uint8_t dst, uint8_t val);
void or(Chip8* chip, uint8_t dst, uint8_t val);
void and(Chip8* chip, uint8_t dst, uint8_t val);
void xor(Chip8* chip, uint8_t dst, uint8_t val);
void sub(Chip8* chip, uint8_t dst, uint8_t val);
void shr(Chip8* chip, uint8_t dst, uint8_t val);
void subn(Chip8* chip, uint8_t dst, uint8_t val);
void shl(Chip8* chip, uint8_t dst, uint8_t val);
void rnd(Chip8* chip, uint8_t dst, uint8_t val);

#endif // INSTRUCTIONS_H

