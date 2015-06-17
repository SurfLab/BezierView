#ifndef POLYGON_H_2002_10_20
#define POLYGON_H_2002_10_20

#include "type.h"
#include "Bezier.h"



/////////////////////////////////////////////////////////////////
//  
//  class Polygon 
//      a polygonal patch
struct Polygon :public Bezier{

    void plot_mesh(float* bg_color);                      // plot the bezier patch
	void plot_patch(bool smooth);          // plot the control polygon
	void plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type);    // plot the highlight lines
	void flip_normal();
}; 



#endif
