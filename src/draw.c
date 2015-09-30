#include "defs.h"

int draw_circle(SDL_Renderer *rend, Sint16 x, Sint16 y, Sint16 rad,
		Uint8 r, Uint8 g, Uint8 b, Uint8 a) {

	SDL_setRenderDrawColor(rend, r, g, b, a);

	if (rad < 0) {
		return -1;
	} else if (rad == 1) {
		SDL_RenderDrawPoint(rend, x, y);
		return 0;
	}

	SDL_RenderDrawLine(rend, x - rad, x + rad, y - rad, y + rad);

	return 1;
}
