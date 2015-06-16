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
    for(int i=0;i<3;i++) p->position[i] = 0;
    // new patch
    TriBezier  * newtri;
    QuadBezier * newquad;
    PolygonMesh    * newpoly;

	// read in the 
    switch(p->type) {

    case POLY:
        newpoly = new PolygonMesh();
	    newpoly->loadFileBV(fp);
        p->object = newpoly;
	    break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
		newquad = new QuadBezier();
        newquad -> loadFile(fp, p->type==TP_EQ, p->type==RATIONAL, p->type==PNTP);
        p->object = newquad;
        break;
    case TRIANG:
    case PNTRI:
		newtri = new TriBezier();
        newtri -> loadFile(fp, p->type==PNTRI);
        p->object = newtri;
		break;
    default:
        fprintf(stderr,"Unknown patch_kind %d\n", p->type);
        exit(1);
    }

	return 0;
};


// translate the patch so it is centered at the point 'center'
void Patch_translate(Patch*p,REAL* center)
{
	int i;
	for(i=0;i<3;i++)
        p->position[i] += center[i];
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
