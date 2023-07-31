#include <chip8.h>

int main(int argc, char **argv) {
	if (argc < 4) {
		fprintf(stderr, "Usage:\n%s <SCALE> <DELAY> <ROM>\n\n", argv[0]);
		fprintf(stderr, "Example:\n%s 10 1000 roms/IBM_Logo.ch8\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Starting Chip-8 emulator...\n");

	// Init the CHIP-8
	Chip8_t chip8 = chip8_init();
	uint8_t scale = atoi(argv[1]);
	int delay = atoi(argv[2]);
	const char *filename = argv[3];

	fprintf(stdout, "Loading %s...\n", filename);
	if (!chip8_load_rom(&chip8, filename)) {
		fprintf(stderr, "Cannot load %s!\n", filename);
		chip8_print_resmem(&chip8);
		chip8_print_promem(&chip8);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "ROM %s loaded!\n", filename);

	// Init the GUI
	Gui_t gui = chip8_gui_init("Chip-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, scale);
	bool quit = false;

	// Event loop
	while (!quit) {
		quit = chip8_gui_handle_input(&chip8.keypad);
		chip8_execute(&chip8);
		chip8_gui_update_screen(chip8.video, sizeof(chip8.video[0]) * VIDEO_WIDTH, &gui);
		usleep(delay);
	}
	fprintf(stdout, "Closing Chip-8 emulator...\n");

	chip8_gui_quit(&gui);

	exit(EXIT_SUCCESS);
}
