#include <stdlib.h>
#include <math.h>

#define G 6.67408e-11

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
struct body bodies[8];

/*
 * Temporary function to populate the system
 */
void populate_bodies () {
	int i;
	int bodies_length = sizeof(bodies)/sizeof(struct body);

	for (i = 0; i < bodies_length; i++) {
		bodies[i].pos.x = i * 100;
		bodies[i].pos.y = i * 100;
		bodies[i].vel.x = i * 100;
		bodies[i].vel.y = i * 100;
		bodies[i].mass = 1000000;
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
		for (j = 0; j < bodies_length; j++) {

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

int main(int argc, const char *argv[])
{

	int i;

	populate_bodies();

	for (i = 0; i <= 1000000; i++)
		update_bodies();
	
	return 0;
}
