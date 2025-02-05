#include "gvga.h"

extern int nw;
extern int nw2;
extern int nh;
extern int nh2;
extern int dx;
extern int dy;

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

EXTERN_C void draw_numbers(GVga *gvga, char *s, int x, int y, int color);
EXTERN_C void draw_digit(GVga *gvga, int x, int y, int color, int digit);
