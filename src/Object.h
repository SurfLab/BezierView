#ifndef OBJECT_H_2002_11_4
#define OBJECT_H_2002_11_4
#include "type.h"

/////////////////////////////////////////////////////////////////
//
//  class Object
//
class Object {

public:
	REAL minx, maxx, miny, maxy, minz, maxz; // axis aligned bounding box
	int init;
	int normal_flipped;
public:
// ------------------------------------------------------------
// constructor
// 
	Object () {
		init = 1;
		normal_flipped = 0;
	}
// Destructor
// 
	
public:
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
	
public:
    // the functions the objects of this class HAVE TO provide
	virtual void plot_patch(bool smooth) = 0;  // plot the patch
	virtual void plot_mesh(float* bg_color) = 0;             // plot the mesh

    // optional functions 
	virtual void plot_crv(int crv_choice)      {};     	  // plot the curvature 
	virtual void plot_highlights(VEC A, VEC H, REAL hl_step, int hl_type) {};     // plot the highlights 
	virtual void plot_crv_needles(int crv_choice, REAL length=1.0) {};   // plot the curvature needles
	virtual void flip_normal() {};     	  // flip the normal
	virtual void free_mem() {};     	  // free the memory
	virtual void evaluate_patch(int SubDepth) {};

	virtual void compute_crv() {};     	  // compute the curvature
};

#endif // OBJECT_H_2002_11_4
