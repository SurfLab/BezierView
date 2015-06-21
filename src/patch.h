#pragma once

typedef struct Patch{
    // the type of the patch, see type.h
    int type;
    int group_id;
    real location[3];

    /*
     * Degree u and v are equal for the triangular Bezier patch.
     * For quad patch, they are the tensor-product degrees
     */
    int degu, degv;
    int Ndegu, Ndegv;   // the degrees of the normal if applicable

    /// AABB should be struct
    real minx, maxx, miny, maxy, minz, maxz; // axis aligned bounding box
    /// For bounding box, we want to know if it was initialized
    int init;
    int normal_flipped;

    //! Number of control points for positions
    //! For polygon, it is the position of vertices around
    //! the polygon
    int pointCount;
    //! Number of normal control points of the patch
    int normalCount;
    real (*position)[DIM];
    real (*normal)[DIM];

    bool art_normal; // true: there is an artificial normal super-posed
                     //       on the patch
                     // false: the normal of the patch will be used


    bool evaluated; // a flag to show if the eval_* buffer is valid

    int  pts;       // evaluate density -- how many points are evaluated
                    // on each edge

    real (*eval_P)[DIM];   // result of the evaluated points
    real (*eval_N)[DIM];   // result of the evaluated normal

    real* crv_array; // result of the curvature

} Patch;

void Patch_init(Patch*);
void Patch_enlarge_AABB(Patch*p,int first);
void Patch_translate(Patch*p,real* center);
void Patch_evaluate(Patch* p, int SubDepth);
void Patch_plotcrv(Patch*p, int crv_choice);
void Patch_plotmesh(Patch*p, color_t bg_color);
void Patch_plotpatch(Patch*p, bool smooth);
void Patch_plotcrvneedles(Patch*p, int crv_choice, double needle_length);
void Patch_plothighlights(Patch*p, vector A, vector H, real hl_step, int hl_type);
void Patch_flipnormal(Patch*p);
void Patch_freeevalmem(Patch*p);


void Patch_createSinglePolygon(Patch*p, int side, real (*V)[DIM], int *F);
void Patch_loadQuadBezier(Patch*p, FILE* fp);
void Patch_loadTriBezier(Patch*p, FILE* fp);

// enlarge the bounding box if necessary to contain (x,y,z)
void Bezier_enlarge_aabb(Patch*p,real x, real y, real z);

/*!
 * \brief
 *   Group structure: attach faces to a group
 *    therefore can specify same color to many faces
 */
typedef struct Group
{   
    char name[255];  // name of the group
    int color_index; // color of the group
} Group;
