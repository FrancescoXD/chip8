#include <chip8.h>

int main(int argc, char **argv) {
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
    chip8_print_promem(&chip);

    //initscr();
    int i = 0;
    while (i < 3) {
        chip8_execute(&chip);
        i++;
        //refresh();
        //getch();
    }
    //endwin();

    exit(EXIT_SUCCESS);
}
