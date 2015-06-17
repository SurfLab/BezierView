#ifndef TRIBEZIER_H_2002_10_20
#define TRIBEZIER_H_2002_10_20

#include "type.h"
#include "Bezier.h"

/////////////////////////////////////////////////////////////////
// 
// class TriBezier
//         A triangular bezier patch
class TriBezier : public Bezier {
public:
	int deg;    // degree of this triangular patch
	int Ndeg;   // the degree of the normal if applicable

	//int fc;     // a facet counter(id) used for some applications
				// to identify this patch
	
//---------------------------------------------------------------
// methods
public:
	void evaluate_patch(int use_art_normal = true);

public:
    TriBezier(int degree);;

	TriBezier() {evaluated = false;};

	void plot_patch(bool smooth);  // plot the triangular patch
	void plot_mesh(float* bg_color);   // plot the control polygon of the triangular patch
	void plot_crv(int crv_choice);    // plot the curvature of the triangular patch

	// plot the curvature needle of the triangular patch
	void plot_crv_needles(int crv_choice, REAL length); 
	void plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type);  // plot the highlight lines for the patch
	void flip_normal();     	  // flip the normal

	int get_deg() {return deg;};

	REAL* get_bb(int i, int j);  // return the control point at grid (i,j, d-i,j)
	
	// load the data from the file
	int loadFile(FILE* fp, bool art_normal = false);  
};

// index mapping functions
int b2i_j(int i, int j, int k, int d);
int b2i_i(int i, int j, int k, int d);
int b2i_k(int i, int j, int k, int d);

#endif
