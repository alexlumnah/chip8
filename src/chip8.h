#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define VID_WIDTH (64)
#define VID_HEIGHT (32)
#define RESET_VECTOR (0x200)
#define FONT_VECTOR (0x50)

typedef enum {
    STATE_HALTED,
    STATE_RUNNING,
    STATE_STEPPING,
} ChipState;

typedef struct Chip8 {

    // Registers
    uint16_t pc;            // Program Counter
    uint16_t i;             // Memory Address Register
    uint8_t  reg[16];       // Registers
    uint8_t  sp;            // Stack Pointer
    uint16_t stack[16];     // Stack

    uint8_t delay;          // Delay Timer
    uint8_t sound;          // Timer Register
    uint16_t keypad;        // Keypress Register
    
    // Memory
    uint8_t ram[0xfff];     // Ram
    uint8_t vid[VID_WIDTH * VID_HEIGHT]; // Video memory

    // Quirks
    bool quirk_vf_reset;    // Flag Reset Quirk
    bool quirk_memory;      // Memory Quirk
    bool quirk_disp_wait;   // Wait for display vsync quirk
    bool quirk_clip;        // Clipping Quirk
    bool quirk_shift;       // Shifting Quirk
    bool quirk_jump;        // Jumping Quirk

    // Clocking
    float clock_f;          // Clock Frequency
    float cycle_f;          // Cycle Frequency
    long cycles;            // Number of cycles executed
    long clocks;            // Number of clock pulses sent

    // State
    ChipState state;        // Chip State

} Chip8;

void init_chip8(Chip8* chip);                   // Initialize VM
void load_rom(Chip8* chip, const char* path);   // Load rom into memory

void dump_state(Chip8* chip);                   // Dump VM State
void dump_ram(Chip8* chip);                     // Dump RAM
void dump_display(Chip8* chip);                 // Draw Display in ASCII

void run(Chip8* chip);                          // Run VM indefinitely
void step(Chip8* chip);                         // Step through cycles

#endif  // CHIP8_H

