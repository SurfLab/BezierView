/* ------------------------------------------------------------
 *  BezierView
 *    A simple viewer for Bezier patches
 *    SurfLab, Univ Of Florida
 *
 *  File:     Patch.cpp
 *  Purpose:  
 *          1. Load the patch from the input file
 *          2. Translate the patch if needed
 *
 */
#include "stdheaders.h"

extern "C" {
#include "util.h"
#include "Patch.h"
#include "glob.h"
}
#include "Object.h"
#include "TriBezier.h"
#include "Polygon.h"
#include "QuadBezier.h"

void Patch_evaluate(Patch* p, int SubDepth)
{
    p->object->evaluate_patch(SubDepth);
}
////////////////////////////////////////////////////////////////
//
//  load the Bezier data from a data file
//
int Patch_loadFile(Patch*p,FILE* fp) {

	return 0;
};


// translate the patch so it is centered at the point 'center'
void Patch_translate(Patch*p,REAL* center)
{
	int i;
	for(i=0;i<3;i++)
        p->position[i] += center[i];
}


void Patch_createSinglePolygon(Patch*p, int side, REAL (*V)[DIM], int *F)
{
    PolygonMesh    * newpoly = new PolygonMesh();
    newpoly->VNum = side;
    newpoly->FNum = 1;
    arrcreate(newpoly->vertices, side);
    arrcreate(newpoly->faces, 1);
    Vertex* vertices = newpoly->vertices;
    Facet * faces = newpoly->faces;
    for(int i = 0; i < side; i++)
    {
        REAL *v = V[F[i]];
        newpoly->enlarge_aabb(v[0],v[1],v[2]);
        vertices[i].set_p(v[0],v[1],v[2]);
        vertices[i].set_n(0,0,0);
        vertices[i].valid = true;
    }
    faces[0].sides = side;
    faces[0].initMem();
    for(int i = 0; i < side; i++)
        faces[0].V_ind[i] = i;
    /* Calculate the normal by averaging side normals */
    for(int i= 0; i < side; i++)
    {
        int prv = i, pt = (i+1)%side, nxt = (i+2)%side;
        REAL V1[DIM], V2[DIM];
        VVminus(vertices[prv].p, vertices[pt].p, V1);
        VVminus(vertices[nxt].p, vertices[pt].p, V2);
        VVcross(V2, V1, vertices[pt].n);
        Normalize(vertices[pt].n);
        VVadd(1.0, faces[0].normal, 1.0, vertices[pt].n, faces[0].normal);
    }
    Normalize(faces[0].normal);


    p->object = newpoly;
}



void Patch_loadQuadBezier(Patch*p, FILE* fp)
{
    QuadBezier * newquad;
    newquad = new QuadBezier();
    newquad -> loadFile(fp, p->type==TP_EQ, p->type==RATIONAL, p->type==PNTP);
    p->object = newquad;
}

void Patch_loadTriBezier(Patch*p, FILE* fp)
{
    TriBezier  * newtri;
    newtri = new TriBezier();
    newtri -> loadFile(fp, p->type==PNTRI);
    p->object = newtri;
}

void Patch_enlarge_AABB(Patch*p,int first) {

	if (first)  // first patch
	{
        ViewLeft  = (float)p->object->minx;
        ViewRight = (float)p->object->maxx;

        ViewBottom= (float)p->object->miny;
        ViewTop   = (float)p->object->maxy;

        ViewFar   = (float)p->object->minz;
        ViewNear  = (float)p->object->maxz;
	}
	else {
        if(ViewLeft  > p->object->minx) ViewLeft  = (float)p->object->minx;
        if(ViewRight < p->object->maxx) ViewRight = (float)p->object->maxx;

        if(ViewBottom> p->object->miny) ViewBottom= (float)p->object->miny;
        if(ViewTop   < p->object->maxy) ViewTop   = (float)p->object->maxy;

        if(ViewFar   > p->object->minz) ViewFar   = (float)p->object->minz;
        if(ViewNear  < p->object->maxz) ViewNear  = (float)p->object->maxz;
	}
}



void Patch_plotcrv(Patch *p, int crv_choice)
{
    p->object->plot_crv(crv_choice);
}



void Patch_plotmesh(Patch *p, float *bg_color)
{
    p->object->plot_mesh(bg_color);
}


void Patch_plotpatch(Patch *p, bool smooth)
{
    p->object->plot_patch(smooth);
}


void Patch_plotcrvneedles(Patch *p, int crv_choice, int needle_length)
{
    p->object->plot_crv_needles(crv_choice, needle_length);
}


void Patch_plothighlights(Patch *p, VEC A, VEC H, REAL hl_step, int hl_type)
{
    p->object->plot_highlights(A,H,hl_step,hl_type);
}


void Patch_flipnormal(Patch *p)
{
    p->object->flip_normal();
}


void Patch_computecrv(Patch *p)
{
    p->object->compute_crv();
}


void Patch_freeevalmem(Patch *p)
{
    ((Bezier*) p->object)->free_eval_mem();
}
