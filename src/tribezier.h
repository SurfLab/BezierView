#ifndef TRIBEZIER_H_2002_10_20
#define TRIBEZIER_H_2002_10_20

int TriBezier_loadFile(Patch*p,FILE* fp, bool art_normal);
void TriBezier_plot_patch(Patch*p,bool smooth);
void TriBezier_plot_mesh(Patch*p, float *bg_color);
void TriBezier_evaluate_patch(Patch*p,int subDepth);
void TriBezier_flip_normal(Patch*p);
void TriBezier_plot_crv(Patch*p,int crv_choice);
void TriBezier_plot_crv_needles(Patch*p,int crv_choice, REAL length);
void TriBezier_plot_highlights(Patch*p,VEC A, VEC H, REAL hl_step, int highlight_type);
REAL* TriBezier_get_bb(Patch*p,int i, int j);

#endif
