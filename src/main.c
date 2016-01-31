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
#include <assert.h>

#include <getopt.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <libguile.h>

#define G 6.67408e-11L
#define PI 3.141592653589

#define DIST_SCALE 1.0
#define TIME_SCALE 1000.0

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

#define SHADER_DIR "src/shaders/"
#define SHADER_EXT ".glsl"

#define KEY_QUIT SDLK_q

int width = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;
Uint32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
int centre_flag = 0;
unsigned int n = 0;

typedef struct {
	GLdouble x;
	GLdouble y;
	GLdouble z;
} vec3;

typedef struct {
	vec3 pos;
	vec3 vel;
	GLuint64 mass;
} body;

/*
 * Reads and compiles a .glsl shader file in the shaders folder, from just the
 * core of the filename (to use shaders/vs1.glsl, filename is just vsl)
 */
GLuint create_shader (const GLenum shader_type, const char * filename)
{
	char _source[4096];
	char dest[64];

	/* Create full filename */
	strcpy(dest, SHADER_DIR);
	strcat(dest, filename);
	strcat(dest, SHADER_EXT);

	/* Open file */
	FILE * s = fopen(dest, "r");
	if (s == NULL) {
		fprintf(stderr, "Could not open file %s\n", dest);
		return 1;
	}

	/* Read all lines in file */
	int i = 0;
	char inc;
	while (fscanf(s, "%c", &inc) > 0)
		_source[i++] = inc;
	
	/* Close file */
	fclose(s);

	/* Add string terminator to file */
	_source[i - 1] = '\0';

	/* Change type of source */
	const char * source = _source;

	/* Compile Shader */
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	/* Check for and report errors */
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char buff[512];
		glGetShaderInfoLog(shader, 512, NULL, buff);
		fprintf(stderr, "Shader %s failed to compile with error:\n", 
				filename);
		fprintf(stderr, "%s\n", buff);
		fprintf(stderr, "\n");
	}

	return shader;
}

/*
 * Fills a mat4 with zeros
 */
void zeros (GLfloat * mat4)
{
	GLushort i;
	for (i = 0; i < 16; i++)
		*(mat4 + i) = 0;
}

/*
 * Creates a 4x4 identity matrix
 */
void identity (GLfloat * mat4)
{
	zeros(mat4);
	
	*mat4 = 1;
	*(mat4 + 5) = 1;
	*(mat4 + 10) = 1;
	*(mat4 + 15) = 1;
}

/*
 * Creates a matrix to move a vector by the vector pos
 */
void translate (vec3 pos, GLfloat * mat4)
{
	identity(mat4);

	*(mat4 + 12) = pos.x;
	*(mat4 + 13) = pos.y;
	*(mat4 + 14) = pos.z;
}

/*
 * finds the modulus of a 1x3 vector
 */
static GLfloat mod_vec3 (vec3 in)
{
	return sqrt((in.x * in.x) + (in.y  * in.y) + (in.z * in.z));
}

/*
 * Finds the normal of a 1x3 vector
 */
static vec3 norm_vec3 (vec3 in)
{
	GLfloat mod = mod_vec3(in);
	vec3 ret = {
		in.x / mod,
		in.y / mod,
		in.z / mod
	};

	return ret;
}

/*
 * Finds the cross product of vectors a and b
 */
static vec3 cross_vec3 (vec3 a, vec3 b)
{
	vec3 ret;

	ret.x = (a.y * b.z) - (a.z * b.y);
	ret.y = (a.z * b.x) - (a.x * b.z);
	ret.z = (a.x * b.y) - (a.y * b.x);

	return ret;
}

/*
 * Calculates the dot product between two 3 value vectors
 */
