#ifndef BVIEW_H
#define BVIEW_H

extern char dataFileName[500];
extern char programDir[500];


// opengl initialize             -- later in this file
void windowinit(int argc, char *argv[]);
void menu_init();
void parse_arg(int argc, char* argv[]);
void readin_curv_bounds();
void define_scene(FILE* fp);
void printkeys();




void initGL();
void init_bezierview(int argc, char* argv[]);
void loadDataFile(const char* fn);

#endif // BVIEW_H
