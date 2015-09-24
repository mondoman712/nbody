#include "defs.h"

void set_pixel (SDL_Surface *surface, int x, int y, Uint32 pixel) {
	Uint8 *target_pixel = (Uint8 *)(*surface).pixels 
		+ y * (*surface).pitch
		+ x * 4;
	*(Uint32 *)target_pixel = pixel;
}

void draw_circle(SDL_Surface *surface, int n_cx, int n_cy, int radius, 
		Uint32 pixel) {
	double error = (double)-radius;
	double x = (double)radius - 0.5;
	double y = (double)0.5;
	double cx = n_cx - 0.5;
	double cy = n_cy - 0.5;

	while (x >= y) {

		set_pixel (surface, (int)(cx + x), (int)(cy + y), pixel);
		set_pixel (surface, (int)(cx + y), (int)(cy + x), pixel);

		if (x != 0) {
		set_pixel (surface, (int)(cx - x), (int)(cy + y), pixel);
		set_pixel (surface, (int)(cx + y), (int)(cy - x), pixel);
		}

		if (y != 0) {
		set_pixel (surface, (int)(cx + x), (int)(cy - y), pixel);
		set_pixel (surface, (int)(cx - y), (int)(cy + x), pixel);
		}

		if (x != 0 && y != 0) {
		set_pixel (surface, (int)(cx - x), (int)(cy - y), pixel);
		set_pixel (surface, (int)(cx - y), (int)(cy - x), pixel);
		}

		error += y;
		++y;
		error += y;

		if (error >= 0) {
			--x;
			error -= x;
			error -= x;
		}
	}
}


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
