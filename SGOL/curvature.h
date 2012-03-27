#ifndef CURVATURE_H_2004_09_11
#define CURVATURE_H_2004_09_11

void 	 init_crv();

double   crv4(VEC v00,VEC v01,VEC v02,VEC v10,VEC v20,VEC v11,
				int degu, int degv, REAL* crv_result);

double   crv3(VEC v00,VEC v10,VEC v20,VEC v01,VEC v02,VEC v11,
				        int deg, REAL* crv_result);     

float*   crv2color(double in);
void     evalPN(VEC v00, VEC v01, VEC v10, VEC P, VEC N);
REAL     get_crv(REAL *crv_array, int loc, int crv_choice);

void	 minmax(REAL* curv, int choice, int num);

#endif
