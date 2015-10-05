#include "main.h"

typedef struct {
	double x;
	double y;
} vector;

typedef struct {
	vector pos;
	vector vel;
	int mass;
} body;

typedef struct {
	vector bl;
	vector tr;
} quad;

typedef struct bhtree {
	body bod;
	quad q;
	struct bhtree* nw;
	struct bhtree* ne;
	struct bhtree* sw;
	struct bhtree* se;
} bhtree;
