#ifndef TOOLS_H
#define TOOLS_H


#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>




int factorial(int number);
char * int2str(int number);
char * uint2str(unsigned int n);
char * double2str(double number);
void hsv2rgb(float h, float s, float v, float *r, float *g, float*b);
void rgb2hsv(float r, float g, float b, float *h, float *s, float *v);
std::string int2stdstr(int number);
std::string float2stdstr(float number);
double find_max (std::vector<double> *v) ;
void HilbertMap(double in, double *xout, double *yout, int depth, int exit);
std::string ip2name(std::string ipstr);
uint32_t ip_stdstring_to_uint32(std::string str);
std::string ip_uint32_to_stdstring(uint32_t ip);
float bound(float num, float min, float max);


float max(float f, double d);



void colorMap(unsigned char *rgb,float value,float min,float max);
void hotColorMap(unsigned char *rgb,float value,float min,float max);
void coldColorMap(unsigned char *rgb,float value,float min,float max);
void jetColorMap(unsigned char *rgb,float value,float min,float max);
void cyclicColorMap(unsigned char *rgb,float value,float min,float max);
void randColorMap(unsigned char *rgb,float value,float min,float max);
void grayColorMap(unsigned char *rgb,float value,float min,float max);
void blueColorMap(unsigned char *rgb,float value,float min,float max);


#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

double deg2rad(double deg);
double rad2deg(double rad);
double toMollweide(double phi, double lambda, double &x, double &y);



#endif // TOOLS_H
