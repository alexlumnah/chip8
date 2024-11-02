#ifndef DISPLAY_H
#define DISPLAY_H

#include "chip8.h"

// Video Display Panel
#define PIXEL_SIZE      (10)
#define DISPLAY_WIDTH   (PIXEL_SIZE * VID_WIDTH)
#define DISPLAY_HEIGHT  (PIXEL_SIZE * VID_HEIGHT)

// Debug Panel
#define DEBUG_TEXT_SIZE (17)
#define DEBUG_WIDTH     (56 * PIXEL_SIZE)
#define DEBUG_HEIGHT    (DISPLAY_HEIGHT)
#define DEBUG_X         (DISPLAY_WIDTH)
#define DEBUG_Y         (0)
#define DEBUG_KEY_SIZE  (40)

// RAM Panel
#define RAM_TEXT_SIZE   (12)
#define RAM_WIDTH       (DISPLAY_WIDTH + DEBUG_WIDTH)
#define RAM_HEIGHT      (2.5 * DISPLAY_HEIGHT)
#define RAM_X           (0)
#define RAM_Y           (DISPLAY_HEIGHT)

// Window
#define WINDOW_WIDTH    (DISPLAY_WIDTH + DEBUG_WIDTH)
#define WINDOW_HEIGHT   (DISPLAY_HEIGHT + RAM_HEIGHT)

void init_display(void);
void update_display(Chip8* chip);
void end_display(void);

bool display_is_open(void);
uint16_t get_keypad_inputs(void);
bool is_space_pressed(void);
bool is_p_pressed(void);
bool is_enter_pressed(void);

#endif  // DISPLAY_H

