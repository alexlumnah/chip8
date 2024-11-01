#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"
#include "vm.h"
#include "display.h"

void init_chip8(Chip8* chip) {

    // Create VM
    chip->vm = calloc(1, sizeof(VM));
    init_vm(chip->vm);

    chip->clock_f = 60.0;   // Clock at 60Hz
    chip->cycle_f = 700;    // Execute instructions at 700Hz

    chip->clocks = 0;
    chip->cycles = 0;
}

void run(Chip8* chip) {

    init_display();

    clock_t start = clock();
    while (true) {
        float delta_t = (clock() - start) / (float)CLOCKS_PER_SEC;
        
        if ( chip->cycles < delta_t * chip->cycle_f) {
            cycle(chip->vm);
        }

        if ( chip->clocks < delta_t * chip->clock_f) {
            send_clock(chip->vm);
            update_display(chip);
        }

    }

    end_display();
    printf("fin.\n");

}
