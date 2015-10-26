/*
 * Nbody, A completely unscientific N body simulator written as a learning
 * project. Started out as a simple orbit sim for my A level computing
 * coursework, written in common lisp. I then adapted it to work as an N body
 * simulator. Since then I have been rewriting it in C, again just as a learning
 * experience.
 *
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 of the License, or (at your option) any later 
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) Sam Smith, 2015
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

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
int centre_flag = 0;
unsigned int n = 0;

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
 * Returns the centre of mass of the array of bodies as a vector
 */
struct vector centre_of_mass (struct body * bodies, int bodies_length)
{
	struct vector ret = {0, 0};
	int i, mtot;

	for (i = 0; i < bodies_length; i++) {
		ret.x += bodies[i].pos.x * bodies[i].mass;
		ret.y += bodies[i].pos.y * bodies[i].mass;
		mtot += bodies[i].mass;
	}

	ret.x /= mtot;
	ret.y /= mtot;

	return ret;
}

/*
 * Fills array with n bodies of random position and velocity (and maybe mass)
 */
int genbods (int n, struct body * bodies)
{
	int i;
	for (i = 0; i < n; i++) {
		bodies[i].pos.x = (rand() % width - (width / 2)) / DIST_SCALE;
		bodies[i].pos.y = (rand() % height - (height / 2)) / DIST_SCALE;
		bodies[i].vel.x = (rand() % 10000) / 1000000.0;
		bodies[i].vel.y = (rand() % 10000) / 1000000.0;

		bodies[i].mass = 1e9;
	}
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
 * Draws the centre of mass point / centre point (when the centre flag is true)
 */
int draw_com (struct vector com, SDL_Renderer * renderer)
{
	if (centre_flag) {
		com.x = width / 2;
		com.y = height / 2;
	} else {
		com.x = (width / 2) + (com.x / DIST_SCALE);
		com.y = (height / 2) - (com.y / DIST_SCALE);
	}

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawPoint(renderer, com.x, com.y);

	return 0;
}

int draw_bodies (struct body * bodies, int bodies_length, struct vector com,
		SDL_Renderer * renderer)
{
	int i;
	long double pos_x, pos_y;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (i = 0; i < bodies_length; i++) {
		pos_x = (width / 2) + (bodies[i].pos.x / DIST_SCALE);
		if (centre_flag) pos_x -= com.x;

		pos_y = (height / 2) - (bodies[i].pos.y / DIST_SCALE);
		if (centre_flag) pos_y += com.y;

		SDL_RenderDrawPoint(renderer, pos_x, pos_y);
	}

	return 0;
}

/*
 * Initializes the SDL window and runs the main loop
 */
static int rendering_loop (struct body * bodies, int bodies_length)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event e;
	SDL_DisplayMode d;

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
		
		struct vector com = centre_of_mass(bodies, bodies_length);
		draw_bodies(bodies, bodies_length, com, renderer);
		if (update_bodies(bodies, bodies_length)) break;

		draw_com(com, renderer);

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
			{"height", required_argument, 0, 'l'},
			{"fullscreen", no_argument, 0, 'f'},
			{"generate", required_argument, 0, 'g'},
			{"centre", no_argument, 0, 'c'},
			{"help", no_argument, 0, 'h'},
			{0, 0, 0, 0}
		};

		option_index = 0;

		c = getopt_long(argc, argv, "w:l:fg:ch",
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
				fprintf(stderr, "Option w requires int value\n");
				return -1;
			}
			break;

		case 'l':
			if(!(height = atoi(optarg))) {
				fprintf(stderr, "Option h requires int value\n");
				return -1;
			}
			break;

		case 'f':
			window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
			break;

		case 'g':
			if (!(n = atoi(optarg))) {
				fprintf(stderr, "Option n requires int value\n");
				return -1;
			}
			break;

		case 'c':
			centre_flag = 1;
			break;

		case 'h':
			printf("Usage: ./nbody [OPTION]...\n");
			printf("Simplistic 2D N body simulator\n\n");
			printf("Options:\n");
			printf(" -w, --width\t Width of the window\n");
			printf(" -l, --height\t Height of the window\n");
			printf(" -f, --fullscreen\t Sets window to fullscreen\n");
			printf(" -g, --generate\t Randomly generates given number of bodies\n");
			printf(" -c, --centre\t Centres renders onto the centre of mass\n\n");
			printf("Examples:\n");
		        printf("\t./nbody -fg 128\t Generates 128 bodies and runs fullscreen\n\n");
			printf("Github: <https://github.com/mondoman712/nbody>\n");
			exit(EXIT_SUCCESS);

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
	srand(time(NULL));

	if (parse_opts(argc, argv) == -1)
		exit(EXIT_FAILURE);

	struct body bodies[n];
	
	if (n) genbods(n, bodies);

	if (rendering_loop(bodies, n)) {
		fprintf(stderr, "Error at rendering\n");
		exit(EXIT_FAILURE);
	}

	print_system(bodies, n);
	
	exit(EXIT_SUCCESS);
}
