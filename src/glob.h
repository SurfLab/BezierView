#pragma once

// this is a copy of glob.c , with extern decalaration
// more detail explanation, refer to glob.c

// all important global variables


//  plot flags
enum DrawFlags {
    DRAWFLAGS_MESH= 1,
    DRAWFLAGS_PATCH=2 ,
    DRAWFLAGS_CRV=16 ,
    DRAWFLAGS_CRVNEEDLE=32,
    DRAWFLAGS_HIGHLIGHT=64 ,
    DRAWFLAGS_REFLLINE= 128,
    DRAWFLAGS_LIGHTING= 256,
    DRAWFLAGS_SMOOTH= 512,
    DRAWFLAGS_ENVMAPPING= 1024,
    DRAWFLAGS_HIDDENLINE= 2048,
    DRAWFLAGS_NORMAL= 4096,
    DRAWFLAGS_BOX = 4096*2,
};


#define COLORNUM 12


/////////////////////////////
// Data structure
#define MAXFACET 6500
#define MAXGROUP   6500
extern Patch face[MAXFACET];
extern Group group [MAXGROUP];
extern int   patch_num;
extern int   group_num;
extern int   has_patch;
extern int   has_polygon;

/////////////////////////////////////////////////////////////////////
// display options
extern int   g_Mode[MAXGROUP];
extern int   g_PenColor[MAXGROUP];
extern int   g_Material[MAXGROUP];
extern color_t g_patchColor[MAXGROUP];
extern double   g_LineWidth[MAXGROUP];
extern int   g_AntiAlias;

extern int   g_current_grp;
extern int   g_redisplay;
extern int   g_mouseMode;
extern int   g_substs[MAXGROUP];

extern int   F_Background;
extern double scale_factor;

// curvature bounds and curvature values
extern double max_crv[5];
extern double min_crv[5];

// current type of curvature
extern int   crv_choice;
extern int   crv_style;
extern real  hl_step;   // highlight density

extern int   firstPatch;

extern double ViewLeft, ViewRight, ViewTop, ViewBottom, ViewNear, ViewFar;
extern double ViewSize, ViewDepth;
extern double ClipNear, ClipFar;
extern double ViewCenter[2];
extern double ObjectCenter[3];
//extern double sizex, sizey, sizez;

extern int   winWidth;
extern int   winHeight;
extern int   background_scale;
extern int   texture1D_initialized;
extern int   texture_loaded;

extern double manual_low, manual_high;
extern int    manual_curvature_low, manual_curvature_high;
extern double curvature_ratio_a, curvature_ratio_b;
extern int    special_curv;



extern color_t g_BackColor[10];
extern char  g_BackColorNames[10][20];
extern color_t g_penColors[10];
extern char  g_penColorNames[10][20];
color_t       getMeshColor ();

extern int   back_choice;
extern int   normal_flipped;
extern int   cur_clipping_plane;
extern double needle_length;

extern int   use_display_list; // whether to use display list to draw the object
