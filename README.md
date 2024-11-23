# Chip-8 Emulator

Simple emulator for chip-8, with debugger view.

Hex keypad inputs are mapped to the following keys:
* [1] [2] [3] [4]
* [q] [w] [e] [r]
* [a] [s] [d] [f]
* [z] [x] [c] [v]

Emulator Control
* [p] - Pause Execution
* [space] - Step One Instruction
* [enter] - Resume Execution

Compile with make. Run using `./main <path_to_rom>`

## Requirements:
* raylib for UI. Link using RAYFLAGS in MakeFile.
* Update .ttf font file path at top of `./src/display.c`

## Tetris Example:
![image](https://github.com/user-attachments/assets/76290b45-0e2f-428b-8936-d5737e7e984e)
