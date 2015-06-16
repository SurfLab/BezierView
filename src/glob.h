#ifndef GLOB_H_2002_10_20
#define GLOB_H_2002_10_20

// this is a copy of glob.c , with extern decalaration
// more detail explanation, refer to glob.c

// all important global variables

#ifndef COLORNUM
#define COLORNUM 12
#endif

//  plot flags
#define DRAWMESH       1
#define DRAWPATCH      2 
#define DRAWPOLYMESH   4 
#define DRAWPOLYPATCH  8 
#define DRAWCRV        16 
#define DRAWCRVNEEDLE  32 
#define DRAWHIGHLIGHT  64 
#define DRAWREFLLINE   128
#define LIGHTING       256
#define SMOOTH         512
#define ENVMAPPING     1024
#define HIDDENLINE     2048
#define NORMAL         4096

extern int drawbox;


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
extern float g_patchColor[MAXGROUP][3];
extern int   g_LineWidth[MAXGROUP];
extern int   g_AntiAlias;

extern int   g_current_grp;
extern int   g_redisplay;
extern int   g_mouseMode;
extern int   g_substs[MAXGROUP];

extern int   F_Background;
extern float scale_factor;

// curvature bounds and curvature values
extern double max_crv[5];
extern double min_crv[5];

// current type of curvature
extern int   crv_choice;
extern int   crv_style;
extern REAL  hl_step;   // highlight density

extern int   firstPatch;

extern float ViewLeft, ViewRight, ViewTop, ViewBottom, ViewNear, ViewFar;
extern float ViewSize, ViewDepth;
extern float ClipNear, ClipFar;
extern float ViewCenter[2];
extern float ObjectCenter[3];
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



extern float g_BackColor[10][3];
extern char  g_BackColorNames[10][20];
extern float g_penColors[10][3];
extern char  g_penColorNames[10][20];
float*       getMeshColor ();

extern int   back_choice;
extern int   normal_flipped;
extern int   cur_clipping_plane;
extern float needle_length;

extern int   use_display_list; // whether to use display list to draw the object

#endif
