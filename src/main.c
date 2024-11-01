#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"
#include "vm.h"

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    Chip8 chip;
    init_chip8(&chip);

    //load_rom(chip.vm, "/Users/lumnah/Documents/c/chip-8/chip8-test-suite/bin/1-chip8-logo.ch8");
    //load_rom(chip.vm, "/Users/lumnah/Documents/c/chip-8/chip8-test-suite/bin/2-ibm-logo.ch8");
    //load_rom(chip.vm, "/Users/lumnah/Documents/c/chip-8/chip8-test-suite/bin/3-corax+.ch8");
    //load_rom(chip.vm, "/Users/lumnah/Documents/c/chip-8/chip8-test-suite/bin/4-flags.ch8");
    load_rom(chip.vm, "/Users/lumnah/Documents/c/chip-8/chip8-test-suite/bin/5-quirks.ch8");

    run(&chip);

    //for (int i = 0; i < 100; i++) {
    //    printf("%d: ",i);
    //    cycle(chip.vm);
    //    dump_state(chip.vm);
    //    dump_display(chip.vm);
    //    //getchar();
    //}

}
