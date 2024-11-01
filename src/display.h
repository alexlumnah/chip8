#ifndef DISPLAY_H
#define DISPLAY_H

#include "chip8.h"

#define PIXEL_SIZE     (10)
#define DISPLAY_WIDTH  (PIXEL_SIZE * VID_WIDTH)
#define DISPLAY_HEIGHT (PIXEL_SIZE * VID_HEIGHT)

void init_display(void);
void update_display(Chip8* chip);
void end_display(void);

bool display_is_open(void);
uint16_t get_keypad_inputs(void);

#endif  // DISPLAY_H

