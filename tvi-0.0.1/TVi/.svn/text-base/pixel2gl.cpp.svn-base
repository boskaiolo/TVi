#include "pixel2gl.h"


void qt2gl_coord(int qt_W, int qt_H, int gl_W, int gl_H, int qt_x, int qt_y, float * gl_x, float * gl_y)
{
    *gl_x = ( (float)qt_x/ (float) qt_W * gl_W) - gl_W/2;

    *gl_y = gl_H/2 - ( (float)qt_y/ (float) qt_H * gl_H);

}




void gl2qt_coord(int qt_W, int qt_H, int gl_W, int gl_H, int * qt_x, int * qt_y, float gl_x, float gl_y)
{
    *qt_x = (int) ( gl_x/gl_W * (float) qt_W + qt_W/2.0f);

    *qt_y = (int) (qt_H/2.0f - gl_y/gl_H * (float) qt_H);

}

