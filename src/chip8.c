#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"
#include "opcodes.h"
#include "display.h"

// Initialize Chip8 VM
void init_chip8(Chip8* chip) {

    // Set program counter to 0x200
    chip->pc = RESET_VECTOR;

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
    for (uint8_t i = 0; i < 16 * 5; i++)
        chip->ram[FONT_VECTOR + i] = font[i];

    // Configure quirks for classic chip-8
    chip->quirk_vf_reset = true;
    chip->quirk_memory = true;
    chip->quirk_disp_wait = true;
    chip->quirk_clip = true;
    chip->quirk_shift = false;
    chip->quirk_jump = false;

    // Configure Clock/Cycle Frequencies
    chip->clock_f = 60.0;
    chip->cycle_f = 700;
    chip->clocks = 0;
    chip->cycles = 0;
}

// Load a rom from file
void load_rom(Chip8* chip, const char* path) {

    FILE* f = fopen(path, "rb");

    // TODO: Handle this more gracefully
    assert(f != NULL && "Unable to open ROM file!");

    // read rom into ram at reset vector
    uint16_t p = RESET_VECTOR;
    while (p < 0xfff && fread(chip->ram + p, 1, 1, f)) {
        p++;
    }
}

// Trigger clock signal, expect to be called at 60Hz
void send_clock(Chip8* chip) {
    if (chip->delay > 0) chip->delay--;
    if (chip->sound > 0) chip->sound--;
}

// Execute fetch/decode/execute cycle
uint8_t cycle(Chip8* chip) {

    // Fetch next opcode
    uint16_t opc = (chip->ram[chip->pc] << 8) + chip->ram[chip->pc + 1];
    chip->pc = (chip->pc + 2) % 0x0ffe;

    // Decode opcode
    switch ( (opc & 0xf000) >> 12) {
    case 0x0:
        if (opc == 0x00e0) {
            cls(chip);
            printf("cls\n");
        } else if (opc == 0x00ee) {
            ret(chip);
            printf("ret\n");
        } else {
            printf("ERROR: INVALID OPCODE: %x\n",opc);
            return 0;
        }
        break;
    case 0x1: {
        uint16_t addr = opc & 0xfff;
        jp(chip, addr);
        printf("jp to %d\n", addr);
        break;
    }
    case 0x2: {
        uint16_t addr = opc & 0xfff;
        call(chip, addr);
    }
    case 0x3: {
        uint8_t reg = (opc & 0xf00) >> 8;
        uint8_t val = (opc & 0xff);
        se(chip, reg, val);
        break;
    }
    case 0x4: {
        uint8_t reg = (opc & 0xf00) >> 8;
        uint8_t val = (opc & 0xff);
        sne(chip, reg, val);
        break;
    }
    case 0x5: {
        if ((opc & 0xf) != 0) {
            printf("ERROR: INVALID OPCODE: %x\n",opc);
            return 0;
        }
        uint8_t x = (opc & 0xf00) >> 8;
        uint8_t y = (opc & 0x0f0) >> 4;
        se(chip, x, chip->reg[y]);
        break;
    }
    case 0x6: {
        uint8_t dst = (opc & 0xf00) >> 8;
        uint8_t val = opc & 0xff;
        ld(chip,dst,val);
        printf("ld %d, %d\n", dst, val);
        break;
    }
    case 0x7: {
        uint8_t dst = (opc & 0xf00) >> 8;
        uint8_t val = opc & 0xff;
        addnc(chip,dst,val);
        printf("addnc %d, %d\n", dst, val);
        break;
    }
    case 0x8: {
        uint8_t dst = (opc & 0xf00) >> 8;
        uint8_t src = (opc & 0x0f0) >> 4;
        uint8_t val = chip->reg[src];
        switch (opc & 0xf) {
        case 0: {
            ld(chip,dst,val);
            break;
        }
        case 1: {
            or(chip,dst,val);
            break;
        }
        case 2: {
            and(chip,dst,val);
            break;
        }
        case 3: {
            xor(chip,dst,val);
            break;
        }
        case 4: {
            add(chip,dst,val);
            break;
        }
        case 5: {
            sub(chip,dst,val);
            break;
        }
        case 6: {
            shr(chip,dst,val);
            break;
        }
        case 7: {
            subn(chip,dst,val);
            break;
        }
        case 0xe: {
            shl(chip,dst,val);
            break;
        }
        default: {
            printf("invalid opcode: %04x",opc);
            return 0;
        }
    }
        break;
    }
    case 9: {
        if ((opc & 0xf) != 0) {
            printf("invalid opcode: %04x",opc);
            return 0;
            break;
        }
        uint8_t reg = (opc & 0xf00) >> 8;
        uint8_t src = (opc & 0x0f0) >> 4;
        uint8_t val = chip->reg[src];
        sne(chip, reg, val);
        printf("sne %d %d\n", reg, val);
        break;
    }
    case 0xa: {
        uint16_t addr = opc & 0xfff;
        ldi(chip, addr);
        printf("ldi %d\n", addr);
        break;
    }
    case 0xb: {
        uint16_t addr = opc & 0xfff;
        uint16_t delta = chip->reg[0];
        jp(chip, addr + delta);
        printf("jp to %d\n", addr + delta);
        break;
    }
    case 0xc: {
        uint8_t dst = (opc & 0xf00) >> 8;
        uint8_t val = (opc & 0x0ff);
        rnd(chip, dst, val);
    }
    case 0xd: {
        uint8_t x = (opc & 0x0f00) >> 8;
        uint8_t y = (opc & 0x00f0) >> 4;
        uint8_t n = (opc & 0x000f);
        drw(chip, x, y, n);
        printf("drw %d, %d, %d\n",x,y,n);
        break;
    }
    case 0xe: {
        uint8_t x = (opc & 0x0f00) >> 8;
        bool key_pressed = (chip->keypad & (1 << chip->reg[x])) >> chip->reg[x];
        if ((opc & 0x00ff) != 0x9e && (opc & 0x00ff) != 0xa1) {
            printf("invalid opcode: %04x",opc);
            return 0;
        } else if ((opc & 0x00ff) == 0x9e && key_pressed) {
            chip->pc += 2;
        } else if ((opc & 0x00ff) == 0xa1 && !key_pressed) {
            chip->pc += 2;
        }
        break;
    }
    case 0xf: {
        uint8_t x = (opc & 0x0f00) >> 8;
        switch (opc & 0xff) {
        case 0x07: {
            ld(chip, x, chip->delay);
            break;
        }
        case 0x0a: {
            // Check for a key press
            chip->pc -= 2;
            for (uint8_t i = 0; i < 16; i++) {
                bool key_pressed = (chip->keypad & (1 << i)) >> i;
                if (key_pressed) {
                    ld(chip, x, i);
                    chip->pc += 2;
                    break;
                }
            }
            break;
        }
        case 0x15: {
            chip->delay = chip->reg[x];
            break;
        }
        case 0x18: {
            chip->sound = chip->reg[x];
            break;
        }
        case 0x1e: {
            chip->i += chip->reg[x];
            break;
        }
        case 0x29: {
            chip->i = FONT_VECTOR + 5 * chip->reg[x];
            return 0;
        }
        case 0x33: {
            uint8_t val = chip->reg[x];
            chip->ram[chip->i] = val / 100;
            chip->ram[chip->i + 1] = val % 100 / 10;
            chip->ram[chip->i + 2] = val % 10;
            break;
        }
        case 0x55: {
            for (uint8_t i = 0; i <= x; i++) {
                chip->ram[chip->i + i] = chip->reg[i];
            }
            if (chip->quirk_memory) chip->i += x;
            break;
        }
        case 0x65: {
            for (uint8_t i = 0; i <= x; i++) {
                chip->reg[i] = chip->ram[chip->i + i];
            }
            if (chip->quirk_memory) chip->i += x;
            break;
        }
        default:
            printf("ERROR: INVALID OPCODE: %x\n",opc);
            return 0;
        }
        break;
    }
    default:
        printf("ERROR: INVALID OPCODE: %x\n",opc);
        return 0;
    }

    return 1;

}