static GLfloat dot_vec3 (vec3 a, vec3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

/*
 * Defines a matrix transformation to move the camera position
 */
void look_at (vec3 eye, vec3 centre, vec3 up, GLfloat * mat4)
{
	vec3 f, s, u;

	f.x = centre.x - eye.x;
	f.y = centre.y - eye.y;
	f.z = centre.z - eye.z;

	f = norm_vec3(f);
	s = norm_vec3(cross_vec3(f, up));
	u = cross_vec3(s, f);

	/*
	 * s.x 		s.y 		s.z 		0
	 * u.x 		u.y 		u.z 		0
	 * -f.x 	-f.y 		-f.z 		0
	 * 0 		0 		0 		1
	 */

	*mat4 = s.x;
	*(mat4 + 1) = u.x;
	*(mat4 + 2) = -f.x;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = s.y;
	*(mat4 + 5) = u.y;
	*(mat4 + 6) = -f.y;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = s.z;
	*(mat4 + 9) = u.z;
	*(mat4 + 10) = -f.z;
	*(mat4 + 11) = 0;

	*(mat4 + 12) = - dot_vec3(s, eye);
	*(mat4 + 13) = - dot_vec3(u, eye);
	*(mat4 + 14) = dot_vec3(f, eye);
	*(mat4 + 15) = 1;
}

/*
 * Defines a perspective projection matrix transformation
 */
void perspective (GLfloat fovy, GLfloat asp, GLfloat znear, GLfloat zfar,
		GLfloat * mat4)
{
	assert(asp != 0);
	assert(znear != zfar);

	GLfloat f = tan(fovy / 2);
	*mat4 = 1 / (f * asp);
	*(mat4 + 1) = 0;
	*(mat4 + 2) = 0;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = 0;
	*(mat4 + 5) = 1 / f;
	*(mat4 + 6) = 0;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = 0;
	*(mat4 + 9) = 0;
	*(mat4 + 10) = - (zfar + znear) / (zfar - znear); 
	*(mat4 + 11) = -1;

	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = - (2 * zfar * znear) / (zfar - znear); 
	*(mat4 + 15) = 0;
}

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat ** vertices, char ** mtl_loc)
{
	int i;
	SCM vrts;

	/* load scheme source file */
	scm_c_primitive_load("src/f_obj.scm");

	/* load scheme packages to use */
	scm_c_use_module("ice-9 rdelim");
	scm_c_use_module("srfi srfi-1");

	/* load necessary scheme function */
	SCM load_obj_sym = scm_c_lookup("load-obj");
	SCM load_obj = scm_variable_ref(load_obj_sym);

	/* call scheme function */
	vrts = scm_call_1(load_obj, scm_from_locale_string(filename));

	if (mtl_loc != NULL)
		*mtl_loc = scm_to_locale_string(scm_list_ref(vrts, scm_from_int(1)));
	vrts = scm_list_ref(vrts, scm_from_int(0));

	/* Copy vertices from scm list into array */
	*vertices = (GLfloat *) malloc((1 + scm_to_int(scm_length(vrts))) 
			* sizeof(GLfloat));
	if (*vertices == NULL) {
		fprintf(stderr, "Failed to allocate memory for %s\n", filename);
		return 1;
	}

	**vertices = (GLfloat) scm_to_double(scm_length(vrts));
	for (i = 0; i < **vertices; i++)
		*(*vertices + i + 1) =
			(GLfloat) scm_to_double(scm_list_ref(vrts,
						scm_from_int(i)));

	return 0;
}

/*
 * Returns the centre of mass of the array of bodies as a vector
 */
static vec3 centre_of_mass (body * bodies, int bodies_length)
{
	vec3 ret = {0, 0, 0};
	int i;
	GLuint64 mtot = 0;

	for (i = 0; i < bodies_length; i++) {
		ret.x += (bodies[i].pos.x * bodies[i].mass);
		ret.y += (bodies[i].pos.y * bodies[i].mass);
		ret.z += (bodies[i].pos.z * bodies[i].mass);
		mtot += bodies[i].mass;
	}

	ret.x /= mtot;
	ret.y /= mtot;
	ret.z /= mtot;

	return ret;
}

/*
 * Fills array with n bodies of random position and velocity (and maybe mass)
 */
