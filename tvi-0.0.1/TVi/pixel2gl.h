#ifndef PIXEL2GL_H
#define PIXEL2GL_H



#define GL_W 10.0f
#define GL_H GL_W


// Given a point (qt_x,qt_y) in QT format (pixel), in a window qt_W x qt_H
// this function returns the (gl_x,gl_y) floats of OpenGL coords
// in a gl_W x gl_H window (it's supposed [-gl_W/2,gl_W/2]x [-gl_H/2,gl_H/2])


/*

 OpenGL
                            (W/2,H/2)
 +-------------+-------------+
 |             |             |
 |             |(0,0)        |
 +-------------+-------------+
 |             |             |
 |             |             |
 +-------------+-------------+
(-W/2,-H/2)


               ^ y
               |
               |
        -------+-------> x
               |
               |








 QT

(0,0)
 +-------------+-------------+
 |             |             |
 |             |(W/2,H/2)    |
 +-------------+-------------+
 |             |             |
 |             |             |
 +-------------+-------------+
                            (W,H)


 +-------> x
 |
 |
 V y



 */


void qt2gl_coord(int qt_W, int qt_H, int gl_W, int gl_H, int qt_x, int qt_y, float * gl_x, float * gl_y);
void gl2qt_coord(int qt_W, int qt_H, int gl_W, int gl_H, int * qt_x, int * qt_y, float gl_x, float gl_y);




#endif // PIXEL2GL_H
