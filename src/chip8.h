#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdint.h>

#include "vm.h"

typedef struct Chip8 {
    VM* vm;
    float clock_f;          // Clock Frequency
    float cycle_f;          // Cycle Frequency

    long cycles;            // Number of cycles executed
    long clocks;            // Number of clock pulses sent
} Chip8;

void init_chip8(Chip8* chip);

void run(Chip8* chip);

void init_screen(void);
void update_screen(Chip8* chip);
void end_screen(void);

#endif  // CHIP8_H