static int genbods (int n, body * bodies)
{
	int i;

	for (i = 0; i < n; i++) {
		bodies[i].pos.x = (rand() % width - (width / 2)) / DIST_SCALE;
		bodies[i].pos.y = (rand() % height - (height / 2)) / DIST_SCALE;
		bodies[i].pos.z = (rand() % height - (height / 2)) / DIST_SCALE;

		bodies[i].vel.x = (rand() % 10000) / 1000000.0;
		bodies[i].vel.y = (rand() % 10000) / 1000000.0;
		bodies[i].vel.z = (rand() % 10000) / 1000000.0;

		bodies[i].mass = 1e9;
	}
	return 0;
}

/*
 * Calculates the accelerations of bodies a and b and edits their velocities
 */
static void calc_accels (body * a, body * b) 
{
	vec3 rv;
	GLdouble r, r2, g_ij, g_ji;

	rv.x = a->pos.x - b->pos.x;
	rv.y = a->pos.y - b->pos.y;
	rv.z = a->pos.z - b->pos.z;

	r2 = rv.x * rv.x + rv.y * rv.y + rv.z * rv.z;
	r = sqrtl(r2);

	g_ji = (b->mass * G) / r2; // Acceleration on I
	g_ij = (a->mass * G) / r2; // Acceleration on J

	rv.x = rv.x / r; 
	rv.y = rv.y / r;
	rv.z = rv.z / r;

	a->vel.x -= g_ji * rv.x;
	b->vel.x += g_ij * rv.x;

	a->vel.y -= g_ji * rv.y;
	b->vel.y += g_ij * rv.y;

	a->vel.z -= g_ji * rv.z;
	b->vel.z += g_ij * rv.z;
}


/*
 * Updates the positions and then velocities of all the bodies in the system
 */
static int update_bodies (body * bodies, int bodies_length, GLdouble t)
{
	int i, j;

	for (i = 0; i < (bodies_length - 1); i++)
		for (j = (i + 1); j < bodies_length; j++)
			calc_accels(&bodies[i], &bodies[j]);

	for (i = 0; i < bodies_length; i++) {
		bodies[i].pos.x += (bodies[i].vel.x * t * TIME_SCALE);
		bodies[i].pos.y += (bodies[i].vel.y * t * TIME_SCALE);
		bodies[i].pos.z += (bodies[i].vel.z * t * TIME_SCALE);
	}

	return 0;
}

/*
 * Draws the centre of mass point / centre point (when the centre flag is true)
 */
static int draw_com (vec3 com, SDL_Renderer * renderer)
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

void draw_body (body b, GLuint attr_vert, GLuint uni_model, GLfloat * verts)
{
	vec3 spos = {b.pos.x / 1024, b.pos.y / 768, b.pos.z / 1000};
	GLfloat model[16];
	translate(spos, model);
	glUniformMatrix4fv(uni_model, 1, GL_FALSE, model);
	
	glVertexAttribPointer(attr_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS, 0, *verts);
}
/*
 * Draws the bodies contained in the array of length bodies_length at *bodies
 */
static void draw_bodies (body * bodies, int bodies_length, GLuint attr_vert,
		GLuint uni_model, GLfloat * verts)
{
	int i;
	for (i = 0; i < bodies_length; i++)
		draw_body(bodies[i], attr_vert, uni_model, verts);
}

/*
 * Initializes the SDL window and runs the main loop
 */
