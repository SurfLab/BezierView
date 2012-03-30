#ifndef BEZIER_H_2002_10_20
#define BEZIER_H_2002_10_20
#include <stdlib.h>
#include "type.h"
#include "Object.h"

/////////////////////////////////////////////////////////////////
//
// abstract class Bezier
//
class Bezier : public Object{

protected:
	int num_points;

public:
	REAL* coeff; // the coefficients of the Bezier patch
				 // &coeff[0*DIM] is the pointer to the first coefficient
				 // &coeff[1*DIM] is the pointer to the second coefficient
				 // and so on
	
	bool art_normal; // true: there is an artificial normal super-posed
					 //       on the patch
					 // false: the normal of the patch will be used
	
	
	int num_normals;  
	REAL* norm;      // the coefficients of the normal

	// evaluated patch

public:
	bool evaluated; // a flag to show if the eval_* buffer is valid
	
	int  pts;       // evaluate density -- how many points are evaluated
					// on each edge

	REAL* eval_P;   // result of the evaluated points
	REAL* eval_N;   // result of the evaluated normal

	REAL* crv_array; // result of the curvature


public:
// ------------------------------------------------------------
// constructor
// 
	Bezier() {art_normal=false; 
		evaluated = false;
		eval_P = eval_N = crv_array = NULL;
		coeff = norm = NULL;
	};

	void free_mem() {
		free_eval_mem();
		if(coeff) free(coeff);
		if(norm)  free(norm);
	}


	void free_eval_mem() {
		if(eval_P)    free(eval_P);
		if(eval_N)    free(eval_N);
		if(crv_array) free(crv_array);
		evaluated = false;
		eval_P = eval_N = crv_array = NULL;
	};
};
#endif