void run(Chip8* chip) {

    init_display();

    clock_t start = clock();
    while (display_is_open()) {
        float delta_t = (clock() - start) / (float)CLOCKS_PER_SEC;

        chip->keypad = get_keypad_inputs();
        
        if (chip->cycles <= delta_t * chip->cycle_f) {
            cycle(chip);
            chip->cycles++;
        }

        if (chip->clocks <= delta_t * chip->clock_f) {
            send_clock(chip);
            update_display(chip);
            chip->clocks++;
        }

    }

    end_display();
    printf("fin.\n");

}

void dump_state(Chip8* chip) {

    printf("VM = {\n");

    printf("  Registers:\n");
    printf("    pc: %d\n",chip->pc);
    printf("    sp: %d\n",chip->sp);
    printf("     i: %d\n",chip->i);
    for (uint8_t i = 0; i < 0xf; i++) {
        printf("    v%x: %d\n", i, chip->reg[i]);
    }

    printf("  Timers:\n");
    printf("    delay: %d\n",chip->delay);
    printf("    sound: %d\n",chip->sound);

    printf("  Stack:\n");
    for (uint8_t i = 0; i < 0xf; i++) {
        printf("    [0x%x] %d", i, chip->stack[i]);
        if (i == chip->sp) printf(" <-");
        printf("\n");
    }

    printf("}\n");
}

void dump_ram(Chip8* chip) {

    for (uint16_t j = 0; j < 0xff; j++) {
        printf("%05d: ", j*0xf);
        for (uint16_t i = 0; i < 0xf; i++) {
            printf("%02x ",chip->ram[j * 0xf + i]);
        }
        printf("\n");
    }
}

// Dump video memory
void dump_video_memory(Chip8* chip) {

    for (int y = 0; y < VID_HEIGHT; y++) {
        for (int x = 0; x < VID_WIDTH; x++) {
            if (chip->vid[y*VID_WIDTH + x] == 1) printf("X");
            else printf(".");
        }
        printf("\n");
    }
    printf("\n");
}

