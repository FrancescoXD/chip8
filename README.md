# chip8
A simple CHIP8 emulator (maybe well written)

# Info
This document should contains all the technical reference, PR for any fix. Reference can be found [here](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM). Another well written reference [here](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/).

## What's Chip-8?
Chip-8 is a simple programming language for old pcs. They had 4K of RAM and used 16-key hexadecimal keypad for input. The interpreter took only 512 bytes of memory.
A new version exists and it's called Chip-48.

## Memory
It's capable to access up to 4KB (4,096 bytes) of RAM. from 0x000 (0) to 0xFFF (4095).  
From 0x000 to 0x1FF (512 bytes) the memory is reserved to the interpreter.  
After the 0x200 (512) the programs begin.

#### Memory Map:
```
+---------------+= 0xFFF (4095) End of Chip-8 RAM
|               |
|               |
|               |
|               |
|               |
| 0x200 to 0xFFF|
|     Chip-8    |
| Program / Data|
|     Space     |
|               |
|               |
|               |
+- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
|               |
|               |
|               |
+---------------+= 0x200 (512) Start of most Chip-8 programs
| 0x000 to 0x1FF|
| Reserved for  |
|  interpreter  |
+---------------+= 0x000 (0) Start of Chip-8 RAM
```

## Registers
Chip-8 has 16 general purpose 8-bit registers. From **_V0_** to **_VF_**. There is also a 16-bit register called **_I_** (_index_) to store memory address so only the lowest 12 bits are used.
**VF** register should not be used by programs.  
There are 2 special registers (8-bit each one), for the delay and sound timer. When these register are != 0 they should be decremented at a rate of *60Hz*.  
There are also some "pseudo-registers" which are not accessable from Chip-8 programs.  
The **program counter** (*PC*) should be 16-bit, and is used to store the currently executing address.  
The **stack pointer** (*SP*) can be 8-bit, it is used to point to the topmost level of the stack.  
The **stack** is an array of 16 16-bit values, used to store the address that the interpreter shoud return to when finished with a subroutine. Chip-8 allows for up to 16 levels of nested subroutines.  

## Keyboard
The keyboard layout is the following:
```
Keypad              Keyboard
+-+-+-+-+           +-+-+-+-+
|1|2|3|C|           |1|2|3|4|
+-+-+-+-+           +-+-+-+-+
|4|5|6|D| mapped to |Q|W|E|R|
+-+-+-+-+ ========> +-+-+-+-+
|7|8|9|E|           |A|S|D|F|
+-+-+-+-+           +-+-+-+-+
|A|0|B|F|           |Z|X|C|V|
+-+-+-+-+           +-+-+-+-+
```

## Display
It's a 64x32 pixel display.  
```
(0,0)	(63,0)
(0,31)	(63,31)
```
Chip-8 draws graphics on screen through the use of **sprites**. A sprite is a group of bytes which are a binary representation of the desired picture. Chip-8 sprites may be up to 15 bytes, for a possible sprite size of 8x15.

## Timers and Sound
The **delay timer** is active whenever the delay timer register (*DT*) is non-zero. This timer does nothing more than subtract 1 from the value of DT at a rate of 60Hz. When DT reaches 0, it deactivates.  
The **sound timer** is active whenever the sound timer register (*STé) is non-zero. This timer also decrements at a rate of 60Hz, however, as long as ST's value is greater than zero, the Chip-8 buzzer will sound. The sound is decided by the author.
