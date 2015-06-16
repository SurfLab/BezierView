/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ. Of Florida
 *
 *  File:    util.c
 *  Purpose: utility functions dealing with vectors, allocating
 *           memory, and so on.
 *  Author   : Xiaobin Wu
 * -------------------------------------------------------------
 */
#include "stdheaders.h"
#include "type.h"
#include "util.h"


void VVcross(VEC v1, VEC v2, VEC v3)
{
    v3[0] = (v1[1])*(v2[2]) - (v1[2])*(v2[1]);
    v3[1] = (v1[2])*(v2[0]) - (v1[0])*(v2[2]);
    v3[2] = (v1[0])*(v2[1]) - (v1[1])*(v2[0]);

    if(DIM >3)
       v3[3] = 0.0;  // only vectors can be cross-producted
}/*VVcross*/

void VVminus(VEC v1,VEC v2, VEC v3)
{
    int i;
	if(DIM==3)
       for (i=0; i<DIM; i++)
          v3[i] = v1[i] - v2[i];
	else {
	   if(v1[3]==0 || v2[3] ==0) {
          for (i=0; i<DIM; i++)
             v3[i] = v1[i] - v2[i];
	   }
	   else {
	      for (i=0; i<3; i++)
             v3[i] = v1[i]/v1[3] - v2[i]/v2[3];
	      v3[3] = 0.0;  // vector
	   }
	}
}/*minus*/


double VVmult(VEC v1, VEC v2)
{
    double res;
    int i;

    res = 0;
    for (i=0; i<3; i++)
        res += v1[i]*v2[i];
    return(res);
}/*VVmult*/

void Vcopy(VEC from, VEC to)
{
    int i;
    for (i=0; i<DIM; i++)
    {
        double h = from[i];   // for debugging
		//printf("h=%f\n", h);
        to[i] = h;
    }
}

/* v3 <- m1*v1 + m2*v2 */
void VVadd( double m1,VEC v1,double m2,VEC v2,VEC v3)
{
    int k;
    for (k=0; k<DIM; k++) v3[k] = m1*v1[k]+m2*v2[k];
}

void VVscale(double m, VEC v, VEC result)
{
    int k;
    for (k=0; k<DIM; k++) result[k] = m*v[k];
}

void VVaddto(double m, VEC v, VEC result)
{
    int k;
    for (k=0; k<DIM; k++) result[k] += m*v[k];
}

void   VVzero(VEC v)
{
	int k;
    for (k=0; k<DIM; k++) v[k] =0;
//	if(DIM>3) v[3] =1;
}

/* ------------------------------------------- 
 * Normalize -- Normalize the input vector
 * Parameters:
 *  v = v/||v||
 *
 */
double Normalize(VEC v)
{
    REAL m;
    int i;
    m  = v[0]*v[0] + v[1]* v[1] + v[2]*v[2];
    m  = sqrt(m);
	if(m!=0)
        for (i=0;i<3;i++) v[i] = v[i]/m;
	return m;
}
double Norm(VEC v)
{
		    return sqrt(v[0]*v[0] + v[1]* v[1] + v[2]*v[2]);
}
// v2 = v1/ ||v1||
double Vnorm(VEC v1, VEC v2)
{
    REAL m;
    int i;
    m  = v1[0]*v1[0] + v1[1]* v1[1] + v1[2]*v1[2];
    m  = sqrt(m);
    //if(m==0) printf("Warning: divided by zero\n");
	if (m==0)  // avoid divide by zero
        for (i=0;i<3;i++) v2[i] = v1[i];
	else
        for (i=0;i<3;i++) v2[i] = v1[i]/m;
    return m;
}

void printV (VEC v)
{
   int i;
   for (i =0; i<DIM; i++)
        printf ("%f, ", v[i]);
   printf("\n");
}

/*
REAL Abs(REAL a)
{
    return (a>0) ? a:(-a);
}
REAL fabs(REAL a)
{
    return (a>0) ? a:(-a);
}
*/

// determinent for a 4x4 matrix
double det4( double x11, double x12, double x13, double x14,
        double x21, double x22, double x23, double x24,
        double x31, double x32, double x33, double x34,
        double x41, double x42, double x43, double x44)
{
     double t0 = x11*x22*x33*x44-x11*x22*x34*x43-
                x11*x32*x23*x44+x11*x32*x24*x43+x11*x42*x23*x34-
                x11*x42*x24*x33-x21*x12*x33*x44+x21*x12*x34*x43+
                x21*x32*x13*x44-x21*x32*x14*x43-x21*x42*x13*x34+
                x21*x42*x14*x33+x31*x12*x23*x44-x31*x12*x24*x43-
                x31*x22*x13*x44+x31*x22*x14*x43+x31*x42*x13*x24-
                x31*x42*x14*x23-x41*x12*x23*x34+x41*x12*x24*x33+
                x41*x22*x13*x34-x41*x22*x14*x33-x41*x32*x13*x24+
                x41*x32*x14*x23;
    return t0;
}

// determinent for a 3x3 matrix
double det3( double x11, double x12, double x13,
        double x21, double x22, double x23,
        double x31, double x32, double x33)
{
    double t0 = x11*x22*x33-x11*x23*x32-x12*x21*x33+x12*x23*x31+
                x13*x21*x32-x13*x22*x31;
    return t0;
}
  

// memeory allocation functions 
REAL* alloc_mem_db(int size)
{
    REAL* add;
    if( (add = (REAL*) malloc (size*sizeof(REAL))) == NULL)
    {
        printf("not enough memory\n");
        exit(1);
    }
    return add;
}

int* alloc_mem_int(int size)
{
    int* add;
    if( (add = (int*) malloc (size*sizeof(int))) == NULL)
    {
        printf("not enough memory\n");
        exit(1);
    }
    return add;
}

int* realloc_mem_int(int* old, int oldsize, int size)
{
    int* add;
    if( (add = (int*) malloc (size*sizeof(int))) == NULL)
    {
        printf("not enough memory\n");
        exit(1);
    }
	memcpy(add, old, oldsize *sizeof(int));
    return add;
}


void *allocate(long size)
{
    void *p;
    if( (p= malloc(size)) != NULL)
        return p;
    else
    {
        printf("out of memory!\n");
        exit(0);
    }
}




//////////////////////////////////////////////////////////
//
REAL VVdist(VEC v1, VEC v2)
{
	VEC d;
	int m;
	for(m=0;m<DIM;m++) d[m] = v1[m] - v2[m];
	return Norm(d);
}

