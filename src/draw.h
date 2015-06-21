#pragma once

void DisableMode(int grp_id, int flag);
bool isDisplayFlagEnabled(int grp_id, int flag);
void ToggleMode(int grp_id, int flag);
void set_g_redisplay();
void init_flags();
void draw();
void    load_position(int n);
void    save_position(int n);
void    updateProjection();
void    updateModelView();

void    define_crv();
void	export_eps();
void	export_igs();
void	flip_normal();
void    re_evaluate(int substeps);
