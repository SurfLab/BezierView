/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    glob.cpp
 *  Purpose: declare all the global variables
 * -------------------------------------------------------------
 */
#include "stdheaders.h"
#include "type.h"
#include "Patch.h"
#include "glob.h"

/////////////////////////////////////////////////////////////////////
// Main data structure 
Patch  face[MAXFACET];    // array of patches
int    patch_num;
Group  group [MAXGROUP];  // array of groups
int    group_num =0 ;

// some information about the object
int    has_patch   = 0;   // the object has smooth patch
int    has_polygon = 0;   // the object has polygon

/////////////////////////////////////////////////////////////////////
// display options
int   g_Mode[MAXGROUP];
int   g_PenColor[MAXGROUP];
int   g_Material[MAXGROUP];
int   g_LineWidth[MAXGROUP];
int   g_AntiAlias;
int   g_current_grp;
int   g_redisplay=1;
int   g_mouseMode;
int   g_substs[MAXGROUP];
float g_patchColor[MAXGROUP][3];

int    environmapping=0;
int    drawbox=0;

// some other display controls
float scale_factor =1.0; 
int    normal_flipped = 0;

// View volume (auto-decided by the patches, thus need an initial bound to start)
float  ViewLeft, ViewRight, ViewTop, ViewBottom, ViewNear, ViewFar;
float  ViewSize, ViewDepth=1;
float  ClipNear, ClipFar;
float  ViewCenter[2] = {0,0};
float  ObjectCenter[3];
int    winWidth=700, winHeight=700;

// environment mapping texture information
int    texture1D_initialized =0;
int    texture_loaded = 0;

// manual curvature bounds 
double manual_low, manual_high;
int    manual_curvature_low =0, manual_curvature_high=0;
// special curvature ratio 
double curvature_ratio_a, curvature_ratio_b;
int    special_curv =0;  // if there is a special curvature plot
double max_crv[5];
double min_crv[5];

// current type of curvature
int crv_choice;
REAL hl_step;   // highlight density

// the light switches
int    light_switch[3];

// the list of available background colors 
float  g_BackColor[10][3] 
          = { {1.0f, 1.0, 1.0},       // White
			{0.0, 0.0, 0.0},        // Black
			{0.4f, 0.4f, 0.4f},     // Gray 0.4
			{0.7f, 0.7f, 0.7f},     // Gray 0.7
			{0.74f, 0.74f, 0.96f},  // light blue 1
			{0.5, 0.5, 1.0},        // light blue 2
			{0.74f, 0.96f, 0.74f},  // light green 1
			{0.5, 1.0, 0.5},        // light green 2
			{1.0, 0.5, 1.0},        // light blue 2
			{1.0, 1.0, 0.5},        // light green
};

char   g_BackColorNames[10][20] = { "White", "Black", "Gray 0.4", "Gray 0.7", "light blue 1",
		"light blue 2", "light green 1", "light green 2", "purple", "yellow"};

// the list of available background colors 
float  g_penColors[10][3] 
          = { {1.0f, 1.0, 1.0},       // White
			{0.0, 0.0, 0.0},        // Black
			{0.4f, 0.4f, 0.4f},     // Gray 0.4
			{0.7f, 0.7f, 0.7f},     // Gray 0.7
			{0.74f, 0.74f, 0.96f},  // light blue 1
			{0.5, 0.5, 1.0},        // light blue 2
			{0.74f, 0.96f, 0.74f},  // light green 1
			{0.5, 1.0, 0.5},        // light green 2
			{1.0, 0.5, 1.0},        // light blue 2
			{1.0, 1.0, 0.5},        // light green
};

char   g_penColorNames[10][20] = { "White", "Black", "Gray 0.4", "Gray 0.7", "light blue 1",
		"light blue 2", "light green 1", "light green 2", "purple", "yellow"};

// choice of the background 
int    back_choice = 0;

// the available colors of the mesh 
float  g_MeshColor[2][3] = { {1.0, 1.0, 1.0}, {0.0, 0.0, 0.0} };

// automatically determine a B/W color 
float* getMeshColor () {
	float* back = g_BackColor[back_choice];
	if(back[0] + back[1] + back[2] < 1.5)  
	    return g_MeshColor[0];
	else
		return g_MeshColor[1];
}

float needle_length =1.0;
int   cur_clipping_plane = -1;
int   use_display_list = 1; // whether to use display list to draw the object

/* end of glob.cpp */
