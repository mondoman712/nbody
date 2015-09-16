#include "defs.h"

int sdl-window(int argc, char *argv[]) {
	int go = 1;
	SDL_Event event;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_WM_SetCaption("test", NULL);
	SDL_SetVideoMode(1024, 768, 32, SDL_HWSURFACE);
	while (go == 1){
		SDL_WaitEvent(&event);
		if (event.type == SDL_QUIT)
			go = 0;
	}
	SDL_Quit();
	return 0;
}
