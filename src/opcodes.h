#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "chip8.h"

void cls(Chip8* chip);
void ret(Chip8* chip);
void jp(Chip8* chip, uint16_t addr);
void ld(Chip8* chip, uint8_t dst, uint8_t val);
void add(Chip8* chip, uint8_t dst, uint8_t val);
void ldi(Chip8* chip, uint16_t addr);
void drw(Chip8* chip, uint8_t x_reg, uint8_t y_reg, uint8_t n);
void se(Chip8* chip, uint8_t reg, uint8_t val);
void sne(Chip8* chip, uint8_t reg, uint8_t val);
void skp(Chip8* chip, uint8_t key);
void sknp(Chip8* chip, uint8_t key);
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
void ldd(Chip8* chip, uint8_t val);
void lds(Chip8* chip, uint8_t val);
void addi(Chip8* chip, uint8_t val);
void ld_sprite(Chip8* chip, uint8_t val);
void ld_bcd(Chip8* chip, uint8_t val);
void str(Chip8* chip, uint8_t xreg);
void ldr(Chip8* chip, uint8_t xreg);

#endif // INSTRUCTIONS_H

