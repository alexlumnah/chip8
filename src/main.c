#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"

int main(int argc, char** argv) {

    Chip8* chip = calloc(1, sizeof(Chip8));
    init_chip8(chip);

    if (argc > 1) {
        load_rom(chip, argv[1]);
    } else {
        printf("Usage: chip8 <path_to_rom>");
    }

    run(chip);

}
