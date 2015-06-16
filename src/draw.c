/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    draw.cpp
 *  Purpose: render all kinds of patches,
 *           including tensor-product, triangular, PNtriangular...
 * ------------------------------------------------------------
 */
#include "stdheaders.h"
#include "type.h"
#include "Patch.h"
#include "glob.h"
#include "util.h"
#include "curvature.h"
#include "highlight.h"
#include "light.h"
#include "draw.h"
#include "menu.h"
#include "texture.h"


//////////////////////////////////////////////////
// Display models

// initialization
void init_flags()
{
	for(int i=0;i < MAXGROUP ; i++)
	{
		g_Material[i] = i;
        g_Mode[i]     = DRAWPATCH | DRAWPOLYPATCH | SMOOTH ;

		g_PenColor[i]  = 1;  // light blue
		g_LineWidth[i] = 3;
	}
	g_Material[0] = 0;
	g_current_grp = 0;
	drawbox = 0;
	g_mouseMode = ROTATE;
	//m_envtexture_id = 0;
    texture1D_initialized = false;

	g_redisplay = true; 
	crv_choice = GAUSS_CRV;
    manual_curvature_low = manual_curvature_high =0;
    readin_curv_bounds();
    needle_length=1.0;
	hl_step = 1.0; //0.5*((CBezierViewDoc *)GetDocument())->size;     // distance of the parallel lights
	g_AntiAlias = false;
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST );
	if(!texture_loaded) {
		if (LoadBitmapTexture("room.bmp") >=0)
		   texture_loaded = 1;
	}

}


/////////////////////////////////////////////////////////////
// check if the display flag is enabled
bool isDisplayFlagEnabled(int grp_id, int flag) {
	if(grp_id ==0 && group_num >0) {
		for(int i=0; i<=group_num; i++) {
			if(!(g_Mode[i] & flag)) {
				return false;
			}
		}
		return true;
	}
	else
		return ((g_Mode[grp_id] & flag) !=0 );
}

// toggle the mode
void ToggleMode(int grp_id, int flag)
{
	if(grp_id ==0 && group_num >0) {
		for(int i=0; i<= group_num; i++) {
            g_Mode[i] ^= flag;

		}
	}
    else{
        g_Mode[grp_id] ^= flag;
    }

    g_redisplay =1;
    //update();

}

// disable the mode
void DisableMode(int grp_id, int flag)
{
	if(grp_id ==0 && group_num >0) {
		for(int i=0; i<= group_num; i++) {
			g_Mode[i] &= (~flag);
		}
	}
	else
		g_Mode[grp_id] &= (~flag);
}

//////////////////////////////////////
// set the curvature bounds
void define_crv()  
{
	REAL low, high;

    // set curvature bounds
	//
    // if manual bounds exists, use manual bounds
    if(manual_curvature_low)
		low = manual_low; 
	else
		low = min_crv[crv_choice];
    if(manual_curvature_high)
		high = manual_high; 
	else
		high = max_crv[crv_choice];

	if(normal_flipped && crv_choice != GAUSS_CRV  && crv_choice != SPECIAL_CRV)
		set_crv_scale(-high, -low);
	else
		set_crv_scale(low, high);

	g_redisplay = 1;
}

// flip the normal of the object
void flip_normal()
{
	int i;

    for (i=1; i<=patch_num ; i++)
        Patch_flipnormal(&(face[i]));

	normal_flipped = !normal_flipped;
	//define_patch();
	define_crv();
}
void set_g_redisplay(){
    g_redisplay=1;
}

void drawWireCube()
{
    glBegin(GL_LINES);
    // Along Z
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, +0.5f);
    glVertex3f(-0.5f, +0.5f, -0.5f);
    glVertex3f(-0.5f, +0.5f, +0.5f);
    glVertex3f(+0.5f, +0.5f, -0.5f);
    glVertex3f(+0.5f, +0.5f, +0.5f);
    glVertex3f(-0.5f, +0.5f, -0.5f);
    glVertex3f(-0.5f, +0.5f, +0.5f);

    // Along X
    glVertex3f(-0.5f, +0.5f, +0.5f);
    glVertex3f(+0.5f, +0.5f, +0.5f);
    glVertex3f(-0.5f, +0.5f, -0.5f);
    glVertex3f(+0.5f, +0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, +0.5f);
    glVertex3f(+0.5f, -0.5f, +0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(+0.5f, -0.5f, -0.5f);

    // Along Y
    glVertex3f(+0.5f, -0.5f, +0.5f);
    glVertex3f(+0.5f, +0.5f, +0.5f);
    glVertex3f(-0.5f, -0.5f, +0.5f);
    glVertex3f(-0.5f, +0.5f, +0.5f);
    glVertex3f(+0.5f, -0.5f, -0.5f);
    glVertex3f(+0.5f, +0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, +0.5f, -0.5f);

    glEnd();
}

