#ifndef LIGHT_H
#define LIGHT_H

void set_color(int i);
void set_colorf(float r, float g, float b);
extern int   light_switch[3];
void	updateLights();
void light_init();
extern  char   mat_name[][20];


#endif // LIGHT_H

