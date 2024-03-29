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

#include "type.h"
#include "patch.h"

#include "polygon.h"


void Polygon_flip_normal(Patch*p)
{
    for(int i=0;i<p->pointCount;i++)
        for(int j = 0; j < 3; j++)
            p->normal[i][j] = -p->normal[i][j];
    p->normal_flipped = !p->normal_flipped;
}




void Polygon_plot_patch(Patch*p)
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
        glTexCoord2d((p->position[pt])[0]/size,(p->position[pt])[2]/size);
        glVertex4dv(p->position[pt]);
    }
    glEnd();

	glPopAttrib();
}


void Polygon_plot_mesh(Patch*p, color_t bg_color)
{
    //glShadeModel(GL_SMOOTH);
    if(bg_color.alpha != 0.0f) {   // if hidden line removal
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT );
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
        glColorc(bg_color);

        glBegin(GL_POLYGON);
        for (int j=0; j<p->pointCount; j++) {
            glVertex4dv(p->position[j]);
        }
        glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);	
		glPopAttrib();
	}


    for (int j=0; j<p->pointCount; j++) {
        glBegin(GL_LINES);
            glVertex4dv(p->position[j]);
            glVertex4dv(p->position[(j+1)%p->pointCount]);
        glEnd();
    }
}


void Polygon_plot_highlights(Patch*p,vector A, vector H, real hl_step, int highlight_type)
{
	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);

    Highlight(p->pointCount, p->position, p->normal, A, H, hl_step, highlight_type);
	glPopAttrib();
}

