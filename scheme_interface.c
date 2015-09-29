#include "defs.h"

void* register_functions(void* data) {
	scm_c_define_gsubr("draw-square", 7, 0, 0, &draw_square);
	return NULL;
}

SCM draw_square(int x, int y, int rad, int r, int g, int b, int a) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = rad;
	rect.h = rad;

	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	
	SDL_RenderFillRect(renderer, &rect);

	SDL_RenderPresent(renderer);

	return SCM_UNSPECIFIED;
}

