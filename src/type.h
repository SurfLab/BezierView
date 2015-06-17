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

typedef struct Color {
    float red, green, blue, alpha;
} color_t;

inline void glColorc(color_t c)
{
    glColor4f(c.red, c.green, c.blue, c.alpha);
}

inline color_t mkcolor4(float red, float green, float blue, float alpha)
{
    color_t c = { red, green, blue, alpha };
    return c;
}
inline color_t mkcolor3(float red, float green, float blue)
{
    return mkcolor4(red, green, blue, 1.0f);
}

inline color_t mknullcolor()
{
    color_t c = { 0.0f, 0.0f, 0.0f, 0.0f };
    return c;
}


inline color_t interp(double u, color_t low, color_t high)
{
    float v = (float)u;
    color_t c;
    c.red = (1-v)*low.red + v * high.red;
    c.green = (1-v) *low.green + v * high.green;
    c.blue = (1-v)*low.blue + v * high.blue;
    c.alpha = (1-v)*low.alpha + v * high.alpha;
    return c;
}

#endif
