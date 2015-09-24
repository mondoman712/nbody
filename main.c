#include "defs.h"

int main (int argc, char* argv[]) {

	static const int width = 1024;
	static const int height = 768;

	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
			"nbody",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			0
			);

	renderer = SDL_CreateRenderer(window, -1, 0);

	// set screen to black
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");


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

//		SDL_RenderClear(renderer);
		filledCircleRGBA(renderer, 512, 384, 20, 255, 255, 255, 255);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);


	atexit(SDL_Quit);

	return 0;
}
