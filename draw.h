#ifndef DRAW_H_2004_9_13
#define DRAW_H_2004_9_13

void DisableMode(int grp_id, int flag);
bool isEnabled(int grp_id, int flag);
void ToggleMode(int grp_id, int flag);

void init_flags();

#endif