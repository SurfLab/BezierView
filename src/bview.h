#ifndef BVIEW_H
#define BVIEW_H
#include "glob.h"
extern char dataFileName[500];
extern char programDir[500];

void project_init();
void modelview_init();
void init_flags();

// opengl initialize             -- later in this file
void windowinit(int argc, char *argv[]);
void light_init();
void menu_init();
void parse_arg(int argc, char* argv[]);
void readin_curv_bounds();
void read_clipping(const char* filename) ; // in SGOL
void define_scene(FILE* fp);
void printkeys();



// glut callbacks                 -- in rotate.cpp
void display();
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void spin();
void advkeyboard(int key, int x, int y);

void initGL();
void init_bezierview(int argc, char* argv[]);

#endif // BVIEW_H