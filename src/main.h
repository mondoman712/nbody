#ifndef _main_h
#define _main_h

SDL_Window *window;
SDL_Renderer *renderer;

struct vector {
	int x;
	int y;
};

struct body {
	struct vector pos;
	struct vector vel;
	double rad;
};

static void *SDL_main_loop();

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

#endif // _main_h
