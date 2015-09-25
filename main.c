#include "defs.h"

/*
void drawFromScheme(SDL_Renderer renderer) {

	struct circle {


	SCM funcPos;
	SCM valPos;
	struct circle {
		int x;
		int y;
		int rad;
		int r;
		int g;
		int b;
		int a;
	}


	scm_init_guile();

	scm_c_primitive_load("bodypos.scm");

	funcPos = scm_variable_ref(scm_c_lookup("bodypos"));
	valPos = scm_call_1(funcPos, scm_
*/

SDL_Window *window;
SDL_Renderer *renderer;

SCM draw_circle(int x, int y, int rad, int r, int g, int b, int a) {
	filledCicleRGBA(renderer, x, y, rad, r, g, b, a);
	return NULL;
}

static void* register_functions(void* data) {
	return NULL;
}

void *SDL_main_loop(){
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

	int width = 1024;
	int height = 768;

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

	pthread_t *tid_sdl;
	//pthread_t tid_guile;

	pthread_create(&tid_sdl, NULL, SDL_main_loop, NULL);

	scm_with_guile(&register_functions, NULL);
	scm_shell(argc, argv);

	pthread_join(&tid_sdl, NULL);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);


	atexit(SDL_Quit);

	return 0;
}
