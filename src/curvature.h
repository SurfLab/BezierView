#pragma once
extern int normal_clipping;

void 	 init_crv();

double   crv4(vector v00,vector v01,vector v02,vector v10,vector v20,vector v11,
				int degu, int degv, real* crv_result);

double   crv3(vector v00,vector v10,vector v20,vector v01,vector v02,vector v11,
				        int deg, real* crv_result);     

color_t   crv2color(double in);
void     evalPN(vector v00, vector v01, vector v10, vector P, vector N);
real     get_crv(real *crv_array, int loc, int crv_choice);

void	 minmax(real* curv, int choice, int num);
int     clickon_crv_bar(int x, int y, int winWidth, int winHeight);
void 	adjust_scale(int i, int winy, int winHeight);
void set_crv_scale(double lowc, double highc);
void draw_crv_bar(color_t color);  // the curvature bar
void draw_clipping_plane(int index, int mode, real  size);
void set_special_curvature(real curvature_ratio_a, real curvature_ratio_b);
void set_crv_bound_array(double* max_array, double* min_array);
void read_clipping(const char* filename) ;
int point_clipped(real* point);
