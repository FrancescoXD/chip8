#ifndef __GUI_H__
#define __GUI_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Gui_t;

Gui_t chip8_gui_init(const char *title, int videowidth, int videoheight, int scalevideo);
void chip8_gui_update_screen(void const *buffer, int pitch, Gui_t *gui);
bool chip8_gui_handle_input(uint8_t *keypad);
void chip8_gui_quit(Gui_t *gui);

#endif
