#pragma once
// primitive data type
typedef double real;
/* Dimension */
#define DIM 4
typedef real vector[DIM];

/* patch type definitions */
enum PatchType {
     POLY   =1,        /* polyhedron */
     TRIANG =3,        /* triangular patch */
     TP_EQ  =4,        /* tensorproduct with same degree in both dir. */
     TP     =5,        /* general tensorproduct */
     TRIM_CURVE =6,
     TP_BSP =7,        /* general b-spline tensorproduct */
     RATIONAL =8,
     PNTRI    =9,      /* PN triangle patch, containing points and normals */
     PNTP    =10,      /* PN quads patch, containing points and normals */
};

enum CurvatureType {
// Curvature types 
     GAUSS_CRV  =0,      // Gaussian curvature
     MEAN_CRV   =1,      // mean curvature
     MAX_CRV    =2,      // maximum curvature
     MIN_CRV    =3,      // minimum curvature
     SPECIAL_CRV=4      // special curvature
};

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
