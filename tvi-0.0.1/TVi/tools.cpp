#include "tools.h"


#include <iostream>
#include <sstream>
#include <cmath>
#include <math.h>
#include <iostream>


int factorial(int number) {
        int temp;

        if(number <= 1) return 1;

        temp = number * factorial(number - 1);
        return temp;
}


char * uint2str(unsigned int n)
{

    static char s[1000];

    sprintf(s, "%u", n);

    return &s[0];

}


char * int2str(int n)
{

    static char s[1000];

    sprintf(s, "%d", n);

    return &s[0];

}


char * double2str(double number)
{
   std::stringstream ss;
   ss.precision(5);
   ss.setf(std::ios::scientific);
   ss << number;
   return (char *) ss.str().c_str();
}



double find_max (std::vector<double> *v) {

    double max = 0;

    for (size_t i=0; i<(*v).size(); i++) {
        if ((*v)[i]>max){
            max = (*v)[i];
        }
    }

    return max;
}


std::string int2stdstr(int number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}


std::string float2stdstr(float number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}

#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)
/* Input:   r, g, b in range [0..1]
   Outputs: h, s, v in range [0..1]
*/
void rgb2hsv(float r, float g, float b, float *h, float *s, float *v)
{
  float max = MAX (r, MAX (g, b)), min = MIN (r, MIN (g, b));
  float delta = max - min;

  *v = max;
  if (max != 0.0)
    *s = delta / max;
  else
    *s = 0.0;
  if (*s == 0.0) *h = -1;
  else {
    if (r == max)
      *h = (g - b) / delta;
    else if (g == max)
      *h = 2 + (b - r) / delta;
    else if (b == max)
      *h = 4 + (r - g) / delta;
    *h *= 60.0;
    if (*h < 0) *h += 360.0;
    *h /= 360.0;
  }
}

/*
 *----------------------------------------------------------
 *
 *  hsv2rgb:  Converts a color specified in the HSV system
 *            to the RGB system.
 *
 *  On input:  h = Hue in the range 0 to 1 (degrees/360),
 *                 where red, green, and blue correspond
 *                 to h = 0, h = 120, and h = 240, respec-
 *                 tively.
 *
 *             s = Saturation value in [0,1], where s = 0
 *                 implies greyscale (h is irrelevant and
 *                 r = g = b = v), and s = 1 implies a
 *                 pure hue (at least one of r, g, and b
 *                 is 0).
 *
 *             v = Value of intensity in [0,1], where
 *                 v = 0 implies black, and v = 1 implies
 *                 the maximum brightness.
 *
 *  A multiple of 360 is added to h if necessary, and the
 *  values of s and v are clamped to the interval [0,1].
 *
 *  On output:  r,g,b = Red, green, and blue color compo-
 *                      nents in the range [0,1].
 *
 *  glsurf functions called:  None
 *  OpenGL functions called:  None
 *
 *----------------------------------------------------------
 */
void hsv2rgb(float h, float s, float v, float *r, float *g, float*b)
{
   int i;         /* Integer part of h/60 */
   float f;       /* Fractional part of h/60 */
   float p,q,t;   /* Scaled down intensities */

   h = h*360;

   while (h < 0.0) {   /* Adjust h if necessary */
      h += 360.0;
   }
   while (h >= 360.0) {
      h -= 360.0;
   }
   if (s < 0.0) {      /* Clamp s to [0,1] */
      s = 0.0;
   } else {
   if (s > 1.0) {
      s = 1.0;
   }}

   if (v < 0.0) {      /* Clamp v to [0,1] */
      v = 0.0;
   } else {
   if (v > 1.0) {
      v = 1.0;
   }}

   h /= 60.0;     /* Map h to [0,6), and compute i and f */
   i = (int) h;
   f = h - (float) i;

   p = v*(1.0 - f*s);        /* Compute p, q, and t */
   q = v*(1.0 - (1.0-f)*s);
   t = v*(1.0 - s);
   switch (i) {
   case 0:
      *r = v;  *g = q;  *b = t;
      break;
   case 1:
      *r = p;  *g = v;  *b = t;
      break;
   case 2:
      *r = t;  *g = v;  *b = q;
      break;
   case 3:
      *r = t;  *g = p;  *b = v;
      break;
   case 4:
      *r = q;  *g = t;  *b = v;
      break;
   case 5:
      *r = v;  *g = t;  *b = p;
      break;
   default:
      break;
   }
   return;
}





