#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "vm.h"
#include "opcodes.h"

//  typedef struct VM {
//  
//      uint8_t ram[0xfff]; // vm-8 ram
//      uint8_t reg[16];    // vm-8 registers
//      uint16_t stack[16]; // stack
//      uint16_t pc;        // program counter
//      uint8_t sp;         // stack pointer
//      uint16_t i;         // memory address register
//  
//      uint64_t display[32]    // video memory
//      uint8_t delay;          // delay timer
//      uint8_t timer;          // timer register
//  } VM;

// Initialize virtual machine
void init_vm(VM* vm) {

    // Set program counter to 0x200
    vm->pc = RESET_VECTOR;

    // Setup Font Sprites
    uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for (uint8_t i = 0; i < 16 * 5; i++) vm->ram[FONT_VECTOR + i] = font[i];
}

// load a rom from file
void load_rom(VM* vm, const char* path) {

    FILE* f = fopen(path, "rb");

    // TODO: Handle this more gracefully
    assert(f != NULL && "Unable to open ROM file!");

    // read rom into ram at reset vector
    uint16_t p = RESET_VECTOR;
    while (p < 0xfff && fread(vm->ram + p, 1, 1, f)) {
        p++;
    }
}


void dump_state(VM* vm) {

    printf("VM = {\n");

    printf("  Registers:\n");
    printf("    pc: %d\n",vm->pc);
    printf("    sp: %d\n",vm->sp);
    printf("     i: %d\n",vm->i);
    for (uint8_t i = 0; i < 0xf; i++) {
        printf("    v%x: %d\n", i, vm->reg[i]);
    }

    printf("  Timers:\n");
    printf("    delay: %d\n",vm->delay);
    printf("    sound: %d\n",vm->sound);

    printf("  Stack:\n");
    for (uint8_t i = 0; i < 0xf; i++) {
        printf("    [0x%x] %d", i, vm->stack[i]);
        if (i == vm->sp) printf(" <-");
        printf("\n");
    }

    printf("}\n");
}

void dump_ram(VM* vm) {

    for (uint16_t j = 0; j < 0xff; j++) {
        printf("%05d: ", j*0xf);
        for (uint16_t i = 0; i < 0xf; i++) {
            printf("%02x ",vm->ram[j * 0xf + i]);
        }
        printf("\n");
    }
}

void dump_display(VM* vm) {

    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            if (vm->display[y*DISPLAY_WIDTH + x] == 1) printf("X");
            else printf(".");
        }
        printf("\n");
    }
    printf("\n");
}

