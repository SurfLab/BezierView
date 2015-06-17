/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    polygon.cpp
 *  Purpose: render polygonal objects
 *  Author   : Xiaobin Wu
 * ------------------------------------------------------------
 */
#include "stdheaders.h"
extern "C" {
#include "util.h"
#include "curvature.h"
#include "highlight.h"
}
#include "Polygon.h"

#define MAXSIDE 50

////////////////////////////////////////////////////////////////
////
//// set the neighboring information


//////////////////////////////////////////////////////////
// flip the normal
void PolygonMesh::flip_normal()
{
	REAL* normal;
	int i;
    // initialize 
    for(i=0;i<VNum;i++) {
		normal = vertices[i].get_n();
        vertices[i].set_n(-normal[0], -normal[1], -normal[2]);
	}
	normal_flipped = !normal_flipped;
}



////////////////////////////////////////////////////////////////
//
//  plot the polygon

void PolygonMesh::plot_patch(bool smooth)
{
    int i, j, pt;

	//
	if(!evaluated) {
		evaluated = true;
		compute_crv();
	}

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);

    glBegin(GL_POLYGON);
    for (j=0; j<VNum; j++) {
        if(!normal_flipped)  // reverse the orientation of the polygon
            pt =  (j);
        else
            pt =  (VNum-1-j);

        glNormal3dv(vertices[pt].get_n());

        double size = 64.0;
        glTexCoord2f((vertices[pt].get_p())[0]/size,(vertices[pt].get_p())[2]/size);
        glVertex4dv(vertices[pt].get_p());
    }
    glEnd();

	glPopAttrib();
}

////////////////////////////////////////////////////////////////
//
// plot the mesh 
//
void PolygonMesh::plot_mesh(float* bg_color)
{
    int i, j, pt;
	
    //glShadeModel(GL_SMOOTH);
	if(bg_color) {   // if hidden line removal
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT );
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glColor3fv(bg_color);

        glBegin(GL_POLYGON);
        for (j=0; j<VNum; j++) {
            glVertex4dv(vertices[j].get_p());
        }
        glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);	
		glPopAttrib();
	}


    for (j=0; j<VNum; j++) {
        glBegin(GL_LINES);
            glVertex4dv(vertices[j].get_p());
            glVertex4dv(vertices[(j+1)%VNum].get_p());
        glEnd();
    }
}

////////////////////////////////////////////////////////////////
//
// plot the highlight lines
//
void PolygonMesh::plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type)
{
    int j;

	REAL P[MAXSIDE*DIM];
	REAL N[MAXSIDE*DIM];

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);
    for (j=0; j<VNum; j++) {
        Vcopy(vertices[j].get_p(), &P[j*DIM]);
        Vcopy(vertices[j].get_n(), &N[j*DIM]);
    }
    Highlight(VNum, P, N, A, H, hl_step, highlight_type);
	glPopAttrib();
}



