#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <libguile.h>

#include "scheme_interface.h"
#include "draw.h"
#include "main.h"

static void *SDL_main_loop () {
	while (1) {
		SDL_Event e;
		if (SDL_PollEvent (&e)) {
			if (e.type == SDL_QUIT)
				break;
		}

//		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	return NULL;
}

int main (int argc, char* argv[]) {

	int width  = DEFAULT_WIDTH;
	int height = DEFAULT_HEIGHT;

	int opt;

	while ((opt = getopt(argc, argv, "w:h:")) != -1) {
		switch(opt) {
			case 'w':
				width = atoi(optarg);
				break;
			case 'h':
				height = atoi(optarg);
				break;
			case '?':
				fprintf(stderr, 
					"Option -%c requires an argument.\n", 
					optopt);
				return 1;
			default:
				abort();
		}
	}

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
		printf("error: %s\n", SDL_GetError());
		return 1;
	}

	scm_with_guile(&register_functions, NULL);
	scm_shell(argc, argv);

	SDL_main_loop();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);


	atexit(SDL_Quit);

	return 0;
}
