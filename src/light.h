#ifndef LIGHT_H
#define LIGHT_H

void set_color(int i);
void set_colorf(color_t c);
extern int   light_switch[3];
void	updateLights();
void light_init();
extern  char   mat_name[][20];


#endif // LIGHT_H

