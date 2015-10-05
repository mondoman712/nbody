#include "main.h"
#include "bht.h"

int contains (quad q, vector v) {
	if ((v.x - q.bl.x) <= (q.tr.x - q.bl.x) &&
			(v.y - q.bl.y) <= (q.tr.y - q.bl.y))
		return 1;
	else return 0;
}


