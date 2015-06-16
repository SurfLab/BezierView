/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    draw.cpp
 *  Purpose: render all kinds of patches,
 *           including tensor-product, triangular, PNtriangular...
 * ------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdheaders.h>
#include "glob.h"
#include "SGOL/util.h"
#include "SGOL/Bezier.h"
#include "texture.h"
#include "SGOL/Polygon.h"
#include "SGOL/QuadBezier.h"
#include "SGOL/TriBezier.h"
#include "SGOL/curvature.h"
#include "draw.h"
#include "menu.h"
#include <QtOpenGL>


/* curvature utility functions */
void set_crv_scale(double lowc, double highc);
void draw_crv_bar(GLfloat* color);  // the curvature bar
void draw_clipping_plane(int index, int mode, REAL  size);

void set_color(int i);            // in light.cpp
void set_colorf(float r, float g, float b); //in light.cpp
void Solve4(double * A, double* x); // in util.cpp
void init_texture(GLubyte *forecolor, GLubyte *backcolor);

void draw(void);

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
//	readin_curv_bounds();
	needle_length=1.0;
	hl_step = 1.0; //0.5*((CBezierViewDoc *)GetDocument())->size;     // distance of the parallel lights
	g_AntiAlias = false;
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST );
	if(!texture_loaded) {
		if (LoadBitmap("room.bmp") >=0)
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
    {
        Patch* fp = &(face[i]);
		(fp->object)->flip_normal();
	}

	normal_flipped = !normal_flipped;
	//define_patch();
	define_crv();
}
void set_g_redisplay(){
    g_redisplay=1;
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
//		glutWireCube(1.0);
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
				(fp->object)->plot_crv(crv_choice);
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
					(fp->object)->plot_mesh(g_BackColor[back_choice]);
				else
					(fp->object)->plot_mesh(NULL);

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

			if(patch_on )
				//&& color != COLORNUM-1)  // COLORNUM -1 is transparent
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
                    if(color == COLORNUM-2)
						set_color(rand()%(COLORNUM-2)); 
                    else
                       set_colorf(r,g,b);
                       //set_color(0);
				}

                (fp->object)->plot_patch(isDisplayFlagEnabled(grp_id, SMOOTH)==1);

				if(mesh_on) {
					glDisable(GL_POLYGON_OFFSET_FILL);
				}
			}

			glDisable(GL_TEXTURE_GEN_S);                        
			glDisable(GL_TEXTURE_GEN_T);                         
			glDisable(GL_TEXTURE_2D); 

            if(isDisplayFlagEnabled(grp_id, DRAWCRVNEEDLE)) {
				(fp->object)->plot_crv_needles(crv_choice, needle_length);
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

                set_colorf(r,g,b);
				(fp->object)->plot_highlights(A, H, hl_step, hl_type);
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
				((Bezier*)(fp->object))->free_eval_mem();
				(fp->object)->evaluate_patch(substeps);
				if (normal_flipped) (fp->object)->flip_normal();
			}
		}
    }
	define_crv();
}


///////////////////////////////////////
//
// output to eps file 
void export_eps()
{
	int i;
	FILE* fp;
	int j, v, f;
	Patch* p;

    GLdouble modelMatrix[16];
    GLdouble projMatrix[16];
    GLint viewport[4];
	GLdouble winx, winy, winz;

	fp = fopen("output.eps", "w");
	if(fp==NULL) return ;  // can't write

    glMatrixMode(GL_MODELVIEW);
	//printf("%f %f %f \n", -ViewCenter[0], -ViewCenter[1], -ViewCenter[2]);
	glPushMatrix();
	glScaled(scale_factor, scale_factor, scale_factor);
	glTranslated(-ObjectCenter[0], -ObjectCenter[1], -ObjectCenter[2]);

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT , viewport);

	fprintf(fp, "%%!PS-Adobe-2.0 EPSF-2.0\n");
	fprintf(fp, "%%%%Title: diffeon.eps\n");
	fprintf(fp, "%%%%Creator: BezierView\n");
	fprintf(fp, "%%%%CreationDate: Wed Jun  4 15:45:41 2003\n");
	fprintf(fp, "%%%%For: xwu@sand (Xiaobin Wu)\n");
	fprintf(fp, "%%%%BoundingBox: 0 0 500 500\n");
	fprintf(fp, "%%%%Magnification: 0.6500\n");
	fprintf(fp, "%%%%EndComments\n");
	fprintf(fp, "/Times-Roman findfont 14 scalefont setfont \n");
	fprintf(fp, ".1 setgray \n");

    for (i=1; i<=patch_num ; i++)
    {
        p = &(face[i]);
        int patch_kind = p->type;
		if(patch_kind == POLY) {
			PolygonMesh * poly = (PolygonMesh*) p->object; 

			for(f=0;f<poly->FNum; f++) {
		        int n = poly->faces[f].get_n(); // number of vertices on this face
				//int ignore =0;

				for (j=0; j<n; j++) {
					v =  poly->faces[f].get_v_ind(j);
					gluProject(
						((poly->get_vertex(v))->get_p())[0],  
						((poly->get_vertex(v))->get_p())[1],
						((poly->get_vertex(v))->get_p())[2],
					    modelMatrix,   
					    projMatrix,   
					    viewport,         
					    &winx, &winy, &winz     
					);
					if(j==0)
						fprintf(fp, "%d %d moveto\n", (int)winx, (int)winy);
					else
						fprintf(fp, "%d %d lineto\n", (int)winx, (int)winy);
				}
				fprintf(fp, "closepath\n");
				fprintf(fp, "stroke\n");
			}
		}
	}
	fprintf(fp, "showpage\n");

	glPopMatrix();
	fclose(fp);
}



