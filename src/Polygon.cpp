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
void Polygon::flip_normal()
{
    for(int i=0;i<this->pointCount;i++)
        for(int j = 0; j < 3; j++)
            this->normal[i][j] = -this->normal[i][j];
    normal_flipped = !normal_flipped;
}



////////////////////////////////////////////////////////////////
//
//  plot the polygon

void Polygon::plot_patch(bool smooth)
{
    int j, pt;

	//
	if(!evaluated) {
		evaluated = true;
		compute_crv();
	}

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);

    glBegin(GL_POLYGON);
    for (j=0; j<this->pointCount; j++) {
        if(!normal_flipped)  // reverse the orientation of the polygon
            pt =  (j);
        else
            pt =  (this->pointCount-1-j);

        glNormal3dv(this->normal[pt]);

        double size = 64.0;
        glTexCoord2f((this->position[pt])[0]/size,(this->position[pt])[2]/size);
        glVertex4dv(this->position[pt]);
    }
    glEnd();

	glPopAttrib();
}

////////////////////////////////////////////////////////////////
//
// plot the mesh 
//
void Polygon::plot_mesh(float* bg_color)
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
        for (j=0; j<this->pointCount; j++) {
            glVertex4dv(this->position[j]);
        }
        glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);	
		glPopAttrib();
	}


    for (j=0; j<this->pointCount; j++) {
        glBegin(GL_LINES);
            glVertex4dv(this->position[j]);
            glVertex4dv(this->position[(j+1)%this->pointCount]);
        glEnd();
    }
}

////////////////////////////////////////////////////////////////
//
// plot the highlight lines
//
void Polygon::plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type)
{
    int j;

	REAL P[MAXSIDE*DIM];
	REAL N[MAXSIDE*DIM];

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);
    for (j=0; j<this->pointCount; j++) {
        Vcopy(this->position[j], &P[j*DIM]);
        Vcopy(this->normal[j], &N[j*DIM]);
    }
    Highlight(this->pointCount, P, N, A, H, hl_step, highlight_type);
	glPopAttrib();
}



