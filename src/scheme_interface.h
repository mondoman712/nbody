#ifndef _scheme_interface_h
#define _scheme_interface_h

// Function Definitions
void* register_functions(void* data);
SCM clear_screen();
SCM draw_square(int x, int y, int w, int h, int r, int g, int b, int a);

#endif // _scheme_interface_h
