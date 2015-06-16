#ifndef DRAW_H_2004_9_13
#define DRAW_H_2004_9_13
#include "bvglwidget.h"
void DisableMode(int grp_id, int flag);
bool isDisplayFlagEnabled(int grp_id, int flag);
void ToggleMode(int grp_id, int flag);
void set_g_redisplay();
void init_flags();



#endif
