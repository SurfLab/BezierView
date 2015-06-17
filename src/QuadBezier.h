#ifndef QUADBEZIER_H_2002_10_21
#define QUADBEZIER_H_2002_10_21

#include "type.h"
#include "Bezier.h"

/////////////////////////////////////////////////////////////////
// 
// class QuadBezier
//         A quadrilateral bezier patch
struct QuadBezier : public Bezier {
	void evaluate_patch(int subDepth);
	REAL* get_v(int side);
    QuadBezier(){evaluated = 0;}
    ~QuadBezier(){
        if(evaluated) {
            arrdelete(eval_P);arrdelete(eval_N);arrdelete(crv_array);
        }
    }

    // ploting routines
	void plot_patch(bool smooth);  // plot the quadrilateral patch
	void plot_mesh(float* bg_color);   // plot the control polygon of the quadrilateral patch
	void plot_crv(int crv_choice);   // plot the curvature of the quadrilateral patch
	void plot_crv_needles(int crv_choice, REAL length=1.0);   // plot the curvature of the quadrilateral patch
	void plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type);  // plot the highlight lines for the patch
	void flip_normal();     	  // flip the normal

	REAL* get_bb(int i, int j) const { 
         return &position[(i*(degv+1)+j)][0];
    }
	
	int loadFile(FILE* fp, bool equal_deg, bool rational, bool art_normal);  
	int create(int degu, int degv);  // directly construct the memory
};

#endif
