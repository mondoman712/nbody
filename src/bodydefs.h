#ifndef _bodydefs_h
#define _bodydefs_h

typedef struct vector_tag{
	double x;
	double y;
} vector;

typedef struct body_tag {
	vector pos;
	vector vel;
	int mass;
	struct body_tag* nextbod;
} body;

extern body* firstbod;
extern body* lastbod;

#endif // _bodydefs_h
