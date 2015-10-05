#include "bodydefs.h"

void add_body (vector pos, vector vel, int mass) {
	body newbod;
	newbod.pos = pos;
	newbod.vel = vel;
	newbod.mass = mass;

	lastbod->nextbod = &newbod;
}


