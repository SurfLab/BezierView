#ifndef TYPE_H_2002_10_20
#define TYPE_H_2002_10_20

// primitive data type
#ifndef REAL
typedef double REAL;
#endif

/* Dimension */
#ifndef DIM
#define DIM 4
#endif

typedef REAL VEC[DIM];

/* patch type definitions */
#define POLY   1        /* polyhedron */
#define TRIANG 3        /* triangular patch */
#define TP_EQ  4        /* tensorproduct with same degree in both dir. */
#define TP     5        /* general tensorproduct */
#define TRIM_CURVE 6
#define TP_BSP 7        /* general b-spline tensorproduct */
#define RATIONAL 8

// added Feb. 2002
#define PNTRI    9      /* PN triangle patch, containing points and normals */
#define PNTP    10      /* PN quads patch, containing points and normals */

// Curvature types 
#define GAUSS_CRV  0      // Gaussian curvature
#define MEAN_CRV   1      // mean curvature
#define MAX_CRV    2      // maximum curvature
#define MIN_CRV    3      // minimum curvature
#define SPECIAL_CRV 4      // special curvature

#define MAXDEG 40     // maximum degree of bezier patches

#endif
