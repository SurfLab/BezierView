#ifndef UTIL_H_
#define UTIL_H_

#include "type.h"

#ifndef DIM
#define DIM 4
#endif

#define tol  0.0000001

void   VVcross(VEC v1, VEC v2, VEC v3);  // v3 = v1 X v2
void   VVminus(VEC v1,VEC v2, VEC v3);   // v3 = v1 - v2
double VVmult(VEC v1, VEC v2);         // return v1 dot v2
void   Vcopy(VEC from, VEC to);            // v2 = v1
void   VVadd( double m1,VEC v1,double m2,VEC v2,VEC v3);
                                       // v3 = m1*v1 + v2*v2
void   VVscale(double m, VEC v, VEC result);
void   VVaddto(double m, VEC v, VEC result);
void   VVzero(VEC v);

REAL   VVdist(VEC v1, VEC v2);


///////////////////////////////////////////////////
// To use Pov-ray's vector commands
inline void Assign_Vector(REAL* to, REAL* from) {
	Vcopy(from, to);
}

// v2 = v1/ || v1 ||
double  Vnorm(VEC v1, VEC v2);

// v = v/||v||
double  Normalize(REAL v[]);
double  Norm(REAL v[]); // return  ||v||
void    printV (VEC v);

//REAL Abs(REAL a);
//REAL fabs(REAL a);

// determine for 3*3 matrix, 4*4 matrix
double det3( double x11, double x12, double x13,
        double x21, double x22, double x23,
        double x31, double x32, double x33);
double det4( double x11, double x12, double x13, double x14,
        double x21, double x22, double x23, double x24,
        double x31, double x32, double x33, double x34,
        double x41, double x42, double x43, double x44);

/* memory allocation methods */
double* alloc_mem_db(int size);
int*    alloc_mem_int(int size);
void*   allocate(long size);
int*    realloc_mem_int(int* old, int oldsize, int size);
void glDrawText(float x, float y, float z, const char * str);
void log_error(const char* context, const char* reason);

/* Array allocation routines */
#define arrcreate(pointer,count) (pointer =  malloc(sizeof(*pointer)*count))
#define arrresize(pointer,count) (pointer =  realloc(pointer, sizeof(*pointer)*count))
#define arrdelete(pointer) (free(pointer))



#endif

