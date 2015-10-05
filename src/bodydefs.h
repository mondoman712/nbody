#ifndef _bodydefs_h
#define _bodydefs_h

typedef struct {
	double x;
	double y;
} vector;

typedef struct body {
	vector pos;
	vector vel;
	int mass;
	struct body* nextbod;
} body;

#endif // _bodydefs_h
