gcc -I./include/ -c ./src/chip8.c -o chip8 -Wall -Wextra -Werror -pedantic
gcc -I./include/ -c ./src/gui.c -o gui -Wall -Wextra -Werror -pedantic
gcc -I./include/ ./src/main.c chip8 gui -o chip8_em -Wall -Wextra -Werror -pedantic -lSDL2
rm chip8 gui