//////////////////////////////////////
// draw all the objects
void draw(void)
{
	static int displayList = 0;
	int i;

    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//	printf("current scale_factor %f\n", scale_factor);
	glScaled(scale_factor, scale_factor, scale_factor);

	// draw a bounding box
	if(drawbox)
	{
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT|GL_LINE_BIT);
		glLineWidth(2.0);
		glDisable(GL_LIGHTING);
		glPushMatrix();
		glScaled((ViewRight - ViewLeft),  ViewTop - ViewBottom, ViewNear - ViewFar);
		glColor3f(1.0, .5, 0.0);
        // Draw a cube bounding box
        drawWireCube();
		glPopMatrix();
		glPopAttrib();
	}

	if(g_redisplay || (!use_display_list) ) {
		int crv_on = false;

		if(use_display_list) {
			if( glIsList(displayList) )   // remove the old list if exists
				glDeleteLists(displayList, 1);

			displayList = glGenLists(1);
			glNewList(displayList, GL_COMPILE_AND_EXECUTE);
		}

		glTranslated(-ObjectCenter[0], -ObjectCenter[1], -ObjectCenter[2]);

		// draw each the patch
		for (i=1; i<=patch_num ; i++)
		{
			Patch* fp      = &(face[i]);
			int patch_kind = fp->type;
           // qDebug()<<"patch type="<<patch_kind;
			int grp_id     = fp->group_id;
            int color      = g_Material[grp_id];
            float r = g_patchColor[grp_id][0];
            float g = g_patchColor[grp_id][1];
            float b = g_patchColor[grp_id][2];
	//		// color of the group that this face belongs to

            if(isDisplayFlagEnabled(grp_id, NORMAL)){
                g_Mode[i]= DRAWPATCH | DRAWPOLYPATCH | SMOOTH ;
            }
            int patch_on = (isDisplayFlagEnabled(grp_id, DRAWPOLYPATCH) && (patch_kind == POLY) )  ||
                           (isDisplayFlagEnabled(grp_id, DRAWPATCH)     && (patch_kind != POLY) );
            int mesh_on  = (isDisplayFlagEnabled(grp_id, DRAWPOLYMESH) && (patch_kind == POLY) ) ||
                           (isDisplayFlagEnabled(grp_id, DRAWMESH)     && (patch_kind != POLY) );

			int line_width = g_LineWidth[grp_id];

            if (isDisplayFlagEnabled(grp_id, DRAWCRV) ||
                    isDisplayFlagEnabled(grp_id, DRAWHIGHLIGHT)	||
                    isDisplayFlagEnabled(grp_id, DRAWREFLLINE))
					patch_on = 0;

            if(isDisplayFlagEnabled(grp_id, DRAWCRV)) {
				crv_on = 1;
                Patch_plotcrv(fp, crv_choice);
			}

			// polygon mesh
			if( mesh_on )
			{
				glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT );
				glDisable(GL_LIGHTING);
				glLineWidth(line_width);
				if(g_AntiAlias) 
					glEnable(GL_LINE_SMOOTH);
				else
					glDisable(GL_LINE_SMOOTH);

				glColor3fv(g_penColors[g_PenColor[grp_id]]);

                if( isDisplayFlagEnabled(g_current_grp, HIDDENLINE) &&
                    (!patch_on ) && (!isDisplayFlagEnabled(grp_id, DRAWCRV)) &&
                    (!isDisplayFlagEnabled(grp_id, DRAWHIGHLIGHT))	&&
                    (!isDisplayFlagEnabled(grp_id, DRAWREFLLINE)) )
                    Patch_plotmesh(fp,g_BackColor[back_choice]);
				else
                    Patch_plotmesh(fp,NULL);

				glPopAttrib();
			}

            if(isDisplayFlagEnabled(grp_id, ENVMAPPING) )
			{
				// Spherical Environment mapping
				glEnable(GL_TEXTURE_2D); 
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
				glEnable(GL_TEXTURE_GEN_S);                        
				glEnable(GL_TEXTURE_GEN_T);                         
			}
			else
			{
				glDisable(GL_TEXTURE_2D); 
			}

            if(patch_on)
			{
				if(mesh_on) {
					glEnable(GL_POLYGON_OFFSET_FILL);
					glPolygonOffset(1.0, 1.0);
				}

                if(color == COLORNUM-1)
				{
					glEnable(GL_BLEND);             // Turn Blending On
					glDisable(GL_DEPTH_TEST);       // Turn Depth Testing Off
					set_color(0);
				}
				else                                // Otherwise
				{
					glDisable(GL_BLEND);            // Turn Blending Off
					glEnable(GL_DEPTH_TEST);        // Turn Depth Testing On
                    if(color == -1)
                        set_colorf(r,g,b);
                    else if(color == COLORNUM-2)
                        set_color(rand()%(COLORNUM-2));
                    else
                       set_color(color);
				}

                Patch_plotpatch(fp,isDisplayFlagEnabled(grp_id, SMOOTH)==1);

				if(mesh_on) {
					glDisable(GL_POLYGON_OFFSET_FILL);
				}
			}

			glDisable(GL_TEXTURE_GEN_S);                        
			glDisable(GL_TEXTURE_GEN_T);                         
			glDisable(GL_TEXTURE_2D); 

            if(isDisplayFlagEnabled(grp_id, DRAWCRVNEEDLE)) {
                Patch_plotcrvneedles(fp,crv_choice, needle_length);
			}

			// highlight lines (also called reflection lines if provided an eye poisition)
            if(isDisplayFlagEnabled(grp_id, DRAWHIGHLIGHT) || isDisplayFlagEnabled(grp_id, DRAWREFLLINE) )
			{
				REAL A[DIM] = {  0.0,  0.0, 40.0, 1.0 };
				REAL H[DIM] = {  0.0,  1.0,  0.0, 0.0 };
				GLubyte forecolor[3]  = {0, 128, 0};
				GLubyte backcolor[3]  = {255, 255, 255};
				int hl_type;
                if (isDisplayFlagEnabled(grp_id, DRAWHIGHLIGHT))
					hl_type =0;
				else 
					hl_type =1;

				if(!texture1D_initialized)
				{
					init_texture(forecolor, backcolor);
					texture1D_initialized = true;
				}

				double mv_matrix[16];
				glMatrixMode(GL_MODELVIEW);
				glGetDoublev(GL_MODELVIEW_MATRIX, mv_matrix);

//				A[2] = 2.0*pDoc->sizey+ pDoc->centery;

				Solve4(mv_matrix, A);
				Solve4(mv_matrix, H);

                Patch_plothighlights(fp, A, H, hl_step, hl_type);
			}
		}
		
		if(crv_on) {
            glClear(GL_DEPTH_BUFFER_BIT);
			draw_crv_bar(getMeshColor());
		}

		if(use_display_list) {
			glEndList();
		}

		g_redisplay =0;
	}
	else
	{
		glCallList(displayList);
	}

	glPopMatrix();
}

//////////////////////////////////////////////////////////////
// re-evaluate the patches using a different subdivision steps
void    re_evaluate(int substeps)
{
    int pat, grp_id, i;

	if(g_current_grp == 0)
		for(i=0;i<=group_num;i++)
			g_substs[i] = substeps;
	else
		g_substs[g_current_grp] = substeps;

	init_crv();
    for (pat=1; pat<=patch_num; pat++)
    {
		Patch* fp = &face[pat];
		if(fp->type != POLY)
		{
			grp_id = fp->group_id;
			if(grp_id == g_current_grp || g_current_grp ==0) {
                Patch_freeevalmem(fp);
                Patch_evaluate(fp, substeps);
                if (normal_flipped) Patch_flipnormal(fp);
			}
		}
    }
	define_crv();
}




//
//
// End of source code  ================================================

