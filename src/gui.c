#include <gui.h>

void gui_send_error(const char *err) {
	fprintf(stderr, "%s", err);
	exit(EXIT_FAILURE);
}

Gui_t chip8_gui_init(const char *title, int videowidth, int videoheight, int scalevideo) {
	Gui_t gui;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		gui_send_error("Could not initialize SDL!\n");
	}

	SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, videowidth * scalevideo, videoheight * scalevideo, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		gui_send_error("Could not create SDL window!\n");
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		gui_send_error("Could not create SDL renderer!\n");
	}

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, videowidth, videoheight);

	gui.window = window;
	gui.renderer = renderer;
	gui.texture = texture;

	return gui;
}

bool chip8_gui_handle_input(uint8_t *keypad) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_1: keypad[0x0] = 1; break;
					case SDLK_2: keypad[0x1] = 1; break;
					case SDLK_3: keypad[0x3] = 1; break;
					case SDLK_4: keypad[0xC] = 1; break;
					case SDLK_q: keypad[0x4] = 1; break;
					case SDLK_w: keypad[0x5] = 1; break;
					case SDLK_e: keypad[0x6] = 1; break;
					case SDLK_r: keypad[0xD] = 1; break;
					case SDLK_a: keypad[0x7] = 1; break;
					case SDLK_s: keypad[0x8] = 1; break;
					case SDLK_d: keypad[0x9] = 1; break;
					case SDLK_f: keypad[0xE] = 1; break;
					case SDLK_z: keypad[0xA] = 1; break;
					case SDLK_x: keypad[0x0] = 1; break;
					case SDLK_c: keypad[0xB] = 1; break;
					case SDLK_v: keypad[0xF] = 1; break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: return true; break;
					case SDLK_1: keypad[0x0] = 0; break;
					case SDLK_2: keypad[0x1] = 0; break;
					case SDLK_3: keypad[0x3] = 0; break;
					case SDLK_4: keypad[0xC] = 0; break;
					case SDLK_q: keypad[0x4] = 0; break;
					case SDLK_w: keypad[0x5] = 0; break;
					case SDLK_e: keypad[0x6] = 0; break;
					case SDLK_r: keypad[0xD] = 0; break;
					case SDLK_a: keypad[0x7] = 0; break;
					case SDLK_s: keypad[0x8] = 0; break;
					case SDLK_d: keypad[0x9] = 0; break;
					case SDLK_f: keypad[0xE] = 0; break;
					case SDLK_z: keypad[0xA] = 0; break;
					case SDLK_x: keypad[0x0] = 0; break;
					case SDLK_c: keypad[0xB] = 0; break;
					case SDLK_v: keypad[0xF] = 0; break;
				}
				break;
		}
	}

	return false;
}

void chip8_gui_update_screen(void const *buffer, int pitch, Gui_t *gui) {
	SDL_UpdateTexture(gui->texture, NULL, buffer, pitch);
	SDL_RenderClear(gui->renderer);
	SDL_RenderCopy(gui->renderer, gui->texture, NULL, NULL);
	SDL_RenderPresent(gui->renderer);
}

void chip8_gui_quit(Gui_t *gui) {
	SDL_DestroyRenderer(gui->renderer);
	SDL_DestroyWindow(gui->window);
	SDL_Quit();
}
