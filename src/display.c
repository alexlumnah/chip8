#include "../include/raylib.h"
#include "display.h"
#include "chip8.h"

#define FONT_PATH "lib/Courier New Bold.ttf"

Font db_font;
Font ram_font;

// Initialize Window
void init_display(void) {

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "chip-8");
    db_font = LoadFontEx(FONT_PATH,  DEBUG_TEXT_SIZE, 0, 250);
    ram_font = LoadFontEx(FONT_PATH, RAM_TEXT_SIZE,   0, 250);

}

// Draw Pixel to Window
void draw_pixel(uint8_t x, uint8_t y, Color c) {
    DrawRectangle(x * PIXEL_SIZE,y * PIXEL_SIZE,PIXEL_SIZE,PIXEL_SIZE,c);
}

// Check if window is still open
bool display_is_open(void) {
    return !WindowShouldClose();
}

// Get Current Keypad Inputs
uint16_t get_keypad_inputs(void) {
    
    // Keys corresponding to 0-F inputs
    int key_dict[] = {KEY_X, KEY_ONE, KEY_TWO, KEY_THREE,
                    KEY_Q, KEY_W, KEY_E, KEY_A,
                    KEY_S, KEY_D, KEY_Z, KEY_C,
                    KEY_FOUR, KEY_R, KEY_F, KEY_V};

    // Read keys from bottom up
    uint16_t keypad = 0;
    for (int k = 15; k >= 0; k--) {
        keypad = keypad << 1;
        keypad = keypad | IsKeyDown(key_dict[k]);
    }

    return keypad;
}

// Return if spacebar is pressed
bool is_space_pressed(void) {
    return IsKeyPressed(KEY_SPACE);
}

// Return if p is pressed
bool is_p_pressed(void) {
    return IsKeyPressed(KEY_P);
}

// Return if r is pressed
bool is_enter_pressed(void) {
    return IsKeyPressed(KEY_ENTER);
}

// Update Display Window
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

    // Draw Debug Information
    Vector2 cursor = {DEBUG_X + DEBUG_TEXT_SIZE/5, DEBUG_Y};
    int size = DEBUG_TEXT_SIZE;
    int spacing = 0;
    DrawRectangle(DEBUG_X, DEBUG_Y, DEBUG_WIDTH, DEBUG_HEIGHT, BLUE);

    // First Draw Special Registers
    DrawTextEx(db_font, TextFormat("Chip-8 Debug Information"),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    DrawTextEx(db_font, TextFormat("Special", chip->pc),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    DrawTextEx(db_font, TextFormat("PC: %d", chip->pc),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    DrawTextEx(db_font, TextFormat("I:  %d", chip->i),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    DrawTextEx(db_font, TextFormat("SP: %d", chip->sp),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    DrawTextEx(db_font, TextFormat("DT: %d", chip->delay),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    DrawTextEx(db_font, TextFormat("ST: %d", chip->sound),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    
    // Next Draw General Registers
    cursor.x += 6 * size;
    cursor.y = DEBUG_Y + size;
    DrawTextEx(db_font, TextFormat("General", chip->pc),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    for (uint8_t i = 0; i <= 0xf; i++) {
        DrawTextEx(db_font, TextFormat("[v%x]: %d", i, chip->reg[i]),
                    cursor, size, spacing, WHITE);
        cursor.y += size;

    }

    // Next Draw Stack
    cursor.x += 6 * size;
    cursor.y = DEBUG_Y + size;
    DrawTextEx(db_font, TextFormat("Stack", chip->pc),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    for (uint8_t i = 0; i <= 0xf; i++) {
        DrawTextEx(db_font, TextFormat(" [%x]: %d", i, chip->stack[i]),
                    cursor, size, spacing, WHITE);
        if (chip->sp == i)
            DrawTextEx(db_font, TextFormat(">", i, chip->stack[i]),
                    cursor, size, spacing, WHITE);
        cursor.y += size;

    }

    // Draw Inputs
    uint8_t keypad[] = {0x1, 0x2, 0x3, 0xc,
                        0x4, 0x5, 0x6, 0xd,
                        0x7, 0x8, 0x9, 0xe,
                        0xa, 0x0, 0xb, 0xf};
    int keypad_left = cursor.x + 8 * size;
    cursor.x = keypad_left;
    cursor.y = DEBUG_Y + size;
    DrawTextEx(db_font, TextFormat("Inputs"),
                cursor, size, spacing, WHITE);
    Vector2 keysize = {DEBUG_KEY_SIZE - 1, DEBUG_KEY_SIZE - 1};
    cursor.y += size;
    for (uint8_t i = 0; i <= 0xf; i++) {

        uint8_t key = keypad[i];

        Color key_col = WHITE;
        Color txt_col = BLUE;
        if ((chip->keypad >> key) & 1) {
            key_col = BLUE;
            txt_col = WHITE;
        }

        DrawRectangleV(cursor, keysize, key_col);
        DrawTextEx(db_font, TextFormat("%x", key),
                    cursor, size, spacing, txt_col);
        cursor.x += DEBUG_KEY_SIZE;

        if (i % 4 == 3) {
            cursor.y += DEBUG_KEY_SIZE;
            cursor.x = keypad_left;
        }

    }
   
    // Draw RAM contents
    cursor.x = RAM_X + RAM_TEXT_SIZE/4;
    cursor.y = RAM_Y + RAM_TEXT_SIZE/4;
    size = RAM_TEXT_SIZE;
    DrawRectangle(RAM_X, RAM_Y, RAM_WIDTH, RAM_HEIGHT, BLUE);
    DrawTextEx(ram_font, TextFormat("RAM", chip->pc),
                cursor, size, spacing, WHITE);
    cursor.y += size;
    for (uint8_t j = 0; j < 64; j++) {
        DrawTextEx(ram_font, TextFormat("%03x: ", j * 64),
                    cursor, size, spacing, WHITE);
        cursor.x += (size * .5) * 6;
        for (uint8_t i = 0; i < 64; i++) {
            Color c = WHITE;
            if (chip->pc == j * 64 + i) c = RED;
            DrawTextEx(ram_font, TextFormat("%02x ", chip->ram[64 * j + i]),
                        cursor, size, spacing, c);
            cursor.x += (size * .5) * 3;
        }
        cursor.x = RAM_X + RAM_TEXT_SIZE/4;
        cursor.y += size;
    }

    EndDrawing();
}

// Close Window
void end_display(void) {
    CloseWindow();
}
