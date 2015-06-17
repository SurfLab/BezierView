#ifndef BEZIER_H_2002_10_20
#define BEZIER_H_2002_10_20
#include "type.h"

/////////////////////////////////////////////////////////////////
//
// abstract class Bezier
//
struct Bezier {

    /// AABB should be struct
    REAL minx, maxx, miny, maxy, minz, maxz; // axis aligned bounding box
    /// what is this init for?
    int init;
    /// this can just go into patch
    int normal_flipped;

    int pointCount;
    int normalCount;

    REAL (*position)[DIM];
    REAL (*normal)[DIM];
	
	bool art_normal; // true: there is an artificial normal super-posed
					 //       on the patch
					 // false: the normal of the patch will be used
	
	
	bool evaluated; // a flag to show if the eval_* buffer is valid
	
	int  pts;       // evaluate density -- how many points are evaluated
					// on each edge

    REAL (*eval_P)[DIM];   // result of the evaluated points
    REAL (*eval_N)[DIM];   // result of the evaluated normal

	REAL* crv_array; // result of the curvature


    Bezier() {
        init = 1;
        normal_flipped = 0;
        art_normal=false;
		evaluated = false;
        eval_P = eval_N = NULL;
        crv_array = NULL;
        position = normal = NULL;
	};

	void free_mem() {
		free_eval_mem();
        if(position) arrdelete(position);
        if(normal)  arrdelete(normal);
	}


	void free_eval_mem() {
        if(eval_P)    arrdelete(eval_P);
        if(eval_N)    arrdelete(eval_N);
        if(crv_array) arrdelete(crv_array);
		evaluated = false;
        eval_P = eval_N = NULL;
        crv_array = NULL;
	};



    // enlarge the bounding box if necessary to contain (x,y,z)
    void enlarge_aabb(REAL x, REAL y, REAL z) {
        if(init) {
            minx = maxx = x;
            miny = maxy = y;
            minz = maxz = z;
            init = 0;
        }
        else {
            if(x>maxx) maxx = x;
            if(x<minx) minx = x;
            if(y>maxy) maxy = y;
            if(y<miny) miny = y;
            if(z>maxz) maxz = z;
            if(z<minz) minz = z;
        }
    }

    // the functions the objects of this class HAVE TO provide
    virtual void plot_patch(bool smooth) = 0;  // plot the patch
    virtual void plot_mesh(float* bg_color) = 0;             // plot the mesh

    // optional functions
    virtual void plot_crv(int crv_choice)      {}     	  // plot the curvature
    virtual void plot_highlights(VEC A, VEC H, REAL hl_step, int hl_type) {}     // plot the highlights
    virtual void plot_crv_needles(int crv_choice, REAL length=1.0) {}   // plot the curvature needles
    virtual void flip_normal() {}   	  // flip the normal
    virtual void evaluate_patch(int SubDepth) {}

    virtual void compute_crv() {}    	  // compute the curvature

};
#endif
