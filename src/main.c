#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#define G 6.67408e-11L
#define DIST_SCALE 1000
#define TIME_SCALE 1

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

int width = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;

struct vector {
	long double x;
	long double y;
};

struct body {
	struct vector pos;
	struct vector vel;
	unsigned long mass;
};

int count = 0;

/*
 * Initialize the array, temporarily at 8 bodies, until I set up the inputs.
 */
struct body bodies[2];

/*
 * Temporary function to populate the system
 */
void populate_bodies () {
	/*
	int i;
	int bodies_length = sizeof(bodies)/sizeof(struct body);

	for (i = 0; i < bodies_length; i++) {
		bodies[i].pos.x = i * 100;
		bodies[i].pos.y = i * 100;
		bodies[i].vel.x = 0;
		bodies[i].vel.y = 0;
		bodies[i].mass = 1000000;
	}
	*/

	unsigned long mass = 10e10;

	bodies[0].pos.x = -100000;
	bodies[0].pos.y = 0;
	bodies[0].vel.x = 0;
	bodies[0].vel.y = -1;
	bodies[0].mass = mass;

	bodies[1].pos.x = 100000;
	bodies[1].pos.y = 0;
	bodies[1].vel.x = 0;
	bodies[1].vel.y = 1;
	bodies[1].mass = mass;
}

/*
 * Updates the positions and then velocities of all the bodies in the system
 */
void update_bodies ()
{
	int i, j;
	int bodies_length = sizeof(bodies)/sizeof(struct body);
	long double F, r_x, r_y, F_x, F_y, r_x2, r_y2;

	for (i = 0; i < (bodies_length - 1); i++) {
		for (j = (i + 1); j < bodies_length; j++) {

			F = G * bodies[i].mass * bodies[j].mass;
			r_x = bodies[i].pos.x - bodies[j].pos.x;
			r_y = bodies[i].pos.y - bodies[j].pos.y;

			r_x2 = powl(r_x, 2);
			r_y2 = powl(r_x, 2);
			F = F / (r_x2 + r_y2);
			
			F_x = F * r_x;
			bodies[i].vel.x -= F_x / bodies[i].mass;
			bodies[j].vel.x += F_x / bodies[j].mass;

			F_y = F * r_y;
			bodies[i].vel.y -= F_y / bodies[i].mass;
			bodies[j].vel.y += F_y / bodies[j].mass;
		}
	}

	for (i = 0; i < bodies_length; i++) {
		bodies[i].pos.x += (bodies[i].vel.x * TIME_SCALE);
		bodies[i].pos.y += (bodies[i].vel.y * TIME_SCALE);
	}
}

void rendering_loop()
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event e;

	int i;
	long double pos_x, pos_y;
	int bodies_length = sizeof(bodies)/sizeof(struct body);

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
			"nbody",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			0);

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderPresent(renderer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	while (1) {
		if (SDL_PollEvent(&e) && (e.type == SDL_QUIT))
				break;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		for (i = 0; i < bodies_length; i++) {
			pos_x = (width / 2) + (bodies[i].pos.x / DIST_SCALE);
			pos_y = (height / 2) - (bodies[i].pos.y / DIST_SCALE);
			SDL_RenderDrawPoint(renderer, pos_x, pos_y);
		}

		update_bodies();
		count++;

		SDL_RenderPresent(renderer);
	}
}

int main(int argc, const char *argv[])
{
	populate_bodies();

	rendering_loop();

	int i;
	int bodies_length = sizeof(bodies)/sizeof(struct body);

	/*
	for (i = 0; i <= 10; i++)
		update_bodies();
	*/

	for (i = 0; i < bodies_length; i++) {
		printf("Final X Pos: %Lf, ", bodies[i].pos.x);
		printf("Final Y Pos: %Lf, ", bodies[i].pos.y);
		printf("Final X Vel: %Lf, ", bodies[i].vel.x);
		printf("Final Y Vel: %Lf, ", bodies[i].vel.y);
		printf("Mass: %ld\n", bodies[i].mass);
	}

	printf("Count: %i\n", count);
	
	return 0;
}
