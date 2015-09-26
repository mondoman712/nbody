#include "defs.h"

SCM draw_circle(int x, int y, int rad, int r, int g, int b, int a) {
	filledCicleRGBA(renderer, x, y, rad, r, g, b, a);
	return NULL;
}
