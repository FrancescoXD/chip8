gcc -I./include/ -c ./src/chip8.c -o chip8
gcc -I./include/ ./src/main.c chip8 -o chip8_em -lncurses
rm chip8