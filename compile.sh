gcc -I./include/ -c ./src/chip8/emu.c -o chip8emu -Wall -Wextra -Werror -pedantic
gcc -I./include/ -c ./src/chip8/gui.c -o chip8gui -Wall -Wextra -Werror -pedantic
gcc -I./include/ ./src/main.c chip8emu chip8gui -o chip8_em -Wall -Wextra -Werror -pedantic -lSDL2
rm chip8emu chip8gui
