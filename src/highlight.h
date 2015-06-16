#ifndef HIGHTLIGHT_H_2004_09_11
#define HIGHTLIGHT_H_2004_09_11


// in highlight.c ---
void Highlight(int n, REAL* P, REAL* N, VEC A, VEC H, REAL hl_step, int highlight_type);
void Solve4(double * A, double* x);
void init_texture(GLubyte *forecolor, GLubyte *backcolor);

#endif
