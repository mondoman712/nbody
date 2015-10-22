#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include <getopt.h>
#include <SDL2/SDL.h>

#define G 6.67408e-11L
#define DIST_SCALE 1.0
#define TIME_SCALE 1.0

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

#define KEY_QUIT SDLK_q

int width = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;
Uint32 window_flags = 0;

struct vector {
	long double x;
	long double y;
};

struct body {
	struct vector pos;
	struct vector vel;
	unsigned long mass;
};

struct bodies_wl {
	struct body * bodies;
	int len;
};

/*
 * Temporary function to populate the system
 */
static int populate_bodies (struct body *bodies) 
{
	unsigned long mass = 10e7;

	bodies[0].pos.x = 0;
	bodies[0].pos.y = 0;
	bodies[0].vel.x = 0;
	bodies[0].vel.y = 0;
	bodies[0].mass = mass * 100;

	bodies[1].pos.x = 100;
	bodies[1].pos.y = 0;
	bodies[1].vel.x = 0;
	bodies[1].vel.y = -0.08;
	bodies[1].mass = mass;

	return 0;
}

/*
 * Prints each value for each body in the system in the same format it takes as
 * input
 */
static int print_system (struct body *bodies, int bodies_length)
{
	int i;

	fprintf(stdout, "%f, %f\n", DIST_SCALE, TIME_SCALE);
	for (i = 0; i < bodies_length; i++) {
		fprintf(stdout, "%Lf, ", bodies[i].pos.x);
		fprintf(stdout, "%Lf, ", bodies[i].pos.y);
		fprintf(stdout, "%Lf, ", bodies[i].vel.x);
		fprintf(stdout, "%Lf, ", bodies[i].vel.y);
		fprintf(stdout, "%ld;\n", bodies[i].mass);
	}

	return 0;
}

/*
 * Calculates the accelerations of bodies a and b and edits their velocities
 */
static int calc_accels (struct body *a, struct body *b) 
{
	long double r_x, r_y, r2, r, g_ij, g_ji;

	r_x = a->pos.x - b->pos.x;
	r_y = a->pos.y - b->pos.y;

	r2 = powl(r_x, 2) + powl(r_y, 2);
	r = sqrtl(r2);

	g_ji = (b->mass * G) / r2; // Acceleration on I
	g_ij = (a->mass * G) / r2; // Acceleration on J

	r_x = r_x / r; 
	r_y = r_y / r;
	a->vel.x -= g_ji * r_x;
	b->vel.x += g_ij * r_x;
	a->vel.y -= g_ji * r_y;
	b->vel.y += g_ij * r_y;

	return 0;
}


/*
 * Updates the positions and then velocities of all the bodies in the system
 */
static int update_bodies (struct body *bodies, int bodies_length)
{
	int i, j;

	for (i = 0; i < (bodies_length - 1); i++)
		for (j = (i + 1); j < bodies_length; j++)
			if (calc_accels(&bodies[i], &bodies[j])) return -1;

	for (i = 0; i < bodies_length; i++) {
		bodies[i].pos.x += (bodies[i].vel.x * TIME_SCALE);
		bodies[i].pos.y += (bodies[i].vel.y * TIME_SCALE);
	}

	return 0;
}

/*
 * Initializes the SDL window and runs the main loop
 */
static int rendering_loop(struct body *bodies, int bodies_length)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event e;
	SDL_DisplayMode d;

	int i;
	long double pos_x, pos_y;

	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL not Initialized\n");
		return 1;
	}

	window = SDL_CreateWindow(
			"nbody",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			window_flags);

	if (window == NULL) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		return 1;
	}

	if (window_flags == SDL_WINDOW_FULLSCREEN_DESKTOP) {
		if (SDL_GetWindowDisplayMode(window, &d)) return 1;
		width = d.w;
		height = d.h;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderPresent(renderer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	while (1) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) 
				break;
			else if (e.type == SDL_KEYUP && e.key.keysym.sym == KEY_QUIT) 
				break;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		for (i = 0; i < bodies_length; i++) {
			pos_x = (width / 2) + (bodies[i].pos.x / DIST_SCALE);
			pos_y = (height / 2) - (bodies[i].pos.y / DIST_SCALE);
			SDL_RenderDrawPoint(renderer, pos_x, pos_y);
		}

		if (update_bodies(bodies, 2)) break;

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);

	return 0;
}

/*
 * Parses the command line arguments, better here than all this being in main
 */
static int parse_opts (int argc, char **argv)
{
	int c, option_index;

	while(1) {
		static struct option long_options[] = {
			{"width", required_argument, 0, 'w'},
			{"height", required_argument, 0, 'h'},
			{"fullscreen", no_argument, 0, 'f'},
			{0, 0, 0, 0}
		};

		option_index = 0;

		c = getopt_long(argc, argv, "w:h:f",
				long_options, &option_index);

		if (c == -1)
			break;

		switch (c) {
		case 0:
			if (long_options[option_index].flag != 0) 
				break;
			printf("option %s", long_options[option_index].name);
			if(optarg) 
				printf("with arg %s", optarg);
			printf("\n");
			break;

		case 'w':
			if(!(width = atoi(optarg))) {
				printf("Option w requires int value\n");
				return -1;
			}
			break;

		case 'h':
			if(!(height = atoi(optarg))) {
				printf("Option h requires int value\n");
				return -1;
			}
			break;

		case 'f':
			window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
			break;

		case '?':
			break;

		default:
			abort();
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	if (parse_opts(argc, argv)) {
		fprintf(stderr, "Error at parse_opts\n");
		exit(EXIT_FAILURE);
	}

	struct body bodies[2];
	populate_bodies(bodies);

	if (rendering_loop(bodies, 2)) {
		fprintf(stderr, "Error at rendering\n");
		exit(EXIT_FAILURE);
	}

	print_system(bodies, 2);
	
	exit(EXIT_SUCCESS);
}