///////////////////////////////////////
// output igs file
int knots(int dg1, int bbase, int ffctr, FILE* fp, int per_line);

void export_igs()
{
	FILE* fp;
    Patch* p;

    int         bbctr,ffctr,
                k,k1,w,w1,rows,flen[4],
                i,j, col,cols, fc,dg;
    double      h;
    double*     hv;

	fp = fopen("output.igs", "w");
	if(fp==NULL) {
		printf("can't open output file output.igs\n");
        return ;  // can't write
    }

	printf("outputing the object into file output.igs..\n");

    flen[0] = 3;                              
    flen[1] = 8;

    fprintf(fp, "WRAP                                                                    S      1\n");
    fprintf(fp, "B-spline surfaces.                                                      S      2\n");
    fprintf(fp, "Number of Patches:  %4d                                                S      3\n", patch_num);

    for(i=1;i<=flen[1];i++)
       fprintf(fp, "                                                                        G      %d\n", i);

    bbctr=1; ffctr=1;
    for (fc=1; fc <= patch_num; fc++) {
        p = &(face[fc]);
        int patch_kind = p->type;
		if(patch_kind == TRIANG ) {
			TriBezier * tri = (TriBezier*) p->object; 
			if(tri->deg == 1) {
				dg = 1; 
            	k = (dg+1)*2;
            	k1 = (k%8 != 0) + k/8;  /* knots */
            	w = (dg+1)*(dg+1);
            	w1 = (w%8 != 0) + w/8;  /* weights */
            	/* deg-line, knots, weights, xyz location, param */
            	rows  = (1+ 2*k1 + w1 + w + 1);  /* size of one block */

            	fprintf(fp,"     128%8d       0       1       0       0       0        00000000D%7d\n",
                	bbctr, ffctr);
            	fprintf(fp,"     128%8d       8      %d       2                NurbSurf       0D%7d\n",
                	0, rows, ffctr+1);
            	bbctr += rows;
            	ffctr += 2;                            
			}
		}
		if(patch_kind == TP || patch_kind == TP_EQ ) {

			QuadBezier * quad = (QuadBezier*) p->object; 

			dg = quad->degu;

            k = (dg+1)*2;
            k1 = (k%8 != 0) + k/8;  /* knots */
            w = (dg+1)*(dg+1);
            w1 = (w%8 != 0) + w/8;  /* weights */
            /* deg-line, knots, weights, xyz location, param */
            rows  = (1+ 2*k1 + w1 + w + 1);  /* size of one block */

            fprintf(fp,"     128%8d       0       1       0       0       0        00000000D%7d\n",
                bbctr, ffctr);
            fprintf(fp,"     128%8d       8      %d       2                NurbSurf       0D%7d\n",
                0, rows, ffctr+1);
            bbctr += rows;
            ffctr += 2;                            
        }                    
	}


    ffctr = 1; bbctr=1;
    for (fc=1; fc<=patch_num ; fc++)
    {
        p = &(face[fc]);
        int patch_kind = p->type;

		if(patch_kind == TRIANG ) {
			TriBezier * tri = (TriBezier*) p->object; 

			if(tri->deg == 1) {
				printf("outputing triangles as degenrated quads\n");

				dg = 1;

				// HEADER 
				fprintf(fp,"128,%7d,%7d,%7d,%7d,0,0,1,0,0,%26dP%7d\n",
					dg,dg,dg,dg,ffctr,bbctr);
				bbctr++;

				// KNOTS 
				w = (dg+1)*(dg+1);
				bbctr = knots(dg+1, bbctr, ffctr,fp,8);
				bbctr = knots(dg+1, bbctr, ffctr,fp,8);
													 
				// the w==RATIONAL coordinate   
				cols = 8;                            
				col = 0;                               
				for (i=0; i<= dg; i++) {               
					for (j=0; j<= dg; j++) {           
						if ((col % cols==0) && (col != 0)) {  /* typeset */ 
							fprintf(fp,"%8dP%7d\n",ffctr,bbctr);
							bbctr++;
						}                              
						h = 1.0; //(fp->bb[0][i][j])->w;      
						fprintf(fp,"%7.5f,", h);
						col++;                         
					}                                  
				}                                      
				// finish line 
				col = col%cols;                        
				if (col != 0) {                        
					col = cols-col;                    
					for (i=0; i< col; i++)             
						fprintf(fp,"        ");    
				}                                      
				fprintf(fp,"%8dP%7d\n",ffctr,bbctr);
	
				bbctr++;                               
													   	
				// the XYZ coordinates 
				for (i=0; i<= 1; i++) {               
					for (j=0; j<= 1; j++) {           
						if(i==1 && j==1)
						    hv=tri->get_bb(0,1);    
						else
						    hv=tri->get_bb(i,j);    
	
						fprintf(fp,"%20e,%20e,%20e,%9dP%7d\n", 
							hv[0],hv[1], hv[2], ffctr,bbctr);
						//fprintf(fp,"%20e,%20e,%20e,%9dP%7d\n", 
							//0.0,0.0, 0.0, ffctr,bbctr);
						bbctr++;                       
					}                                  
				}                                      
				fprintf(fp,"0.00000,1.00000,0.00000,1.00000;%40dP%7d\n",
					ffctr,bbctr);                      
				bbctr++;                               
				ffctr += 2;                            
			}
			else{
				printf("skiping the pach number %d (not tensor product)\n", fc);
			}
		}
		else if(patch_kind == TP || patch_kind == TP_EQ ) {

			QuadBezier * quad = (QuadBezier*) p->object; 

			dg = quad->degu;

			// HEADER 
			fprintf(fp,"128,%7d,%7d,%7d,%7d,0,0,1,0,0,%26dP%7d\n",
				dg,dg,dg,dg,ffctr,bbctr);
			bbctr++;

			// KNOTS 
			w = (dg+1)*(dg+1);
			bbctr = knots(dg+1, bbctr, ffctr,fp,8);
			bbctr = knots(dg+1, bbctr, ffctr,fp,8);
													 
			// the w==RATIONAL coordinate   
			cols = 8;                            
			col = 0;                               
			for (i=0; i<= dg; i++) {               
				for (j=0; j<= dg; j++) {           
					if ((col % cols==0) && (col != 0)) {  /* typeset */ 
						fprintf(fp,"%8dP%7d\n",ffctr,bbctr);
						bbctr++;
					}                              
					h = 1.0; //(fp->bb[0][i][j])->w;      
					fprintf(fp,"%7.5f,", h);
					col++;                         
				}                                  
			}                                      
			// finish line 
			col = col%cols;                        
			if (col != 0) {                        
				col = cols-col;                    
				for (i=0; i< col; i++)             
					fprintf(fp,"        ");    
			}                                      
			fprintf(fp,"%8dP%7d\n",ffctr,bbctr);

			bbctr++;                               
													   
			// the XYZ coordinates 
			for (i=0; i<= dg; i++) {               
				for (j=0; j<= dg; j++) {           
					hv=quad->get_bb(i,j);    
					fprintf(fp,"%20e,%20e,%20e,%9dP%7d\n", 
						hv[0],hv[1], hv[2], ffctr,bbctr);
					bbctr++;                       
				}                                  
			}                                      
			fprintf(fp,"0.00000,1.00000,0.00000,1.00000;%40dP%7d\n",
				ffctr,bbctr);                      
			bbctr++;                               
			ffctr += 2;                            
		}                                          
		else
			printf("skiping the pach number %d (not tensor product)\n", fc);
	}
	flen[2] = ffctr;                           
	flen[3] = bbctr;                           
	// structure of file 
	fprintf(fp,"S%7dG%7dD%7dP%7d%40dT%7d\n",
				flen[0],flen[1],flen[2]-1,flen[3]-1,1,1);
	fclose(fp);
	printf("done\n");
}

/* write knots of bb-form in IGES */           
int knots(int dg1, int bbase, int ffctr, FILE* fp, int per_line)       
{                                              
    int i,j;                                   
                                               
    for (i=0; i< dg1; i++){                    
        if ((i%per_line==0) && (i != 0)) {     
            fprintf(fp,"%8dP%7d\n",ffctr,bbase);
            bbase++;                           
        }                                      
        fprintf(fp,"0.00000,");                
    }                                          
    for (; i< 2*dg1; i++){                     
        if (i%per_line==0) {                   
            fprintf(fp,"%8dP%7d\n",ffctr,bbase);
            bbase++;                           
        }                                      
        fprintf(fp,"1.00000,");                
    }                                          
    j = (2*dg1)%per_line;                      
    if (j != 0) {                              
            j = per_line-j;                    
            for (i=0; i< j; i++){              
                fprintf(fp,"        ");        
            }                                  
    }                                          
    fprintf(fp,"%8dP%7d\n",ffctr,bbase);       
    bbase++;                                   
    return(bbase);                             
}             



//
//
// End of source code  ================================================

