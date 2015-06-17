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
#include "util.h"
#include "curvature.h"
#include "highlight.h"
#include "polygon.h"


void Polygon_flip_normal(Patch*p)
{
    for(int i=0;i<p->pointCount;i++)
        for(int j = 0; j < 3; j++)
            p->normal[i][j] = -p->normal[i][j];
    p->normal_flipped = !p->normal_flipped;
}




void Polygon_plot_patch(Patch*p,bool smooth)
{
    int j, pt;

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);

    glBegin(GL_POLYGON);
    for (j=0; j<p->pointCount; j++) {
        if(!p->normal_flipped)  // reverse the orientation of the polygon
            pt =  (j);
        else
            pt =  (p->pointCount-1-j);

        glNormal3dv(p->normal[pt]);

        double size = 64.0;
        glTexCoord2f((p->position[pt])[0]/size,(p->position[pt])[2]/size);
        glVertex4dv(p->position[pt]);
    }
    glEnd();

	glPopAttrib();
}


void Polygon_plot_mesh(Patch*p,float* bg_color)
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
        for (j=0; j<p->pointCount; j++) {
            glVertex4dv(p->position[j]);
        }
        glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);	
		glPopAttrib();
	}


    for (j=0; j<p->pointCount; j++) {
        glBegin(GL_LINES);
            glVertex4dv(p->position[j]);
            glVertex4dv(p->position[(j+1)%p->pointCount]);
        glEnd();
    }
}


void Polygon_plot_highlights(Patch*p,VEC A, VEC H, REAL hl_step, int highlight_type)
{
	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);

    Highlight(p->pointCount, &p->position[0][0], &p->normal[0][0], A, H, hl_step, highlight_type);
	glPopAttrib();
}

