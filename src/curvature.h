#ifndef CURVATURE_H_2004_09_11
#define CURVATURE_H_2004_09_11
extern int normal_clipping;

void 	 init_crv();

double   crv4(VEC v00,VEC v01,VEC v02,VEC v10,VEC v20,VEC v11,
				int degu, int degv, REAL* crv_result);

double   crv3(VEC v00,VEC v10,VEC v20,VEC v01,VEC v02,VEC v11,
				        int deg, REAL* crv_result);     

float*   crv2color(double in);
void     evalPN(VEC v00, VEC v01, VEC v10, VEC P, VEC N);
REAL     get_crv(REAL *crv_array, int loc, int crv_choice);

void	 minmax(REAL* curv, int choice, int num);
int     clickon_crv_bar(int x, int y, int winWidth, int winHeight);
void 	adjust_scale(int i, int winy, int winHeight);
void set_crv_scale(double lowc, double highc);
void draw_crv_bar(GLfloat* color);  // the curvature bar
void draw_clipping_plane(int index, int mode, REAL  size);
void set_special_curvature(REAL curvature_ratio_a, REAL curvature_ratio_b);
void set_crv_bound_array(double* max_array, double* min_array);
void read_clipping(const char* filename) ;
int point_clipped(REAL* point);
#endif
