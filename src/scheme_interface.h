#ifndef _scheme_interface_h
#define _scheme_interface_h

// Function Definitions
SCM clear_screen();
SCM draw_square(int x, int y, int w, int h, int r, int g, int b, int a);
void* register_functions(void* data);

#endif // _scheme_interface_h