static int rendering_loop (body * bodies, int bodies_length)
{
	SDL_Window *window;
	SDL_Event e;

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

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	if (gl_context == NULL) {
		fprintf(stderr, "Failed to create OpenGL context\n");
		return 1;
	}

	const unsigned char * version = glGetString(GL_VERSION);
	if (version == NULL) {
		fprintf(stderr, "Failed to get GL version\n");
		return 1;
	} else {
		fprintf(stderr, "GL version is: %s\n", version);
	}

	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status) {
		fprintf(stderr, "Error %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	/*
	GLfloat * verts = NULL;
	read_obj("assets/models/sphere.obj", &verts, NULL);
	*/
	GLfloat verts[] = {
		3,
		-0.1, -0.1, 0.0,
		0.1, -0.1, 0.0,
		0.1, 0.0, 0.0
	};

	glBufferData(GL_ARRAY_BUFFER, *verts, verts + 1,
			GL_STATIC_DRAW);
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	GLuint vs = create_shader(GL_VERTEX_SHADER, "vs1");
	GLuint fs = create_shader(GL_FRAGMENT_SHADER, "fs1");

	GLuint sp = glCreateProgram();
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glBindFragDataLocation(sp, 0, "out_colour");
	glLinkProgram(sp);
	glUseProgram(sp);

	GLint uni_model = glGetUniformLocation(sp, "model");

	int mx = 0, my = 0;
	GLfloat lon = PI, lat = PI / 2, r = 1.0;
	GLfloat sens = 1.0;
	GLfloat view[16];
	vec3 campos;
	campos.x = cos(lon) * sin(lat);
	campos.y = sin(lon) * sin(lat);
	campos.z = cos(lat);
	vec3 cen = {0.0, 0.0, 0.0}, up = {1.0, 0.0, 0.0};
	look_at(campos, cen, up, view);
	GLint uni_view = glGetUniformLocation(sp, "view");
	glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);

	GLfloat proj[16];
	GLfloat fov = PI / 2;
	perspective(fov, width / height, 0.1, 100.0, proj);
	GLint uni_proj = glGetUniformLocation(sp, "proj");
	glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint attr_vert = glGetAttribLocation(sp, "vert");

	Uint64 t, tl;
	GLdouble tpf;
	t = SDL_GetPerformanceCounter();

	while (1) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) 
				break;
			else if (e.type == SDL_KEYUP && e.key.keysym.sym == KEY_QUIT) 
				break;
			else if (e.type == SDL_MOUSEBUTTONDOWN 
					&& e.button.button == SDL_BUTTON_LEFT) {
				SDL_SetRelativeMouseMode(SDL_TRUE);
				SDL_GetMouseState(&mx, &my);
			} else if (e.type == SDL_MOUSEBUTTONUP
					&& e.button.button == SDL_BUTTON_LEFT)
				SDL_SetRelativeMouseMode(SDL_FALSE);
			else if (e.type == SDL_MOUSEWHEEL) {
				r -= e.wheel.y / 10.0;
				campos.x = r * cos(lon) * sin(lat);
				campos.y = r * sin(lon) * sin(lat);
				campos.z = r * cos(lat);
				look_at(campos, cen, up, view);
				glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);
			}
		}
		
		if (SDL_GetRelativeMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			lat -= ((GLfloat) my / (GLfloat) height) * sens;
			lon -= ((GLfloat) mx / (GLfloat) width) * sens;
			campos.x = r * cos(lon) * sin(lat);
			campos.y = r * sin(lon) * sin(lat);
			campos.z = r * cos(lat);
			look_at(campos, cen, up, view);
			glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);
		}


		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tl = t;
		t = SDL_GetPerformanceCounter();
		tpf = (GLdouble) (t - tl) / (GLdouble) SDL_GetPerformanceFrequency();
		
		vec3 com = centre_of_mass(bodies, bodies_length);
		update_bodies(bodies, bodies_length, tpf);
		draw_bodies(bodies, bodies_length, attr_vert, uni_model, verts);

		SDL_GL_SwapWindow(window);

		printf("\r%02.6fms", tpf * 1000);
	}

	printf("\n");

	SDL_DestroyWindow(window);

	return 0;
}

/*
 * Prints the help message
 */
static void print_help ()
{
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
}


/*
 * Parses the command line arguments, better here than all this being in main
 */
static int parse_opts (int argc, char **argv)
{
	int c, option_index;

	static struct option long_options[] = {
		{"width", required_argument, 0, 'w'},
		{"height", required_argument, 0, 'l'},
		{"fullscreen", no_argument, 0, 'f'},
		{"generate", required_argument, 0, 'g'},
		{"centre", no_argument, 0, 'c'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	while(1) {
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
			window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
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
			print_help();
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
	scm_init_guile();

	if (parse_opts(argc, argv) == -1)
		exit(EXIT_FAILURE);

	body bodies[n];
	
	if (n) {
		srand(time(NULL));
	       	genbods(n, bodies);
	}

	if (rendering_loop(bodies, n)) {
		fprintf(stderr, "Error at rendering\n");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
