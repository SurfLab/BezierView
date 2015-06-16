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
}

#include "TriBezier.h"
#include "Polygon.h"
#include "QuadBezier.h"
#include "Patch.h"
#include "glob.h"

////////////////////////////////////////////////////////////////
//
//  load the Bezier data from a data file
//
int Patch::loadFile(FILE* fp) {
	// new patch
    TriBezier  * newtri;
    QuadBezier * newquad;
    PolygonMesh    * newpoly;

	// read in the 
	switch(type) {

    case POLY:
        newpoly = new PolygonMesh();
	    newpoly->loadFileBV(fp);
        object = newpoly; 
	    break;
    case TP:
    case TP_EQ:
    case RATIONAL:
    case PNTP:
		newquad = new QuadBezier();
		newquad -> loadFile(fp, type==TP_EQ, type==RATIONAL, type==PNTP);
        object = newquad; 
        break;
    case TRIANG:
    case PNTRI:
		newtri = new TriBezier();
		newtri -> loadFile(fp, type==PNTRI);
        object = newtri; 
		break;
    default:
        fprintf(stderr,"Unknown patch_kind %d\n", type);
        exit(1);
    }

	return 0;
};


// translate the patch so it is centered at the point 'center'
void Patch::translate(REAL* center)
{
	int i;
	for(i=0;i<3;i++)
	    position[i] += center[i];
}


void Patch::enlarge_AABB(int first) {

	if (first)  // first patch
	{
		ViewLeft  = (float)object->minx;
		ViewRight = (float)object->maxx;

		ViewBottom= (float)object->miny;
		ViewTop   = (float)object->maxy;

		ViewFar   = (float)object->minz;
		ViewNear  = (float)object->maxz;
	}
	else {
		if(ViewLeft  > object->minx) ViewLeft  = (float)object->minx;
		if(ViewRight < object->maxx) ViewRight = (float)object->maxx;

		if(ViewBottom> object->miny) ViewBottom= (float)object->miny;
		if(ViewTop   < object->maxy) ViewTop   = (float)object->maxy;

		if(ViewFar   > object->minz) ViewFar   = (float)object->minz;
		if(ViewNear  < object->maxz) ViewNear  = (float)object->maxz;
	}
}

