#ifndef _scheme_interface_h
#define _scheme_interface_h

#include "defs.h"

// Function Defanitions
void* register_functions(void* data);
SCM draw_square(int x, int y, int rad, int r, int g, int b, int a);
SCM clear_screen();

#endif // _scheme_interface_h
