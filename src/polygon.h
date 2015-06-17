#ifndef POLYGON_H_2002_10_20
#define POLYGON_H_2002_10_20

#include "type.h"
#include "patch.h"


void Polygon_flip_normal(Patch*p);
void Polygon_plot_patch(Patch*p,bool smooth);
void Polygon_plot_mesh(Patch*p, float *bg_color);
void Polygon_plot_highlights(Patch*p,VEC A, VEC H, REAL hl_step, int highlight_type);



#endif
