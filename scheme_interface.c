#include "defs.h"
#include "main.h"

void* register_functions(void* data) {
	return NULL;
}

SCM draw_circle(int x, int y, int rad, int r, int g, int b, int a) {
	filledCicleRGBA(renderer, x, y, rad, r, g, b, a);
	return NULL;
}

scm_c_define_gsubr("draw-circle", 7, 0, 0, &draw_circle);