/////////////////////////////cut////////////////////////////////
/*
*HilbertMap()
*  Takes a value in between 0 and 1-4^(-depth).
*  Output written to *xout and *yout.
*  depth is the order of the curve (12 is sub-pixel for most modern
displays).
*  exit used internally, leave at 0 calling externally
*/
void HilbertMap(double in, double *xout, double *yout, int depth, int exit)
{
 double x,y;
 int opt=(int)floor(fabsf(in)*4);
 if(depth<=0)
 {
  switch (exit)
  {
    case 0:
      x= 0.5;
      y= 0.0;
    break;
    case 1:
      x= 0.0;
      y=-0.5;
    break;
  }
  switch(opt)
  {
    case 0:
      *xout=.25;
      *yout=.25+.5*(4*in-opt);
    break;
    case 1:
      *xout=.25+.5*(4*in-opt);
      *yout=.75;
    break;
    case 2:
      *xout=.75;
      *yout=.75-.5*(4*in-opt);
    break;
    case 3:
    default :
      *xout=.75+x*(4*in-opt);
      *yout=.25+y*(4*in-opt);
    break;
  }
  return;
 }
 else
 {
  switch(opt)
  {
    case 0:
      HilbertMap(4*fabsf(in)-opt,&x,&y,depth-1,0);
      *xout=0.0+0.5*y;
      *yout=0.0+0.5*x;
    break;
    case 1:
      HilbertMap(4*fabsf(in)-opt,&x,&y,depth-1,0);
      *xout=0.0+0.5*x;
      *yout=0.5+0.5*y;
    break;
    case 2:
      HilbertMap(4*fabsf(in)-opt,&x,&y,depth-1,1);
      *xout=0.5+0.5*x;
      *yout=0.5+0.5*y;
    break;
    case 3:
    default :
      HilbertMap(4*fabsf(in)-opt,&x,&y,depth-1,1-exit);
      *xout=1.0-0.5*y;
      *yout=0.5-0.5*x;
    break;
  }
 }
}


