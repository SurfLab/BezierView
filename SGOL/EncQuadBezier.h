#ifndef ENCQUADBEZIER_H_2002_10_26
#define ENCQUADBEZIER_H_2002_10_26
#include "QuadBezier.h"
#include "util.h"

/////////////////////////////////////////////////////////////////
//
// class EncQuadBezier
//
// Description: Enclosed Quadrilatrel Bezier patch
//
//
class EncQuadBezier : public QuadBezier {

private:
	double* o_enc;  // outter enclosure of this patch
	double* i_enc;  // inner enclosure of this patch
	int* cralong;   // crease along direction at each quad of enclosure

#define segu  3        // number of segments on u
#define segv  3        // number of segments on v
#define d1    (segu+1)

	VEC sup_pt[d1][d1];   // support points 
	VEC sup_nor[d1][d1];   // support normal 
	double lambda[d1][d1][2];   // offset distance

	bool enc_computed;
public:
// ------------------------------------------------------------
// constructor
// 
	EncQuadBezier () { 
        QuadBezier();
        enc_computed = false;
    }

private:
	REAL* get_enc(int dir, int i, int j) {
        if(dir ==0) return &i_enc[(i*(segv+1)+j)*DIM];
            else  return &o_enc[(i*(segv+1)+j)*DIM];
	}; 


	// 4 steps to compute surface enclosure
	void make_env();
	void make_sup();
	int  make_lam ();
	void make_tri();

	// this function calls the above 4 functions
	void compute_enclosure();

public:
	void plot_enc_patch(int dir);
	void plot_enc_mesh(int dir);
	
	void plot_patch() {plot_enc_patch(1);};
	void plot_ctrl() {plot_enc_mesh(1);};

	void outputtofile(FILE* fpw);
};

#endif
