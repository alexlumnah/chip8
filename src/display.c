#include <curses.h>

#include "chip8.h"
#include "vm.h"

WINDOW* display;
WINDOW* monitor;
void init_display(void) {

    // Initialize curses library
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    timeout(0); // Dont wait block when waiting for input

    // Create windows for displaying text
    display = newpad(DISPLAY_HEIGHT, DISPLAY_WIDTH);
    monitor = newpad(DISPLAY_HEIGHT, COLS - DISPLAY_WIDTH);

    // Initialize all pads
    refresh();
}

void update_display(Chip8* chip) {

    VM* vm = chip->vm;

    // First clear display
    wclear(display);

    // Now draw VM video memory
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            if (vm->display[y*DISPLAY_WIDTH + x] == 1)
                mvwaddch(display,y,x,'X');
            else mvwaddch(display,y,x,'.');
        }
    }
    prefresh(display, 0, 0, 0, 0, DISPLAY_HEIGHT, DISPLAY_WIDTH);
}

void end_display(void) {
    endwin();
}