#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
std::string ip2name(std::string ipstr)
{

    return std::string("ip2name");


    struct in_addr ip;
    struct hostent *hp;
    if (!inet_aton(ipstr.c_str(), &ip))
        return std::string("");

    if ((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL)
        return std::string(ipstr);
    else
        return std::string("(")+std::string(hp->h_name)+std::string(")");

}








void jetColorMap(unsigned char *rgb,float value,float min,float max)
{
  unsigned char c1=144;
  float max4=(max-min)/4;
  value-=min;
  if(value==HUGE_VAL)
    {rgb[0]=rgb[1]=rgb[2]=255;}
  else if(value<0)
    {rgb[0]=rgb[1]=rgb[2]=0;}
  else if(value<max4)
    {rgb[0]=0;rgb[1]=0;rgb[2]=c1+(unsigned char)((255-c1)*value/max4);}
  else if(value<2*max4)
    {rgb[0]=0;rgb[1]=(unsigned char)(255*(value-max4)/max4);rgb[2]=255;}
  else if(value<3*max4)
    {rgb[0]=(unsigned char)(255*(value-2*max4)/max4);rgb[1]=255;rgb[2]=255-rgb[0];}
  else if(value<max)
    {rgb[0]=255;rgb[1]=(unsigned char)(255-255*(value-3*max4)/max4);rgb[2]=0;}
  else {rgb[0]=255;rgb[1]=rgb[2]=0;}
}

void hotColorMap(unsigned char *rgb,float value,float min,float max)
{

    value += 0.20f*(max-value);

  float max3=(max-min)/3;
  value-=min;
  if(value==HUGE_VAL)
    {rgb[0]=rgb[1]=rgb[2]=255;}
  else if(value<0)
    {rgb[0]=rgb[1]=rgb[2]=0;}
  else if(value<max3)
    {rgb[0]=(unsigned char)(255*value/max3);rgb[1]=0;rgb[2]=0;}
  else if(value<2*max3)
    {rgb[0]=255;rgb[1]=(unsigned char)(255*(value-max3)/max3);rgb[2]=0;}
  else if(value<max)
    {rgb[0]=255;rgb[1]=255;rgb[2]=(unsigned char)(255*(value-2*max3)/max3);}
  else {rgb[0]=rgb[1]=rgb[2]=255;}
}

void coldColorMap(unsigned char *rgb,float value,float min,float max)
{
  float max3=(max-min)/3;
  value-=min;
  if(value==HUGE_VAL)
    {rgb[0]=rgb[1]=rgb[2]=255;}
  else if(value<0)
    {rgb[0]=rgb[1]=rgb[2]=0;}
  else if(value<max3)
    {rgb[0]=0;rgb[1]=0;rgb[2]=(unsigned char)(255*value/max3);}
  else if(value<2*max3)
    {rgb[0]=0;rgb[1]=(unsigned char)(255*(value-max3)/max3);rgb[2]=255;}
  else if(value<max)
    {rgb[0]=(unsigned char)(255*(value-2*max3)/max3);rgb[1]=255;rgb[2]=255;}
  else {rgb[0]=rgb[1]=rgb[2]=255;}
}

void blueColorMap(unsigned char *rgb,float value,float min,float max)
{
  value-=min;
  if(value==HUGE_VAL)
    {rgb[0]=rgb[1]=rgb[2]=255;}
  else if(value<0)
    {rgb[0]=rgb[1]=rgb[2]=0;}
  else if(value<max)
    {rgb[0]=0;rgb[1]=0;rgb[2]=(unsigned char)(255*value/max);}
  else {rgb[0]=rgb[1]=0;rgb[2]=255;}
}

void positiveColorMap(unsigned char *rgb,float value,float min,float max)
{
  value-=min;
  max-=min;
  value/=max;

  if(value<0){
  rgb[0]=rgb[1]=rgb[2]=0;
    return;
  }
  if(value>1){
  rgb[0]=rgb[1]=rgb[2]=255;
  return;
  }

  rgb[0]=192;rgb[1]=0;rgb[2]=0;
  rgb[0]+=(unsigned char)(63*value);
  rgb[1]+=(unsigned char)(255*value);
  if(value>0.5)
  rgb[2]+=(unsigned char)(255*2*(value-0.5));
}

void negativeColorMap(unsigned char *rgb,float value,float min,float max)
{
  value-=min;
  max-=min;
  value/=max;

  rgb[0]=0;rgb[1]=0;rgb[2]=0;
  if(value<0) return;
  if(value>1){
  rgb[1]=rgb[2]=255;
  return;
  }

  rgb[1]+=(unsigned char)(255*value);
  if(value>0.5)
  rgb[2]+=(unsigned char)(255*2*(value-0.5));

}

void colorMap(unsigned char *rgb,float value,float min,float max)
{
  if(value>0)
    positiveColorMap(rgb,value,0,max);
  else
    negativeColorMap(rgb,value,min,0);
/*
  if(value>0)
    hotColorMap(rgb,value,min,max);
  else
    coldColorMap(rgb,value,min,max);
        */
}

void cyclicColorMap(unsigned char *rgb,float value,float min,float max)
{
  float max3=(max-min)/3;
  value-=(max-min)*(float)floor((value-min)/(max-min));
  if(value<max3)
    {rgb[0]=(unsigned char)(255-255*value/max3);rgb[1]=0;rgb[2]=255-rgb[0];}
  else if(value<2*max3)
    {rgb[0]=0;rgb[1]=(unsigned char)(255*(value-max3)/max3);rgb[2]=255-rgb[1];}
  else if(value<max)
    {rgb[0]=(unsigned char)(255*(value-2*max3)/max3);rgb[1]=255-rgb[0];rgb[2]=0;}

}
void randColorMap(unsigned char *rgb,float value,float min,float max)
{
  srand((int)(65000*(value-min)/(max-min)));
  rgb[0]=(unsigned char)(255*rand());
  rgb[1]=(unsigned char)(255*rand());
  rgb[2]=(unsigned char)(255*rand());
}

void grayColorMap(unsigned char *rgb,float value,float min,float max)
{
  max-=min;
  value-=min;
  rgb[0]=rgb[1]=rgb[2]=(unsigned char)(255*value/max);
}

float max(float f, double d)
{
    if (f>d)
        return f;
    else
        return (float) d;
}




#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#define EPSILON 0.01

double deg2rad(double deg)
{
  return (deg * PI / 180);
}

double rad2deg(double rad)
{
  return (rad * 180 / PI);
}


//=====================================================================================
double toMollweide(double phi, double lambda, double &x, double &y)
{

        phi = deg2rad(phi);
        lambda = deg2rad(lambda);


        x = phi;
        y = lambda;

        const double eps = 1e-6; 	// how well we solve for theta
        const int nmax = 20;		// max number of iterations for Newton's method
        const double lambda0 = 0;	// offset
        // Need to solve 2theta + sin(2theta) == pi sin(phi), for theta
        double theta;
        // Upper limit
        if( fabs(phi - M_PI/2) < 1e-6) {
                theta = M_PI/2;
        }
        // Lower limit
        else if(fabs(phi + M_PI/2) < 1e-6 ) {
                theta = -M_PI/2;
        }
        else {
                double dtheta;
                double f,df;
                // Use Newton's method to solve
                // Initial guess
                theta = phi/2;
                // function for whose zero we solve for
                f = 2*theta + sin(2*theta) - M_PI*sin(phi);
                int i;
                for(i = 0; i < nmax; i++) {
                        // are we close enough to zero?
                        if( fabs(f) < eps)
                                break;
                        // derivation of f
                        df = 2*(1+cos(2*theta));
                        // Newton's method error
                        dtheta = -f/df;
                        // update guess
                        theta += dtheta;
                        // new value of function
                        f = 2*theta + sin(2*theta) - M_PI*sin(phi);
                }
        }
        // We now have the theta we need
        x = /*   2*sqrt(2.)*  */ (lambda- lambda0)*cos(theta)/M_PI;
        y = /*   sqrt(2.)*    */ sin(theta);



        return theta;
}


uint32_t ip_stdstring_to_uint32(std::string str)
{

    char*ip1 = strtok (&str[0],".");
    char*ip2 = strtok (NULL,".");
    char*ip3 = strtok (NULL,".");
    char*ip4 = strtok (NULL,".");

    //std::cout << ip1 << " " << ip2 << " " << ip3 << " " << ip4 << std::endl;

    uint32_t ip = atoi(ip1)<<24;
    ip+= atoi(ip2)<<16;
    ip+= atoi(ip3)<<8;
    ip+= atoi(ip4);

    //std::cout << ip << std::endl;

    return ip;
}



std::string ip_uint32_to_stdstring(uint32_t ip)
{

    char str[20];

    sprintf(str, "%d.%d.%d.%d", ((ip>>24) & 0xFF), ((ip>>16) & 0xFF), ((ip>>8) & 0xFF), ((ip) & 0xFF));
    std::string s = (str);

    return s;
}


float bound(float num, float min, float max)
{
    if (num<min)
        num = min;
    else if (num>max)
        num = max;

    return num;
}
