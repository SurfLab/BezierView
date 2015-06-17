#ifndef QUADBEZIER_H_2002_10_21
#define QUADBEZIER_H_2002_10_21



REAL* QuadBezier_get_bb(Patch*p,int i, int j);
void QuadBezier_loadFile(Patch*p,FILE* fp, bool equal_deg,
                         bool rational, bool art_normal);
int QuadBezier_create(Patch*p,int degu, int degv);
void QuadBezier_plot_patch(Patch*p,bool smooth);
void QuadBezier_evaluate_patch(Patch*p,int subDepth);
void QuadBezier_flip_normal(Patch*p);
void QuadBezier_plot_mesh(Patch*p,float* bg_color);
void QuadBezier_plot_crv(Patch*p,int crv_choice);
void QuadBezier_plot_crv_needles(Patch*p,int crv_choice, REAL length);
void QuadBezier_plot_highlights(Patch*p,VEC A, VEC H, REAL hl_step, int highlight_type);



#endif
