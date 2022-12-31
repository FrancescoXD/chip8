#include <chip8.h>

/**
 * Init the chip's structure
*/
Chip8_t chip8_init() {
	srand(time(NULL));

	Chip8_t chip8;
	// Fill the memory with 0s
	memset(&chip8, 0, sizeof(Chip8_t));
	// Set the PC to the START_ADDRESS (0x200)
	chip8.pc = START_ADDRESS;
	// Load the fonts
	chip8_load_fontset(&chip8);

	return chip8;
}

/**
 * Load the fontset in the memory
*/
void chip8_load_fontset(Chip8_t *chip) {
	uint8_t fontset[FONTSET_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (size_t i = 0; i < FONTSET_SIZE; ++i) {
		chip->memory[i] = fontset[i];
	}
}

/**
 * Load the ROM in the memory (starting from 0x200)
*/
bool chip8_load_rom(Chip8_t *chip, const char *filename) {
	int fd = open(filename, O_RDONLY, 0);
	if (fd < 0) return false;
	ssize_t romsize = read(fd, &chip->memory[START_ADDRESS], MEM_SIZE - START_ADDRESS);
	if (romsize < 0) return false;
	fprintf(stdout, "Rom size: %lu\n", romsize);
	return true;
}

/**
 * Print chip's memory in hex
*/
void chip8_print_resmem(Chip8_t *chip) {
	fprintf(stdout, "Printing reserved memory...\n");
	for (size_t i = 0x0; i < START_ADDRESS; i += 2) {
		fprintf(stdout, "%X%X ", chip->memory[i], chip->memory[i+1]);
	}
	fprintf(stdout, "\n");
}

void chip8_print_promem(Chip8_t *chip) {
	fprintf(stdout, "Printing programs memory...\n");
	for (size_t i = START_ADDRESS; i < MEM_SIZE; i += 2) {
		fprintf(stdout, "%X%X ", chip->memory[i], chip->memory[i+1]);
	}
	fprintf(stdout, "\n");
}

void chip8_fetch_opcode(Chip8_t *chip) {
	chip->opcode = chip->memory[chip->pc];
	chip->opcode <<= 8u;
	fprintf(stdout, "opcode pc: %04X\n", chip->opcode);
	chip->opcode |= chip->memory[chip->pc + 1];
	fprintf(stdout, "opcode pc + pc+1: %04X\n", chip->opcode);
}

void chip8_execute(Chip8_t *chip) {
	fprintf(stdout, "pc: %04X\n", chip->pc);
	chip8_fetch_opcode(chip);
	chip->pc += 0x2;

	switch (chip->opcode & 0xF000u) {
		case 0x0000:
			switch (chip->opcode & 0x00FFu) {
				case 0x00E0: chip8_00e0(chip); puts("CALLED cls 00e0"); break;
				case 0x00EE: chip8_00ee(chip); puts("CALLED ret 00ee"); break;
			}
			break;
		case 0x1000: chip8_1nnn(chip); puts("CALLED 1nnn"); break;
		case 0x6000: chip8_6xnn(chip); puts("CALLED 6xnn"); break;
		case 0x7000: chip8_7xnn(chip); puts("CALLED 7xnn"); break;
		case 0xA000: chip8_annn(chip); puts("CALLED annn"); break;
		case 0xD000: chip8_dxyn(chip); puts("CALLED dxyn"); break;
		default:
			fprintf(stderr, "Invalid OPCODE: %X\n", chip->opcode);
	}
}

/**
 * OPCODE OPERATIONS HERE
 * EXAMPLE:
 * 8xy0
 * Vx = (opcode & 0x0F00u) >> 8;
 * 8AB0
 * A = 1010
 * B = 1011
 * OPCODE:
 * 1000 1010 1011 0000
 * VX:
 * 0000 1111 0000 0000
 * RES:
 * 0000 0000 0000 1010
 * The same is for Vy but it shifts only 4 digits
*/

/**
 * CLS
 * Clear the video (screen)
*/
void chip8_00e0(Chip8_t *chip) {
	memset(chip->video, 0, sizeof(chip->video));
}

/**
 * JMP
 * Jump to an address
*/
void chip8_1nnn(Chip8_t *chip) {
	uint16_t address = chip->opcode & 0x0FFFu;
	chip->pc = address;
}

/**
 * SUBROUTINES
 * Push an address to the stack and call the subroutine
*/
void chip8_2nnn(Chip8_t *chip) {
	uint16_t address = chip->opcode & 0x0FFFu;
	chip->stack[chip->sp] = chip->pc;
	chip->sp++;
	chip->pc = address;
}

/**
 * RET
 * Return to the caller address
 * Move the stack pointer and set the new program counter
*/
void chip8_00ee(Chip8_t *chip) {
	chip->sp--;
	chip->pc = chip->stack[chip->sp];
}

/**
 * SKIP
*/
void chip8_3xnn(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;
	if (chip->registers[Vx] == byte) chip->pc += 2;
}

void chip8_4xnn(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;
	if (chip->registers[Vx] != byte) chip->pc += 2;
}

void chip8_5xy0(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	if (chip->registers[Vx] == chip->registers[Vy]) chip->pc += 2;
}

void chip8_9xy0(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	if (chip->registers[Vx] != chip->registers[Vy]) chip->pc += 2;
}

/**
 * SET
 * Set the register to a value
*/
void chip8_6xnn(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;
	chip->registers[Vx] = byte;
}

/**
 * ADD
 * Add a value to Vx
 * It should set the CARRY FLAG (VF), but we can skip
*/
void chip8_7xnn(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;
	chip->registers[Vx] += byte;
}

/**
 * LOGICAL AND ARITHMETIC OPERATIONS
*/

/**
 * 8XY0: Set
*/
void chip8_8xy0(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	chip->registers[Vx] = chip->registers[Vy];
}

/**
 * 8XY1: Binary OR
*/
void chip8_8xy1(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	chip->registers[Vx] = chip->registers[Vx] | chip->registers[Vy];
}

/**
 * 8XY2: Binary AND
*/
void chip8_8xy2(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	chip->registers[Vx] = chip->registers[Vx] & chip->registers[Vy];
}

/**
 * 8XY3: Logical XOR
*/
void chip8_8xy3(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	chip->registers[Vx] = chip->registers[Vx] ^ chip->registers[Vy];
}

/**
 * 8XY4: Add
 * Unlike 7XNN, this addition will affect the carry flag
*/
void chip8_8xy4(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	uint16_t carry = chip->registers[Vx] + chip->registers[Vy];
	if (carry > 0xFF) {
		chip->registers[0xF] = 0x1;
	} else {
		chip->registers[0xF] = 0x0;
	}
	chip->registers[Vx] = chip->registers[Vx] + chip->registers[Vy];
}

/**
 * 8XY5 and 8XY7: Subtract
 * 8XY5 sets VX to the result of VX - VY
 * 8XY7 sets VX to the result of VY - VX
*/
void chip8_8xy5(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	if (chip->registers[Vx] > chip->registers[Vy]) {
		chip->registers[0xF] = 0x1;
	} else {
		chip->registers[0xF] = 0x0;
	}
	chip->registers[Vx] = chip->registers[Vx] - chip->registers[Vy];
}

void chip8_8xy7(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	if (chip->registers[Vy] > chip->registers[Vx]) {
		chip->registers[0xF] = 0x1;
	} else {
		chip->registers[0xF] = 0x0;
	}
	chip->registers[Vy] = chip->registers[Vy] - chip->registers[Vx];
}

/**
 * 8XY6 and 8XYE: Shift
 * Ambiguous instruction!
*/
void chip8_8xy6(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	chip->registers[Vx] = chip->registers[Vy];
	chip->registers[0xF] = chip->registers[Vx] & 0x1;
	chip->registers[Vx] >>= 1;
}

void chip8_8xye(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	chip->registers[Vx] = chip->registers[Vy];
	chip->registers[0xF] = chip->registers[Vx] >> 7;
	chip->registers[Vx] <<= 1;
}

/**
 * ANNN: Set index
*/
void chip8_annn(Chip8_t *chip) {
	uint16_t value = chip->opcode & 0x0FFFu;
	chip->index = value;
}

/**
 * BNNN: Jump with offset
 * Ambiguous instruction!
*/
void chip8_bxnn(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8;
	uint16_t addr = chip->opcode & 0x00FF;
	chip->pc = addr + chip->registers[Vx];
}

/**
 * CXNN: Random
*/
void chip8_cxnn(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8;
	uint8_t nn = chip->opcode & 0x00FFu;
	chip->registers[Vx] = nn & (rand() % 255);
}

/**
 * DXYN: Display
*/
void chip8_dxyn(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4;
	uint8_t x = chip->registers[Vx] % VIDEO_WIDTH;
	uint8_t y = chip->registers[Vy] % VIDEO_HEIGHT;
	//fprintf(stdout, "DEBUG\nx: %d\ny: %d\n", x, y);
	chip->registers[0xF] = 0x0;

	uint8_t rows = chip->opcode & 0x000Fu;

	for (size_t row = 0; row < rows; ++row) {
		uint8_t sprite_data = chip->memory[chip->index + row];
		for (size_t col = 0; col < 8; ++col) {
			uint8_t pixel = sprite_data >> 7;
			sprite_data <<= 1;
			size_t pos = x * (VIDEO_WIDTH * VIDEO_HEIGHT) + y;
			if (sprite_data && chip->video[pos]) {
				chip->video[pos] = 0x0;
				chip->registers[0xF] = 0x1;
				mvprintw(x, y, "X");
			} else if (sprite_data && !chip->video[pos]) {
				chip->video[pos] = 0x1;
				mvprintw(x, y, "");
			}
			x++;
		}
		y++;
	}
}

/**
 * EX9E and EXA1: Skip if key
*/

/**
 * FX07, FX15 and FX18: Timers
*/

/**
 * FX1E: Add to index
*/

/**
 * FX0A: Get key
*/

/**
 * FX29: Font character
*/

/**
 * FX33: Binary-coded decimal conversion
*/

/**
 * FX55 and FX65: Store and load memory
*/
