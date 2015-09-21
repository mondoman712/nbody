#include "defs.h"

int makesdlwindow(int width) {
	int go = 1;
	SDL_Event event;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_WM_SetCaption("test", NULL);
	SDL_SetVideoMode(width, 768, 32, SDL_HWSURFACE);
	while (go == 1){
		SDL_WaitEvent(&event);
		if (event.type == SDL_QUIT)
			go = 0;
	}
	SDL_Quit();
	return 0;
}
