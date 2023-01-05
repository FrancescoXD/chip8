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
		chip->memory[FONTSET_START_ADDRESS + i] = fontset[i];
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
	fprintf(stdout, "Rom size: %luB\n", romsize);
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
	chip->opcode |= chip->memory[chip->pc + 1];
}

void chip8_execute(Chip8_t *chip) {
	chip8_fetch_opcode(chip);
	//fprintf(stdout, "pc: 0x%04X\n", chip->pc);
	//fprintf(stdout, "opcode: 0x%04X\n", chip->opcode);
	chip->pc += 0x2;

	switch (chip->opcode & 0xF000u) {
		case 0x0000:
			switch (chip->opcode & 0x00FFu) {
				case 0x00E0: chip8_00e0(chip); break;
				case 0x00EE: chip8_00ee(chip); break;
			}
			break;
		case 0x1000: chip8_1nnn(chip); break;
		case 0x2000: chip8_2nnn(chip); break;
		case 0x3000: chip8_3xnn(chip); break;
		case 0x4000: chip8_4xnn(chip); break;
		case 0x5000: chip8_5xy0(chip); break;
		case 0x6000: chip8_6xnn(chip); break;
		case 0x7000: chip8_7xnn(chip); break;
		case 0x8000:
			switch (chip->opcode & 0x000Fu) {
				case 0x0000: chip8_8xy0(chip); break;
				case 0x0001: chip8_8xy1(chip); break;
				case 0x0002: chip8_8xy2(chip); break;
				case 0x0003: chip8_8xy3(chip); break;
				case 0x0004: chip8_8xy4(chip); break;
				case 0x0005: chip8_8xy5(chip); break;
				case 0x0006: chip8_8xy6(chip); break;
				case 0x0007: chip8_8xy7(chip); break;
				case 0x000E: chip8_8xye(chip); break;
				default:
					fprintf(stderr, "Invalid OPCODE: %X\n", chip->opcode);
					break;
			}
			break;
		case 0x9000: chip8_9xy0(chip); break;
		case 0xA000: chip8_annn(chip); break;
		case 0xB000: chip8_bxnn(chip); break;
		case 0xC000: chip8_cxnn(chip); break;
		case 0xD000: chip8_dxyn(chip); break;
		case 0xE000:
			switch (chip->opcode & 0x00FFu) {
				case 0x00A1: chip8_exa1(chip); break;
				case 0x009E: chip8_ex9e(chip); break;
				default:
					fprintf(stderr, "Invalid OPCODE: %X\n", chip->opcode);
					break;
			}
			break;
		case 0xF000:
			switch (chip->opcode & 0x00FFu) {
				case 0x0007: chip8_fx07(chip); break;
				case 0x000A: chip8_fx0a(chip); break;
				case 0x0015: chip8_fx15(chip); break;
				case 0x0018: chip8_fx18(chip); break;
				case 0x0029: chip8_fx29(chip); break;
				case 0x001E: chip8_fx1e(chip); break;
				case 0x0033: chip8_fx33(chip); break;
				case 0x0055: chip8_fx55(chip); break;
				case 0x0065: chip8_fx65(chip); break;
				default:
					fprintf(stderr, "Invalid OPCODE: %X\n", chip->opcode);
					break;
			}
			break;
		default:
			fprintf(stderr, "Invalid OPCODE: %X\n", chip->opcode);
			break;
	}
	if (chip->delay_timer > 0) {
		chip->delay_timer--;
	}

	if (chip->sound_timer > 0) {
		// BEEP
		chip->sound_timer--;
	}
}

/**
 * OPCODE OPERATIONS HERE
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
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
	uint8_t x = chip->registers[Vx] & (VIDEO_WIDTH - 1);
	uint8_t y = chip->registers[Vy] & (VIDEO_HEIGHT - 1);
	chip->registers[0xF] = 0x0;

	uint8_t rows = chip->opcode & 0x000Fu;

	for (size_t row = 0; row < rows; ++row) {
		uint8_t sprite_data = chip->memory[chip->index + row];
		for (size_t col = 0; col < 8; ++col) {
			uint8_t pixel = sprite_data >> 7;
			sprite_data <<= 1;
			size_t pos = (y + row) * VIDEO_WIDTH + (x + col);
			if (pixel && (chip->video[pos] == 0xFFFFFFFF)) {
				//UNDRAW POINT
				chip->video[pos] = 0x0;
				chip->registers[0xF] = 0x1;
			} else if (pixel && !chip->video[pos]) {
				//DRAW POINT
				chip->video[pos] = 0xFFFFFFFF;
			}
		}
	}
}

/**
 * EX9E and EXA1: Skip if key
*/
void chip8_ex9e(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t key = chip->registers[Vx];
	if (chip->keypad[key]) {
		chip->pc += 2;
	}
}

void chip8_exa1(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t key = chip->registers[Vx];
	if (!chip->keypad[key]) {
		chip->pc += 2;
	}
}

/**
 * FX07, FX15 and FX18: Timers
 * 
 * TODO:
 * Timer and Sound needs to be decremented...
*/
void chip8_fx07(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	chip->registers[Vx] = chip->delay_timer;
}

void chip8_fx15(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	chip->delay_timer = chip->registers[Vx];
}

void chip8_fx18(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	chip->sound_timer = chip->registers[Vx];
}

/**
 * FX1E: Add to index
*/
void chip8_fx1e(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint32_t result = chip->index + chip->registers[Vx];
	if (result > 0xFFFF) {
		chip->registers[0xF] = 0x1;
	}
	chip->index += chip->registers[Vx];
}

/**
 * FX0A: Get key
*/
uint8_t _find_key(uint8_t registers[]) {
	for (size_t i = 0; i <= 0xF; ++i) {
		if (registers[i]) {
			return i;
		}
	}
	return -1;
}

void chip8_fx0a(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8;
	int val = _find_key(chip->registers);
	if (val == -1) chip->pc -= 2;
	else chip->registers[Vx] = val;
}

/**
 * FX29: Font character
*/
void chip8_fx29(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t digit = chip->registers[Vx];
	chip->index = FONTSET_START_ADDRESS + (5 * digit);
}

/**
 * FX33: Binary-coded decimal conversion
*/
void chip8_fx33(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t value = chip->registers[Vx];

	chip->memory[chip->index + 2] = value % 10;
	value /= 10;
	chip->memory[chip->index + 1] = value % 10;
	value /= 10;
	chip->memory[chip->index] = value % 10;
}

/**
 * FX55 and FX65: Store and load memory
 * Ambiguous instruction!
*/
void chip8_fx55(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	for (size_t i = 0; i <= Vx; ++i) {
		chip->memory[chip->index + i] = chip->registers[i];
	}
}

void chip8_fx65(Chip8_t *chip) {
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	for (size_t i = 0; i <= Vx; ++i) {
		chip->registers[i] = chip->memory[chip->index + i];
	}
}
