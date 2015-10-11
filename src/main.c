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

/*
 * Initialize the array, temporarily at 8 bodies, until I set up the inputs.
 */
struct body bodies[2];

/*
 * Temporary function to populate the system
 */
void populate_bodies () {
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
}

/*
 * Prints each value for each body in the system in the same format it takes as
 * input
 */
static int print_system ()
{
	int i;
	int bodies_length = sizeof(bodies)/sizeof(struct body);

	printf("%f, %f\n", DIST_SCALE, TIME_SCALE);
	for (i = 0; i < bodies_length; i++) {
		printf("%Lf, ", bodies[i].pos.x);
		printf("%Lf, ", bodies[i].pos.y);
		printf("%Lf, ", bodies[i].vel.x);
		printf("%Lf, ", bodies[i].vel.y);
		printf("%ld;\n", bodies[i].mass);
	}

	return 0;
}

/*
 * Updates the positions and then velocities of all the bodies in the system
 */
static int update_bodies ()
{
	int i, j;
	int bodies_length = sizeof(bodies)/sizeof(struct body);
	long double r_x, r_y, r2, r, g_ij, g_ji;

	for (i = 0; i < (bodies_length - 1); i++) {
		for (j = (i + 1); j < bodies_length; j++) {

			r_x = bodies[i].pos.x - bodies[j].pos.x;
			r_y = bodies[i].pos.y - bodies[j].pos.y;

			r2 = powl(r_x, 2) + powl(r_y, 2);
			r = sqrtl(r2);

			g_ji = (bodies[j].mass * G) / r2; // Acceleration on I
			g_ij = (bodies[i].mass * G) / r2; // Acceleration on J

			r_x = r_x / r; 
			r_y = r_y / r;
			bodies[i].vel.x -= g_ji * r_x;
			bodies[j].vel.x += g_ij * r_x;
			bodies[i].vel.y -= g_ji * r_y;
			bodies[j].vel.y += g_ij * r_y;
		}
	}

	for (i = 0; i < bodies_length; i++) {
		bodies[i].pos.x += (bodies[i].vel.x * TIME_SCALE);
		bodies[i].pos.y += (bodies[i].vel.y * TIME_SCALE);
	}

	return 0;
}

/*
 * Initializes the SDL window and runs the main loop
 */
static int rendering_loop()
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event e;
	SDL_DisplayMode d;

	int i;
	long double pos_x, pos_y;
	int bodies_length = sizeof(bodies)/sizeof(struct body);

	if (SDL_Init(SDL_INIT_VIDEO)) return 1;

	window = SDL_CreateWindow(
			"nbody",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			window_flags);

	if (window_flags == SDL_WINDOW_FULLSCREEN_DESKTOP) {
		if (SDL_GetWindowDisplayMode(window, &d)) return 1;
		width = d.w;
		height = d.h;
	}

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

		if (update_bodies()) return 1;

		SDL_RenderPresent(renderer);
	}

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
	if (parse_opts(argc, argv)) return -1;

	populate_bodies();

	if (rendering_loop()) return 1;

	print_system();
	
	return 0;
}
