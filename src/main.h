#ifndef _main_h
#define _main_h

SDL_Window *window;
SDL_Renderer *renderer;

struct body {
	Sint16 x;
	Sint16 y;
	Sint16 rad;
};

void *SDL_main_loop();

#endif // _main_h
