#pragma once

void Polygon_flip_normal(Patch*p);
void Polygon_plot_patch(Patch*p,bool smooth);
void Polygon_plot_mesh(Patch*p, color_t bg_color);
void Polygon_plot_highlights(Patch*p,vector A, vector H, real hl_step, int highlight_type);
