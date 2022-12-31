#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include <ncurses.h>

#define STACK_SIZE 16
#define REGISTERS_COUNT 16
#define KEYPAD_COUNT 16
#define VIDEO_HEIGHT 32
#define VIDEO_WIDTH 64
#define MEM_SIZE 4096
#define FONTSET_SIZE 80

#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50

typedef struct {
    // Chip-8 uses up to 4KB of RAM
    uint8_t memory[MEM_SIZE];
    // Program Counter (PC) is a special 16-bit register
    // It starts from 0x200
    // An OPCODE is made by 2 bytes, so we need pc and pc+1
    uint16_t pc;
    // Stack Pointer (SP) points to the top of the stack
    uint8_t sp;
    // Index register used to store address in operations
    uint16_t index;
    // Stack is used to store the return address when a subroutines finishes
    uint16_t stack[STACK_SIZE];
    // 16 (8-bit) general purpose registers from V0 to VF
    // VF is used as a flag to hold info about the result of the operations
    uint8_t registers[REGISTERS_COUNT];
    // 2 special registers
    uint8_t delay_timer;
    uint8_t sound_timer;
    // The Keyboard (Keypad) layout
    uint8_t keypad[KEYPAD_COUNT];
    uint8_t video[VIDEO_WIDTH * VIDEO_HEIGHT];
    // The read OPCODE
    uint16_t opcode;
} Chip8_t;

Chip8_t chip8_init();
bool chip8_load_rom(Chip8_t *chip, const char *filename);
void chip8_print_resmem(Chip8_t *chip);
void chip8_print_promem(Chip8_t *chip);
void chip8_load_fontset(Chip8_t *chip);
void chip8_fetch_opcode(Chip8_t *chip);
void chip8_execute(Chip8_t *chip);

void chip8_dxyn(Chip8_t *chip);
void chip8_00e0(Chip8_t *chip);
void chip8_00ee(Chip8_t *chip);
void chip8_1nnn(Chip8_t *chip);
void chip8_2nnn(Chip8_t *chip);
void chip8_3xnn(Chip8_t *chip);
void chip8_4xnn(Chip8_t *chip);
void chip8_5xy0(Chip8_t *chip);
void chip8_9xy0(Chip8_t *chip);
void chip8_6xnn(Chip8_t *chip);
void chip8_7xnn(Chip8_t *chip);
void chip8_8xy0(Chip8_t *chip);
void chip8_8xy1(Chip8_t *chip);
void chip8_8xy2(Chip8_t *chip);
void chip8_8xy3(Chip8_t *chip);
void chip8_8xy4(Chip8_t *chip);
void chip8_8xy5(Chip8_t *chip);
void chip8_8xy7(Chip8_t *chip);
void chip8_8xy6(Chip8_t *chip);
void chip8_8xye(Chip8_t *chip);
void chip8_annn(Chip8_t *chip);
void chip8_bxnn(Chip8_t *chip);
void chip8_cxnn(Chip8_t *chip);
void chip8_dxyn(Chip8_t *chip);

#endif