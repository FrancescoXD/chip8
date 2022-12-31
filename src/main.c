#include <chip8.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage:\n%s <ROM>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Starting chip8 emulator...\n");

	Chip8_t chip = chip8_init();
	const char *filename = argv[1];

	fprintf(stdout, "Loading %s...\n", filename);
	if(!chip8_load_rom(&chip, filename)) {
		fprintf(stderr, "Cannot load %s!\n", filename);
		chip8_print_resmem(&chip);
		chip8_print_promem(&chip);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "ROM %s loaded!\n", filename);
	//chip8_print_promem(&chip);

	initscr();
	resize_term(VIDEO_HEIGHT, VIDEO_WIDTH);
	
	mvprintw(0, 0, "CHIP-8 Emulator");
	mvprintw(1, 0, "Loading %s...", filename);

	while (1) {
		chip8_execute(&chip);
		refresh();
	}
	
	//getch();
	endwin();

	exit(EXIT_SUCCESS);
}
