#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"
#include "opcodes.h"
#include "display.h"

#define LOG(...) LOG_HELPER(__VA_ARGS__, "")
#define LOG_HELPER(opc, str, ...) \
     printf("%04d: 0x%04x - " str "%s\n", chip->pc - 2, opc, __VA_ARGS__)

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
    
    // Default State
    chip->state = STATE_HALTED;
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
    uint8_t  xreg = (opc & 0x0f00) >> 8;    // X Register 
    uint8_t  xval = chip->reg[xreg];        // Value in X Register
    uint8_t  yreg = (opc & 0x00f0) >> 4;    // Y Register
    uint8_t  yval = chip->reg[yreg];        // Value in Y Register
    uint8_t  nibb = (opc & 0x000f);         // Last nibble
    uint8_t  ival = (opc & 0x00ff);         // Immediate Value
    uint16_t addr = (opc & 0x0fff);         // 12bit Memory Address

    // Execute opcode
    switch ( (opc & 0xf000) >> 12) {
    case 0x0:
        if (opc == 0x00e0) {
            LOG(opc, "cls");
            cls(chip);
        } else if (opc == 0x00ee) {
            LOG(opc, "ret");
            ret(chip);
        } else {
            LOG(opc, "UNDEFINED OPCODE");
            return 0;
        }
        break;
    case 0x1:
        LOG(opc, "jp %d", addr);
        jp(chip, addr);
        break;
    case 0x2:
        LOG(opc, "call %d", addr);
        call(chip, addr);
        break;
    case 0x3:
        LOG(opc, "se [v%x]=%d, %d", xreg, xval, ival);
        se(chip, xreg, ival);
        break;
    case 0x4:
        LOG(opc, "sne [v%x]=%d, %d", xreg, xval, ival);
        sne(chip, xreg, ival);
        break;
    case 0x5:
        if ((opc & 0xf) != 0) {
            LOG(opc, "UNDEFINED OPCODE");
            return 0;
        }
        LOG(opc, "se [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
        se(chip, xreg, yval);
        break;
    case 0x6:
        LOG(opc, "ld [v%x], %d", xreg, ival);
        ld(chip, xreg, ival);
        break;
    case 0x7:
        LOG(opc, "addnc [v%x]=%d, %d", xreg, xval, ival);
        addnc(chip, xreg, ival);
        break;
    case 0x8: {
        switch (opc & 0xf) {
        case 0:
            LOG(opc, "ld [v%x], [v%x]=%d", xreg, yreg, yval);
            ld(chip, xreg, yval);
            break;
        case 1:
            LOG(opc, "or [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
            or(chip, xreg, yval);
            break;
        case 2:
            LOG(opc, "and [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
            and(chip, xreg, yval);
            break;
        case 3:
            LOG(opc, "xor [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
            xor(chip, xreg, yval);
            break;
        case 4:
            LOG(opc, "add [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
            add(chip, xreg, yval);
            break;
        case 5:
            LOG(opc, "sub [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
            sub(chip, xreg, yval);
            break;
        case 6:
            LOG(opc, "shr [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
            shr(chip, xreg, yval);
            break;
        case 7:
            LOG(opc, "subn [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
            subn(chip, xreg, yval);
            break;
        case 0xe:
            LOG(opc, "shl [v%x]=%d, [v%x]=%d", xreg, xval, yreg, yval);
            shl(chip, xreg, yval);
            break;
        default:
            LOG(opc, "UNDEFINED OPCODE");
            return 0;
        }
        break;
    }
    case 9:
        if ((opc & 0xf) != 0) {
            LOG(opc, "UNDEFINED OPCODE");
            return 0;
        }
        LOG(opc, "sne [v%x]=%d, [v%x]=%d", xreg, xval, yreg, ival);
        sne(chip, xreg, yval);
        break;
    case 0xa:
        LOG(opc, "ldi %d", addr);
        ldi(chip, addr);
        break;
    case 0xb: {
        uint16_t delta = chip->reg[0];
        LOG(opc, "jp %d + [v0]=%d = %d", addr, delta, addr + delta);
        jp(chip, addr + delta);
        break;
    }
    case 0xc:
        LOG(opc, "rnd [v%x], %d", xreg, ival);
        rnd(chip, xreg, ival);
        break;
    case 0xd:
        LOG(opc, "drw [v%x]=%d, [v%x]=%d, %d",xreg,xval,yreg,yval,nibb);
        drw(chip, xreg, yreg, nibb);
        break;
    case 0xe:
        if (ival == 0x9e) {
            LOG(opc, "skp [v%x]=%d", xreg, xval);
            skp(chip, xval);
        } else if (ival == 0xa1) {
            LOG(opc, "sknp [v%x]=%d", xreg, xval);
            sknp(chip, xval);
        } else {
            LOG(opc, "UNDEFINED OPCODE");
            return 0;
        }
        break;
    case 0xf: {
        switch (opc & 0xff) {
        case 0x07:
            LOG(opc, "ld [v%x], [delay]=%d", xval, chip->delay);
            ld(chip, xreg, chip->delay);
            break;
        case 0x0a:
            // Check for a key press
            chip->pc -= 2;
            LOG(opc, "wait until keypress");
            for (uint8_t i = 0; i < 16; i++) {
                bool keypress = (chip->keypad & (1 << i)) >> i;
                if (keypress) {
                    LOG(opc, "ld [v%x], [key]=%d", xreg, i);
                    ld(chip, xreg, i);
                    chip->pc += 2;
                    break;
                }
            }
            break;
        case 0x15:
            LOG(opc, "ld [delay], [v%x]=%d", xreg, xval);
            ldd(chip, xval);
            break;
        case 0x18:
            LOG(opc, "ld [sound], [v%x]=%d", xreg, xval);
            lds(chip, xval);
            break;
        case 0x1e:
            LOG(opc, "add [i]=%d, [v%x]=%d", chip->i, xreg, xval);
            addi(chip, xval);
            break;
        case 0x29:
            LOG(opc, "ld sprite [i], [v%x]=%x", xreg, xval);
            ld_sprite(chip, xval);
            return 0;
        case 0x33:
            LOG(opc, "ld bcd [i], [v%x]=%d", xreg, xval);
            ld_bcd(chip, xval);
            break;
        case 0x55:
            LOG(opc, "str [i], [v0] - [v%x]", xreg);
            str(chip, xreg);
            break;
        case 0x65:
            LOG(opc, "ld [v0] - [v%x], [i]", xreg);
            ldr(chip, xreg);
            break;
        default:
            LOG(opc, "UNDEFINED OPCODE");
            return 0;
        }
        break;
    }
    default:
        LOG(opc, "UNDEFINED OPCODE");
        return 0;
    }

    return 1;

}

// Core execution loop
void loop(Chip8* chip, ChipState state) {

    init_display();

    clock_t start = clock();
    chip->state = state;
    while (display_is_open()) {

        float delta_t = (clock() - start) / (float)CLOCKS_PER_SEC;
        chip->keypad = get_keypad_inputs();

        switch (chip->state) {
        case STATE_RUNNING: {
            if (chip->cycles <= delta_t * chip->cycle_f) {
                cycle(chip);
                chip->cycles++;
            }

            if (chip->clocks <= delta_t * chip->clock_f) {
                send_clock(chip);
                update_display(chip);
                chip->clocks++;
            }
            break;
        }
        case STATE_STEPPING: {
            // Update display and clock at 60fps
            if (chip->clocks <= delta_t * chip->clock_f) {

                // Step forward when space is pressed
                if (is_space_pressed()) {
                    cycle(chip);
                    chip->cycles++;
                }
                send_clock(chip);
                update_display(chip);
                chip->clocks++;
            }
            break;
        }
        case STATE_HALTED:
            // Update display at 60fps
            if (chip->clocks <= delta_t * chip->clock_f) {
                update_display(chip);
            }
            break;
        }

        // Each Frame, check for state changes from pressing p, s, r
        if (is_p_pressed()) {
            chip->state = STATE_HALTED;
        }
        if (is_space_pressed()) {
            if (chip->state != STATE_STEPPING) {
                chip->cycles = 0;
                chip->clocks = 0;
                start = clock();
            }
            chip->state = STATE_STEPPING;
        }
        if (is_enter_pressed()) {
            if (chip->state != STATE_RUNNING) {
                chip->cycles = 0;
                chip->clocks = 0;
                start = clock();
            }
            chip->state = STATE_RUNNING;
        }
    }

    end_display();
    printf("fin.\n");

}

// Run loop
void run(Chip8* chip) {
    loop(chip, STATE_RUNNING);
}

// Step through loop
void step(Chip8* chip) {
    loop(chip, STATE_STEPPING);
}

// Dump VM State
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

// Dump RAM Contents
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

