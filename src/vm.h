#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdint.h>

#define DISPLAY_WIDTH (64)
#define DISPLAY_HEIGHT (32)
#define RESET_VECTOR (0x200)
#define FONT_VECTOR (0x50)

typedef struct VM {

    uint16_t pc;            // Program Counter
    uint16_t i;             // Memory Address Register
    uint8_t  reg[16];       // Registers
    uint8_t  sp;            // Stack Pointer
    uint16_t stack[16];     // Stack

    uint8_t delay;          // Delay Timer
    uint8_t sound;          // Timer Register
    
    uint8_t ram[0xfff];     // Ram
    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT]; // Video memory

    uint8_t keypad[16];     // Keypress Registers

} VM;

void init_vm(VM* vm);
void load_rom(VM* vm, const char* path);

void dump_state(VM* vm);
void dump_ram(VM* vm);
void dump_display(VM* vm);

uint8_t cycle(VM* vm);      // Execute single instruction
void send_clock(VM* vm);    // Trigger clock signal at 60Hz

#endif  // VM_H

