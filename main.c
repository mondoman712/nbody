#include "defs.h"

int main (int argc, char* argv[]) {

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *bitmapTex;
	SDL_Surface *bitmapSurface;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
			"nbody",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			1024,
			768,
			0
			);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	bitmapSurface = SDL_LoadBMP("hello.bmp");
	bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
	SDL_FreeSurface(bitmapSurface);

	if (window == NULL) {
		printf("does broke: %s\n", SDL_GetError());
		return 1;
	}

	while (1) {
		SDL_Event e;
		if (SDL_PollEvent (&e)) {
			if (e.type == SDL_QUIT)
				break;
		}

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	//SDL_Delay(3000);

	SDL_DestroyTexture(bitmapTex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	atexit(SDL_Quit);

	return 0;
}