// Execute fetch/decode/execute cycle
uint8_t cycle(VM* vm) {

    // Fetch next opcode
    uint16_t opc = (vm->ram[vm->pc] << 8) + vm->ram[vm->pc + 1];
    vm->pc = (vm->pc + 2) % 0x0ffe;

    // Decode opcode
    switch ( (opc & 0xf000) >> 12) {
    case 0x0:
        if (opc == 0x00e0) {
            cls(vm);
            //printf("cls\n");
        } else if (opc == 0x00ee) {
            ret(vm);
            //printf("ret\n");
        } else {
            //printf("ERROR: INVALID OPCODE: %x\n",opc);
            return 0;
        }
        break;
    case 0x1: {
        uint16_t addr = opc & 0xfff;
        jp(vm, addr);
        //printf("jp to %d\n", addr);
        break;
    }
    case 0x2: {
        uint16_t addr = opc & 0xfff;
        call(vm, addr);
    }
    case 0x3: {
        uint8_t reg = (opc & 0xf00) >> 8;
        uint8_t val = (opc & 0xff);
        se(vm, reg, val);
        break;
    }
    case 0x4: {
        uint8_t reg = (opc & 0xf00) >> 8;
        uint8_t val = (opc & 0xff);
        sne(vm, reg, val);
        break;
    }
    case 0x5: {
        if ((opc & 0xf) != 0) {
            //printf("ERROR: INVALID OPCODE: %x\n",opc);
            return 0;
        }
        uint8_t x = (opc & 0xf00) >> 8;
        uint8_t y = (opc & 0x0f0) >> 4;
        se(vm, x, vm->reg[y]);
        break;
    }
    case 0x6: {
        uint8_t dst = (opc & 0xf00) >> 8;
        uint8_t val = opc & 0xff;
        ld(vm,dst,val);
        //printf("ld %d, %d\n", dst, val);
        break;
    }
    case 0x7: {
        uint8_t dst = (opc & 0xf00) >> 8;
        uint8_t val = opc & 0xff;
        addnc(vm,dst,val);
        //printf("addnc %d, %d\n", dst, val);
        break;
    }
    case 0x8: {
        uint8_t dst = (opc & 0xf00) >> 8;
        uint8_t src = (opc & 0x0f0) >> 4;
        uint8_t val = vm->reg[src];
        switch (opc & 0xf) {
        case 0: {
            ld(vm,dst,val);
            break;
        }
        case 1: {
            or(vm,dst,val);
            break;
        }
        case 2: {
            and(vm,dst,val);
            break;
        }
        case 3: {
            xor(vm,dst,val);
            break;
        }
        case 4: {
            add(vm,dst,val);
            break;
        }
        case 5: {
            sub(vm,dst,val);
            break;
        }
        case 6: {
            shr(vm,dst,val);
            break;
        }
        case 7: {
            subn(vm,dst,val);
            break;
        }
        case 0xe: {
            shl(vm,dst,val);
            break;
        }
        default: {
            //printf("invalid opcode: %04x",opc);
            return 0;
        }
    }
        break;
    }
    case 9: {
        if ((opc & 0xf) != 0) {
            //printf("invalid opcode: %04x",opc);
            return 0;
            break;
        }
        uint8_t reg = (opc & 0xf00) >> 8;
        uint8_t src = (opc & 0x0f0) >> 4;
        uint8_t val = vm->reg[src];
        sne(vm, reg, val);
        //printf("sne %d %d\n", reg, val);
        break;
    }
    case 0xa: {
        uint16_t addr = opc & 0xfff;
        ldi(vm, addr);
        //printf("ldi %d\n", addr);
        break;
    }
    case 0xb: {
        uint16_t addr = opc & 0xfff;
        uint16_t delta = vm->reg[0];
        jp(vm, addr + delta);
        //printf("jp to %d\n", addr + delta);
        break;
    }
    case 0xc: {
        uint8_t dst = (opc & 0xf00) >> 8;
        uint8_t val = (opc & 0x0ff);
        rnd(vm, dst, val);
    }
    case 0xd: {
        uint8_t x = (opc & 0x0f00) >> 8;
        uint8_t y = (opc & 0x00f0) >> 4;
        uint8_t n = (opc & 0x000f);
        drw(vm, x, y, n);
        //printf("drw %d, %d, %d\n",x,y,n);
        break;
    }
    case 0xe: {
        uint8_t x = (opc & 0x0f00) >> 8;
        if (vm->keypad[vm->reg[x]] == 0) vm->pc += 2;
        break;
    }
    case 0xf: {
        uint8_t x = (opc & 0x0f00) >> 8;
        switch (opc & 0xff) {
        case 0x07: {
            ld(vm, x, vm->delay);
            break;
        }
        case 0x0a: {
            // Check for a key press
            vm->pc -= 2;
            for (uint8_t i = 0; i < 16; i++) {
                if (vm->keypad[i] == 1) {
                    ld(vm, x, i);
                    vm->pc += 2;
                    break;
                }
            }
            break;
        }
        case 0x15: {
            vm->delay = vm->reg[x];
            break;
        }
        case 0x18: {
            vm->sound = vm->reg[x];
            break;
        }
        case 0x1e: {
            vm->i += vm->reg[x];
            break;
        }
        case 0x29: {
            vm->i = FONT_VECTOR + 5 * vm->reg[x];
            return 0;
        }
        case 0x33: {
            uint8_t val = vm->reg[x];
            vm->ram[vm->i] = val / 100;
            vm->ram[vm->i + 1] = val % 100 / 10;
            vm->ram[vm->i + 2] = val % 10;
            break;
        }
        case 0x55: {
            for (uint8_t i = 0; i <= x; i++) {
                vm->ram[vm->i + i] = vm->reg[i];
            }
            break;
        }
        case 0x65: {
            for (uint8_t i = 0; i <= x; i++) {
                vm->reg[i] = vm->ram[vm->i + i];
            }
            break;
        }
        default:
            //printf("ERROR: INVALID OPCODE: %x\n",opc);
            return 0;
        }
        break;
    }
    default:
        //printf("ERROR: INVALID OPCODE: %x\n",opc);
        return 0;
    }

    return 1;

}

// Trigger clock signal, expect to be called at 60Hz
void send_clock(VM* vm) {
    if (vm->delay > 0) vm->delay--;
    if (vm->sound > 0) vm->sound--;
}

