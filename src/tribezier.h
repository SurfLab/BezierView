#pragma once

int TriBezier_loadFile(Patch*p,FILE* fp, bool art_normal);
void TriBezier_plot_patch(Patch*p,bool smooth);
void TriBezier_plot_mesh(Patch*p, color_t bg_color);
void TriBezier_evaluate_patch(Patch*p,int subDepth);
void TriBezier_flip_normal(Patch*p);
void TriBezier_plot_crv(Patch*p,int crv_choice);
void TriBezier_plot_crv_needles(Patch*p,int crv_choice, real length);
void TriBezier_plot_highlights(Patch*p,vector A, vector H, real hl_step, int highlight_type);
real* TriBezier_get_bb(Patch*p,int i, int j);
