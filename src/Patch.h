#ifndef PATCH_H_2002_10_20
#define PATCH_H_2002_10_20

typedef struct Object Object;

typedef struct Patch{

    int type;  // the type of the patch, see type.h

	Object*  object;

    int group_id;    // which group this face belongs to 
 //   int color;       // if the group has random color, record face color here

	REAL position[3];

} Patch;

int  Patch_loadFile(Patch* p,FILE* fp);
void Patch_enlarge_AABB(Patch*p,int first);
void Patch_translate(Patch*p,REAL* center);
void Patch_evaluate(Patch* p, int SubDepth);
void Patch_plotcrv(Patch*p, int crv_choice);
void Patch_plotmesh(Patch*p, float* bg_color);
void Patch_plotpatch(Patch*p, bool smooth);
void Patch_plotcrvneedles(Patch*p, int crv_choice, int needle_length);
void Patch_plothighlights(Patch*p, VEC A, VEC H, REAL hl_step, int hl_type);
void Patch_flipnormal(Patch*p);
void Patch_computecrv(Patch*p);
void Patch_freeevalmem(Patch*p);


void Patch_createSinglePolygon(Patch*p, int side, REAL (*V)[DIM], int *F);
void Patch_loadQuadBezier(Patch*p, FILE* fp);
void Patch_loadTriBezier(Patch*p, FILE* fp);

// Group structure: attach faces to a group 
//                    therefore can specify same color to many faces
typedef struct Group
{   
    char name[255];  // name of the group
    int color_index; // color of the group
//	int   subDepth;    // subdivision steps of the current group

} Group;



#endif
