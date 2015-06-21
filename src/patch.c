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
#include "util.h"
#include "patch.h"
#include "glob.h"
#include "tribezier.h"
#include "polygon.h"
#include "quadbezier.h"

void Patch_init(Patch*p)
{
    p->init = 1;
    p->normal_flipped = 0;
    p->art_normal=false;
    p->evaluated = false;
    p->eval_P = p->eval_N = NULL;
    p->crv_array = NULL;
    p->position = p->normal = NULL;
}

// translate the patch so it is centered at the point 'center'
void Patch_translate(Patch*p,real* center)
{
	int i;
	for(i=0;i<3;i++)
        p->location[i] += center[i];
}


void Patch_createSinglePolygon(Patch*p, int side, vector *V, int *F)
{
    p->pointCount = side;
    arrcreate(p->position, side);
    vector *position = p->position;
    arrcreate(p->normal, side);
    vector *normal = p->normal;
    for(int i = 0; i < side; i++)
    {
        real *v = V[F[i]];
        Bezier_enlarge_aabb(p, v[0],v[1],v[2]);
        position[i][0] = v[0];
        position[i][1] = v[1];
        position[i][2] = v[2];
        position[i][3] = v[3];
    }
    /* Calculate the normal by averaging side normals */
    for(int i= 0; i < side; i++)
    {
        int prv = i, pt = (i+1)%side, nxt = (i+2)%side;
        vector V1, V2;
        VVminus(position[prv], position[pt], V1);
        VVminus(position[nxt], position[pt], V2);
        VVcross(V2, V1, normal[pt]);
        Normalize(normal[pt]);
    }
}



void Patch_loadQuadBezier(Patch*p, FILE* fp)
{
    QuadBezier_loadFile(p, fp, p->type==TP_EQ, p->type==RATIONAL, p->type==PNTP);
}

void Patch_loadTriBezier(Patch*p, FILE* fp)
{
    TriBezier_loadFile(p, fp, p->type==PNTRI);
}

void Patch_enlarge_AABB(Patch*p,int first) {

	if (first)  // first patch
	{
        ViewLeft  = (float)p->minx;
        ViewRight = (float)p->maxx;

        ViewBottom= (float)p->miny;
        ViewTop   = (float)p->maxy;

        ViewFar   = (float)p->minz;
        ViewNear  = (float)p->maxz;
	}
	else {
        if(ViewLeft  > p->minx) ViewLeft  = (float)p->minx;
        if(ViewRight < p->maxx) ViewRight = (float)p->maxx;

        if(ViewBottom> p->miny) ViewBottom= (float)p->miny;
        if(ViewTop   < p->maxy) ViewTop   = (float)p->maxy;

        if(ViewFar   > p->minz) ViewFar   = (float)p->minz;
        if(ViewNear  < p->maxz) ViewNear  = (float)p->maxz;
	}
}



void Patch_plotcrv(Patch *p, int crv_choice)
{
    switch(p->type)
    {
    case POLY:
        break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
        QuadBezier_plot_crv(p, crv_choice);
        break;
    case TRIANG:
    case PNTRI:
        TriBezier_plot_crv(p, crv_choice);
        break;
    }

}



void Patch_plotmesh(Patch *p, color_t bg_color)
{
    switch(p->type)
    {
    case POLY:
        Polygon_plot_mesh(p,bg_color);
        break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
        QuadBezier_plot_mesh(p, bg_color);
        break;
    case TRIANG:
    case PNTRI:
        TriBezier_plot_mesh(p, bg_color);
        break;
    }

}


void Patch_plotpatch(Patch *p, bool smooth)
{
    switch(p->type)
    {
    case POLY:
        Polygon_plot_patch(p, smooth);
        break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
        QuadBezier_plot_patch(p, smooth);
        break;
    case TRIANG:
    case PNTRI:
        TriBezier_plot_patch(p, smooth);
        break;
    }
}


void Patch_plotcrvneedles(Patch *p, int crv_choice, double needle_length)
{
    switch(p->type)
    {
    case POLY:
        break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
        QuadBezier_plot_crv_needles(p, crv_choice, needle_length);
        break;
    case TRIANG:
    case PNTRI:
        TriBezier_plot_crv_needles(p, crv_choice, needle_length);
        break;
    }

}


void Patch_plothighlights(Patch *p, vector A, vector H, real hl_step, int hl_type)
{
    switch(p->type)
    {
    case POLY:
        Polygon_plot_highlights(p, A, H, hl_step, hl_type);
        break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
        QuadBezier_plot_highlights(p, A, H, hl_step, hl_type);
        break;
    case TRIANG:
    case PNTRI:
        TriBezier_plot_highlights(p, A, H, hl_step, hl_type);
        break;
    }

}


void Patch_flipnormal(Patch *p)
{
    switch(p->type)
    {
    case POLY:
        Polygon_flip_normal(p);
        break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
        QuadBezier_flip_normal(p);
        break;
    case TRIANG:
    case PNTRI:
        TriBezier_flip_normal(p);
        break;
    }

}



void Patch_evaluate(Patch* p, int SubDepth)
{
    switch(p->type)
    {
    case POLY:
        break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
        QuadBezier_evaluate_patch(p, SubDepth);
        break;
    case TRIANG:
    case PNTRI:
        TriBezier_evaluate_patch(p, SubDepth);
        break;
    }
}


void Patch_freeevalmem(Patch *p)
{
    Patch* b = p;
    if(b->eval_P)    arrdelete(b->eval_P);
    if(b->eval_N)    arrdelete(b->eval_N);
    if(b->crv_array) arrdelete(b->crv_array);
    b->evaluated = false;
    b->eval_P = b->eval_N = NULL;
    b->crv_array = NULL;
}

void Bezier_free_mem(Patch*p) {
    Patch* b = p;
    Patch_freeevalmem(p);
    if(b->position) arrdelete(b->position);
    if(b->normal)  arrdelete(b->normal);
}


void Bezier_enlarge_aabb(Patch *p, real x, real y, real z)
{
    if(p->init) {
        p->minx = p->maxx = x;
        p->miny = p->maxy = y;
        p->minz = p->maxz = z;
        p->init = 0;
    }
    else {
        if(x>p->maxx) p->maxx = x;
        if(x<p->minx) p->minx = x;
        if(y>p->maxy) p->maxy = y;
        if(y<p->miny) p->miny = y;
        if(z>p->maxz) p->maxz = z;
        if(z<p->minz) p->minz = z;
    }
}
