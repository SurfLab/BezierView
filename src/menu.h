#pragma once

// menu controls 

enum MouseModeEnum {
    MOUSEMODE_ZOOM = 1,
    MOUSEMODE_ROTATE = 2,
    MOUSEMODE_MOVE = 3,
    MOUSEMODE_CLIPNEAR = 4,
    MOUSEMODE_CLIPFAR = 5,
};

enum MenuControlEnum {
    MENUCONTROL_ZOOMIN        =81,
    MENUCONTROL_ZOOMOUT       =82,
    MENUCONTROL_QUIT          =83,
    MENUCONTROL_RESET_PROJECTION=84,
    MENUCONTROL_FLIPNORMAL    =86,
    MENUCONTROL_MOUSEMODE     =55,
    MENUCONTROL_COLOR          =1,
    MENUCONTROL_DISPLAY        =2,
    MENUCONTROL_INHLDENSE     =32,
    MENUCONTROL_DEHLDENSE     =33,
    MENUCONTROL_SAVE           =3,
    MENUCONTROL_LOAD           =4,
    MENUCONTROL_CLIPSTOP      =26,
    MENUCONTROL_LIGHT         =25,
    MENUCONTROL_CRVSTYLE       =6,
    MENUCONTROL_BLACKBACK      =7,
    MENUCONTROL_BLACKPEN       =8,
    MENUCONTROL_SUBST          =9,
    MENUCONTROL_LINEWIDTH     =10,
    MENUCONTROL_ANTIALIAS     =51,
    MENUCONTROL_USELISTS      =52,
    MENUCONTROL_GROUP         =12,
    MENUCONTROL_CURVATURE_TYPE=14,
};


// menu handling procedures

void menu_proc(int entry, int parameter);
void color_proc_rgb(color_t c);
void keyboard(unsigned char key);
void advkeyboard(int key);

