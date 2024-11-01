#include "../include/raylib.h"
#include "display.h"
#include "chip8.h"

void init_display(void) {

    InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "chip-8");

}

void draw_pixel(uint8_t x, uint8_t y, Color c) {

    DrawRectangle(x * PIXEL_SIZE,y * PIXEL_SIZE,PIXEL_SIZE,PIXEL_SIZE,c);
}

bool display_is_open(void) {
    return !WindowShouldClose();
}

uint16_t get_keypad_inputs(void) {
    
    // Keys corresponding to 0-F inputs
    int key_dict[] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR,
                    KEY_Q, KEY_W, KEY_E, KEY_R,
                    KEY_A, KEY_S, KEY_D, KEY_F,
                    KEY_Z, KEY_X, KEY_C, KEY_V};

    // Read keys from bottom up
    uint16_t keypad = 0;
    for (int k = 15; k >= 0; k--) {
        keypad = keypad << 1;
        keypad = keypad | IsKeyDown(key_dict[k]);
    }

    return keypad;
}

void update_display(Chip8* chip) {

    // First clear display
    BeginDrawing();
    ClearBackground(WHITE);

    // Now draw VM video memory
    for (int y = 0; y < VID_HEIGHT; y++) {
        for (int x = 0; x < VID_WIDTH; x++) {
            if (chip->vid[y * VID_WIDTH + x] == 1)
                draw_pixel(x,y,WHITE);
            else draw_pixel(x,y,BLACK);
        }
    }
    EndDrawing();
}

void end_display(void) {
    CloseWindow();
}
