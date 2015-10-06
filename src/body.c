#include <stdlib.h>
#include <math.h>

#include "bodydefs.h"

int add_body (vector pos, vector vel, int mass)
{
	body newbod;
	newbod.pos = pos;
	newbod.vel = vel;
	newbod.mass = mass;

	if (!firstbod)
		firstbod = &newbod;

	if (lastbod)
		lastbod->nextbod = &newbod;

	lastbod = &newbod;

	return 0;
}

int vect_eq (vector vect1, vector vect2)
{
	return (vect1.x == vect2.x) && (vect1.y == vect2.y);
}

int body_eq (body bod1, body bod2)
{
	if (vect_eq(bod1.pos, bod2.pos) &&
			vect_eq(bod1.vel, bod2.vel) &&
			bod1.mass == bod2.mass)
		return 1;
	else return 0;
}

/*
 * exact version of pow, for when the exponent is an integer.
 */
double expow (double x, int y)
{
	if (y == 0)
		return 1;
	else if (y == 1)
		return x;
	else if (y == 2)
		return x * x;
	else {
		double tot = x;
		int i;

		for (i = 2; i <= y; i++)
			tot *= x;

		return tot;
	}
}

/*
 * Calculates the distance between 2 vectors
 */
double distance (vector pos1, vector pos2)
{
	double a, b;

	a = expow(abs(pos1.x - pos2.x), 2);
	b = expow(abs(pos1.y - pos2.y), 2);

	return sqrt(a + b);
}

/*
void update_body (body* bod)
{
	body* inc = firstbod;

	while (inc) {
		// Do things here

		inc = inc->nextbod;
	}

	
}
*/
