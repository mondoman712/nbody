#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

#define G 6.67408e-11
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

int width = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;

SDL_Window *window;
SDL_Renderer *renderer;

struct vector {
	double x;
	double y;
};

struct body {
	struct vector pos;
	struct vector vel;
	unsigned int mass;
};

/*
 * Initialize the array, temporarily at 8 bodies, until I set up the inputs.
 */
struct body bodies[2];

/*
 * Temporary function to populate the system
 */
void populate_bodies () {
	int i;
	int bodies_length = sizeof(bodies)/sizeof(struct body);

	for (i = 0; i < bodies_length; i++) {
		bodies[i].pos.x = i * 100;
		bodies[i].pos.y = i * 100;
		bodies[i].vel.x = 0;
		bodies[i].vel.y = 0;
		bodies[i].mass = 100000;
	}
}

/*
 * Updates the positions and then velocities of all the bodies in the system
 */
void update_bodies ()
{
	int i, j;
	int bodies_length = sizeof(bodies)/sizeof(struct body);
	double _F, r_x, r_y, F_x, F_y;

	for (i = 0; i < (bodies_length - 1); i++) {
		for (j = (i + 1); j < bodies_length; j++) {

			_F = G * bodies[i].mass * bodies[j].mass;

			r_x = abs(bodies[i].pos.x - bodies[j].pos.x);
			r_y = abs(bodies[i].pos.y - bodies[j].pos.y);

			F_x = _F / pow(r_x, 2);
			F_y = _F / pow(r_y, 2);

			bodies[i].vel.x += F_x / bodies[i].mass;
			bodies[i].vel.y += F_y / bodies[i].mass;
			bodies[j].vel.x += F_x / bodies[j].mass;
			bodies[j].vel.y += F_y / bodies[j].mass;
		}
	}

	for (i = 0; i < bodies_length; i++) {
		bodies[i].pos.x += bodies[i].vel.x;
		bodies[i].pos.y += bodies[i].vel.y;
	}
}

void SDL_main_loop ()
{
	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e))
			if (e.type == SDL_QUIT)
				break;

		int i;
		double pos_x, pos_y;
		int bodies_length = sizeof(bodies)/sizeof(struct body);
		for (i = 0; i < bodies_length; i++) {
			pos_x = (width / 2) + bodies[i].pos.x;
			pos_y = (width / 2) - bodies[i].pos.y;
			SDL_RenderDrawPoint(renderer, pos_x, pos_y);
		}

		update_bodies();

		SDL_RenderPresent(renderer);
	}
}

int main(int argc, const char *argv[])
{
	populate_bodies();

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
			"nbody",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			1024,
			768,
			0);

	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_main_loop();

	int i;
	int bodies_length = sizeof(bodies)/sizeof(struct body);

	/*
	for (i = 0; i < bodies_length; i++) {
		printf("Final X Pos: %f, ", bodies[i].pos.x);
		printf("Final Y Pos: %f, ", bodies[i].pos.y);
		printf("Final X Vel: %f, ", bodies[i].vel.x);
		printf("Final Y Vel: %f, ", bodies[i].vel.y);
		printf("Mass: %i\n", bodies[i].mass);
	}

	for (i = 0; i <= 2; i++)
		update_bodies();
	*/

	for (i = 0; i < bodies_length; i++) {
		printf("Final X Pos: %f, ", bodies[i].pos.x);
		printf("Final Y Pos: %f, ", bodies[i].pos.y);
		printf("Final X Vel: %f, ", bodies[i].vel.x);
		printf("Final Y Vel: %f, ", bodies[i].vel.y);
		printf("Mass: %i\n", bodies[i].mass);
	}
	
	return 0;
}
