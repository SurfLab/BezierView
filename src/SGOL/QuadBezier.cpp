/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    QuadBezier.cpp
 *  Purpose: render Tensor-product Bezier patches
 *  Author   : Xiaobin Wu
 * ------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#define FREEGLUT_STATIC 1
#include <stdheaders.h>
#include <math.h>
#include "QuadBezier.h"
#include "curvature.h"
#include "highlight.h"
#include "util.h" 


// functions prototypes -------------------------

// quad Bezier patch subdivision 
void     RSubDiv(VEC bb[], int step, int degu, int degv, int sizeu, int sizev);
void     BBcopy4( REAL * buf, int degu, int degv, int st, VEC * bb);

// Decastel algorithms for 1-D and 2-D Bezier
void     DeCastel2(GLdouble* coeff, int degu, int degv,   
				double u, double v, GLdouble* pts);
void     DeCastel1(GLdouble* coeff, int deg, double u, GLdouble* pts);

// global variables -------------------------


int use_art_normal = 1;    // allow PN quads and PN triangles

////////////////////////////////////////////////////////////////
//
// load the Bezier data from a data file
//
int QuadBezier::loadFile(FILE* fp, bool equal_deg,
			   	bool rational, bool art_normal) {
	int i, m;
	int degu, degv;

    fscanf(fp,"%d", &degu);
	if(equal_deg)
		degv = degu;
	else
        fscanf(fp,"%d", &degv);

	// create the patch ( allocate memory, assign degrees)
	create(degu, degv);
//    num_points = (degu+1)*(degv+1);
//    coeff = alloc_mem_db(num_points*DIM);    // allocate memory 

	if(art_normal)   // if this is a PN quad Bezier patch
	{
        fscanf(fp,"%d", &Ndegu);
        fscanf(fp,"%d", &Ndegv);
		num_normals = (Ndegu+1)*(Ndegv+1);
	}

    // read in all control points

    for (i=0;i<num_points;i++) {
 	    for (m=0;m<3;m++)
            fscanf(fp,"%lg", &coeff[i*DIM+m]);

		if(rational)
            fscanf(fp,"%lg", &coeff[i*DIM+3]);
		else
	    	coeff[i*DIM+3] = 1.0;

        // adjust view volume to contain the point
        enlarge_aabb(coeff[i*DIM+0]/coeff[i*DIM+3],
           coeff[i*DIM+1]/coeff[i*DIM+3], coeff[i*DIM+2]/coeff[i*DIM+3]);
    }

    // read in all coefficients of the normal function, if PN quad
	if(art_normal) {
    	norm = alloc_mem_db(num_normals*DIM);    // allocate memory 
    	for (i=0;i<num_normals;i++) {
 	    	for (m=0;m<3;m++)
            	fscanf(fp,"%lg", &norm[i*DIM+m]);

			if (DIM==4)
	    		norm[i*DIM+3] = 1.0;
		}
    }
	
	this->art_normal = art_normal;

    //evaluate_patch(); 
	return 0;
};


////////////////////////////////////////////////////////////////
//
// create the necessary space for the Bezier Patch
//
int QuadBezier::create(int degu, int degv) {
	this->degu = degu;
	this->degv = degv;
    num_points = (degu+1)*(degv+1);
    coeff = alloc_mem_db(num_points*DIM);    // allocate memory 
	return 0;
};


////////////////////////////////////////////////////////////////////
//
//   draw the patch 
//
void QuadBezier::plot_patch(bool smooth)
{
	int i,j;
	int loc;

	if(!evaluated)
		return;

	glPushAttrib(GL_LIGHTING_BIT);

	if(!smooth) {
		glShadeModel(GL_FLAT);
	}

	  // the filling patch
	  // the plot of the patch consists of a set of quad strips
	  // 
	  // draw a strip for each i in [0,pts)
	  for(i=0;i<pts;i++) {

		glBegin(GL_QUAD_STRIP);
        for(j=0;j<=pts;j++)       // this loop will draw the quad strip:
        {
			if(!normal_flipped) { // reverse the orientation of the patch
				loc = i*(pts+1)+j;                // (i,j)----(i, j+1) -- ...
				glNormal3dv(&(eval_N[loc*DIM]));  //   |         |
				glVertex4dv(&(eval_P[loc*DIM]));  //   |         |
				loc = (i+1)*(pts+1)+j;            // (i+1,j)--(i+1, j+1) -- ...
				glNormal3dv(&(eval_N[loc*DIM]));
				glVertex4dv(&(eval_P[loc*DIM]));
			}
			else {
				loc = (i+1)*(pts+1)+j;            // (i+1,j)--(i+1, j+1) -- ...
				glNormal3dv(&(eval_N[loc*DIM]));
				glVertex4dv(&(eval_P[loc*DIM]));
				loc = i*(pts+1)+j;                // (i,j)----(i, j+1) -- ...
				glNormal3dv(&(eval_N[loc*DIM]));  //   |         |
				glVertex4dv(&(eval_P[loc*DIM]));  //   |         |
			}
		}
		glEnd();
      }

   glPopAttrib();
}


///////////////////////////////////////////////////////////
//
// Evaluate the quadrilateral Bezier patch
//
void QuadBezier::evaluate_patch(int subDepth)
{
	int     size;
	int     Cu, Cv, st, C;
	int     sizeu, sizev, bigstepu, bigstepv;
	int     i, r, rs, r1, r2, c, loc;
	double  h;
	VEC*    bb;

	pts  = 1 << subDepth;

	// allocate the memory for the result of evaluation 
	C    = pts+1;
	size = C*C;            // how big should the array be
	eval_P = alloc_mem_db(size*DIM);
	eval_N = alloc_mem_db(size*DIM);
	crv_array = alloc_mem_db(size*4);  // 4 types of curvatures

	// allocate a temporary memory to perform subdivision
	//    subdivision VS de Casteljau ?? 
	//    
	//    now subdivision because the code already exists, 
	//   
	//    De Casteljau clearly do not need this temporary memory,
	//    but maybe slower compare to this code.
	//
    st = pts;         // original space between two coefficients

						// it is set to 'pts' so that after subdivision
						// the memory becomes tight

    sizeu = st*degu;  // size for both directions
    sizev = st*degv;
    Cu = sizeu+1;       // 0,0     ..  0,sizeu 
    Cv = sizev+1;       // sizev,0 .. sizev, sizev

    bb = (VEC *) alloc_mem_db( Cu * Cv * DIM);

    // BBcopy4(PAcopy4) -- copy the original data into the sparse array
    BBcopy4( coeff, degu, degv, pts, bb);

    // subdivision
    for (i=0; i <subDepth; i++)
    {
        RSubDiv(bb, st, degu, degv, sizeu,sizev);
        st = st/2;  // distance halves after each subdivision
    }

//    bigstepu = st*degu;	/* distance between patches -> column direction */
//    bigstepv = st*degv;	/* distance between patches -> row direction */

    bigstepu = degu;	/* distance between patches -> column direction */
    bigstepv = degv;	/* distance between patches -> row direction */

    // st==1 
    for (r=0; r<sizev; r += bigstepv)  // row
    {
        rs = r*Cu;
        r1 = (r+st)*Cu;
        r2 = (r+2*st)*Cu;
        for (c = 0; c<sizeu; c += bigstepu) {   // column
            loc = (c/bigstepu*C + r/bigstepv) ;
			// curvature
            h = crv4(bb[rs+c],bb[rs+c+st],bb[rs+c+2*st], // curvature
                bb[r1+c],bb[r2+c],bb[r1+c+st],degu, degv, &crv_array[loc*4]);

			evalPN(bb[rs+c], bb[r1+c], bb[rs+c+st], &eval_P[loc*DIM],
							&eval_N[loc*DIM]);
            //printf (" %d %d %d %d %d %d \n", rs+c, rs+c+st, rs+c+2*st,
            //      r1+c, r2+c, r1+c+st);
        }

        // last col _| note: stencil is rotated by 90 degrees c = sizeu;
        loc = (c/bigstepu*C + r/bigstepv) ;
        h =crv4(bb[rs+c],bb[r1+c],bb[r2+c], bb[rs+c-st],
			bb[rs+c-2*st],bb[r1+c-st],degv, degu, &crv_array[loc*4]);

		evalPN(bb[rs+c], bb[rs+c-st], bb[r1+c], &eval_P[loc*DIM],
							&eval_N[loc*DIM]);

    }
      // top row |-  
    r = sizev;
    rs = r*Cu;
    r1 = (r-st)*Cu;
    r2 = (r-2*st)*Cu;
    for (c = 0; c<sizeu; c += bigstepu) {
        loc = (c/bigstepu*C + r/bigstepv) ;
        h =crv4(bb[rs+c],bb[r1+c],bb[r2+c], bb[rs+c+st],  	// curvature
			bb[rs+c+2*st],bb[r1+c+st],degv, degu, &crv_array[loc*4]);

		evalPN(bb[rs+c], bb[rs+c+st], bb[r1+c], &eval_P[loc*DIM],
							&eval_N[loc*DIM]);

    }

      // top right -|  
    c = sizeu;
    loc = (c/bigstepu*C + r/bigstepv) ;
    h = crv4(bb[rs+c],bb[rs+c-st],bb[rs+c-2*st], bb[r1+c],  // curvature
			bb[r2+c], bb[r1+c-st],degu, degv, &crv_array[loc*4]); 

	evalPN(bb[rs+c], bb[r1+c], bb[rs+c-st],  &eval_P[loc*DIM],
							&eval_N[loc*DIM]);

	free(bb);  // free the space used for subdivision

	// evaluate the artificial normals if necessary
	if(art_normal && use_art_normal) {
		printf("using artificial normals\n");
		for(r=0;r<=pts;r++) {
			double u = 1-(double)r/pts;
		    for(c=0;c<=pts;c++) {
			    double v = 1-(double)c/pts;
				loc = r*C+c;
				//printf("loc = %d, u=%f, v=%f\n", loc, u, v);
				DeCastel2(norm, Ndegu, Ndegv, u, v, &eval_N[loc*DIM]);
				Normalize(&eval_N[loc*DIM]);
			}
		}
	}

	// set the evaluated flag to be true
	evaluated = true;
	normal_flipped = false;
}


// flip the normal direction
void QuadBezier::flip_normal()
{
	int     i, m, C, size;
	if(!evaluated) {
		return;
	}

	// size of the normal array
	C    = pts+1;
	size = C*C;    

	// reverse the normals
	for(i=0;i<size;i++)
		for(m=0;m<3;m++)
			eval_N[i*DIM+m] = -eval_N[i*DIM+m];

	normal_flipped = !normal_flipped;
}

/////////////////////////////////////////////////////////////////////////
//
// draw the control polygon
//
// 
void QuadBezier::plot_mesh(float* bg_color)
{
    int   i,j;
   	int loc;

	glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT);
	if(bg_color) {   // if hidden line removal
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
	    glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glColor3fv(bg_color);

 	    for(i=0;i<degu;i++) {
			glBegin(GL_QUADS);
			for(j=0;j<degv;j++)       // this loop will draw the quad strip:
			{
				loc = (i)*(degv+1)+j;            // (i+1,j)--(i+1, j+1) -- ...
				glVertex4dv(&(coeff[loc*DIM]));
				loc = (i+1)*(degv+1)+j;                // (i,j)----(i, j+1) -- ...
				glVertex4dv(&(coeff[loc*DIM]));  //   |         |
				loc = (i+1)*(degv+1)+j+1;            // (i+1,j)--(i+1, j+1) -- ...
				glVertex4dv(&(coeff[loc*DIM]));
				loc = i*(degv+1)+j+1;                // (i,j)----(i, j+1) -- ...
				glVertex4dv(&(coeff[loc*DIM]));  //   |         |
			}
			glEnd();
		}
		glDisable(GL_POLYGON_OFFSET_FILL);	
		glPopAttrib();
	}

	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  for(i=0;i<degu;i++) {
//		  glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
        for(j=0;j<degv;j++)       // this loop will draw the quad strip:
        {
				loc = (i)*(degv+1)+j;            // (i+1,j)--(i+1, j+1) -- ...
				glVertex4dv(&(coeff[loc*DIM]));
				loc = (i+1)*(degv+1)+j;                // (i,j)----(i, j+1) -- ...
				glVertex4dv(&(coeff[loc*DIM]));  //   |         |
				loc = (i+1)*(degv+1)+j+1;            // (i+1,j)--(i+1, j+1) -- ...
				glVertex4dv(&(coeff[loc*DIM]));
				loc = i*(degv+1)+j+1;                // (i,j)----(i, j+1) -- ...
				glVertex4dv(&(coeff[loc*DIM]));  //   |         |
		}
		glEnd();
	  }
	  glPopAttrib();
}

//////////////////////////////////////////////////////////////////////// 
//
//  plot_crv : plot the curvature for the quadrilateral patch
//
void QuadBezier::plot_crv(int crv_choice)
{
    int   i,j;
    int     loc;  
	REAL h;

	// evaluate the patch first if needed
	if(!evaluated) {
		return;
//		evaluate_patch();
	}

	glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);  // curvature is shown with light off

	// almost same as plot_patch expect for use curvature as color
	for(i=0;i<pts;i++) {

		glBegin(GL_QUAD_STRIP);
        for(j=0;j<=pts;j++)       // this loop will draw the quad strip:
        {
		    loc = i*(pts+1)+j;                // (i,0)----(i, 1) -- ...
		    h = get_crv(crv_array, loc, crv_choice); //      |

			// switch the sign if normal flipped   |         |
			// except for Gaussian curvature
			if(normal_flipped && crv_choice != GAUSS_CRV  && crv_choice != SPECIAL_CRV)
				h = -h;

//			printf("h=%f\n", h);
			glColor3fv( crv2color(h));        //   |         |
		    glVertex4dv(&(eval_P[loc*DIM]));  //   |         |
                                              //   |         |
		    loc = (i+1)*(pts+1)+j;            // (i+1,0)--(i+1,1) -- ...
		    h = get_crv(crv_array, loc, crv_choice);

			// switch the sign if normal flipped 
			// except for Gaussian curvature
			if(normal_flipped && ( crv_choice != GAUSS_CRV && crv_choice != SPECIAL_CRV ))
				h = -h;

		    glColor3fv( crv2color(h));
		    glVertex4dv(&(eval_P[loc*DIM]));
		}
		glEnd();
    }

	glPopAttrib();
}

//////////////////////////////////////////////////
// For normal clipping -- Added May 06 2004
extern int normal_clipping ;
int point_clipped(REAL* point);


/////////////////////////////////////////////////////////
//
//  plot the needles representing the curvature
//
void QuadBezier::plot_crv_needles(int crv_choice, REAL length)
{
	int i,j, loc;
	REAL h;
	int st = 1; //pts;

    glDisable(GL_LIGHTING);  // curvature is shown with light off
	for(i=0;i<=pts;i+=st) {
        for(j=0;j<=pts;j+=st)       
        {
			VEC sum;
		    loc = i*(pts+1)+j;             
			if(normal_clipping && !point_clipped(&eval_P[loc*DIM])) {

			h = get_crv(crv_array, loc, crv_choice);

		    glColor3fv( crv2color(h));   // use the color of the curvature
			glBegin(GL_LINES);
		    //glVertex3dv(&(eval_P[loc*DIM]));

			VVadd(1.0, &(eval_P[loc*DIM]), 0.00, &(eval_N[loc*DIM]),sum);
		    glVertex3dv(sum); 
			VVadd(1.0, &(eval_P[loc*DIM]), h*length, &(eval_N[loc*DIM]),
					sum);
		    glVertex3dv(sum); 
			glEnd();
			}
		}
		glEnd();
    }
    glEnable(GL_LIGHTING);  // curvature is shown with light off
}

//////////////////////////////////////////////////////////////////////// 
//
//  plot_highlights : plot the highlights for the quadrilateral patch
//
void QuadBezier::plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type)
{
    int  i,j,k;
    int  loc[4];  
	REAL P[4*DIM], N[4*DIM];

	// evaluate the patch first if needed
	if(!evaluated) {
		return;
//		evaluate_patch();
	}

    //glDisable(GL_LIGHTING);  // highlight is shown with light off
	for(i=0;i<pts;i++) 
        for(j=0;j<pts;j++)       // this loop will draw the quad 
    {
		if(normal_flipped) {
			loc[0] = i*(pts+1)+j;          // (i,j)----(i, j+1) 
			loc[1] = i*(pts+1)+(j+1);      //   |         |
			loc[2] = (i+1)*(pts+1)+(j+1);  //   |         |
			loc[3] = (i+1)*(pts+1)+j;      //   |         |
										   // (i+1,j)--(i+1, j+1)
		}
		else
		{
			loc[3] = i*(pts+1)+j;          // (i,j)----(i, j+1) 
			loc[2] = i*(pts+1)+(j+1);      //   |         |
			loc[1] = (i+1)*(pts+1)+(j+1);  //   |         |
			loc[0] = (i+1)*(pts+1)+j;      //   |         |
										   // (i+1,j)--(i+1, j+1)
		}

        for (k=0; k<4; k++) {
			Vcopy( &eval_P[loc[k]*DIM], &P[k*DIM]);
			Vcopy( &eval_N[loc[k]*DIM], &N[k*DIM]);
        }
	
		Highlight(4, P, N, A, H, hl_step, highlight_type);
	}

    //glEnable(GL_LIGHTING);  // turn the light back on
}

/* SUBDIVIDE RECTANGULAR PATCHES */
/* coefficients are in an array of size "size"
 * spaced "step" units apart
 *
 * bb -- Bezier coefficients
 * step -- space between two coefficients 
 * deg -- degree of the bezier patch
 * size -- number of columns in the array
 *
 */
void RSubDiv(VEC bb[], int step, int degu, int degv, int sizeu, int sizev)
{    
    int 	m,k,l;
    int 	row,col,
		C;
    int 	h,h1,h2,st2,bigstepu, bigstepv,
		i1,i2,i3;

    st2 = step/2;
    bigstepu = step*degu;
    bigstepv = step*degv;
    C = sizeu+1;

    for (row=0; row<sizev; row += bigstepv)   /* patch-level */
	for (col=0; col<=sizeu; col += step)  {
	    /* subdivide a curve-column of degree degv */
	    for (l=0; l<degv; l++)  {	/*levels of subdiv. triangle */
		h = row + l*st2;
		for (k=0; k<(degv-l); k++)  {
		    h1= h + step;
		    h2= h + st2; 
		    i1 = h2*C+col;
		    i2 = h*C+col;
		    i3 = h1*C+col;
		    for (m=0; m<DIM; m++)
			bb[i1][m] = (bb[i2][m] + bb[i3][m])/2;
		    h = h1;
		}
	    }
	}
    for (col=0; col<sizeu; col += bigstepu)   /* 2 x patch-level */
	for (row=0; row<=sizev; row += st2)  {
	    /* subdivide a curve-row of degree deg */
	    for (l=0; l<degu; l++)  {	/*levels of subdiv. triangle */
		h = col + l*st2;
		for (k=0; k<(degu-l); k++)  {
		    h1= h + step;
		    h2= h + st2; 
		    i1 = row*C+h2;
		    i2 = row*C+h;
		    i3 = row*C+h1;
		    for (m=0; m<DIM; m++) 
			bb[i1][m] = (bb[i2][m] + bb[i3][m])/2;
		    h = h1;
		}
	    }
	}
}

/* De Casteljau algorithm for tensor-product function */
void DeCastel2(GLdouble* coeff, int degu, int degv, 
				double u, double v, GLdouble* pts)
{
	int i,j;
    REAL *Ubuffer = alloc_mem_db((degu+1)*DIM);
    REAL *Vbuffer = alloc_mem_db((degv+1)*DIM);

	// calculate Du
    for (i= 0;  i<=degu; i++)
    {
        for (j= 0; j<=degv; j++)
			Vcopy(&coeff[(i*(degv+1)+j)*DIM], &Vbuffer[j*DIM]);

        DeCastel1(Vbuffer, degv, v, &Ubuffer[i*DIM]);
    }
    DeCastel1(Ubuffer, degu, u, pts);
	free(Ubuffer);
	free(Vbuffer);
}

/* De Casteljau algorithm for 1 variable function */
void DeCastel1(GLdouble* coeff, int deg, double u, GLdouble* pts)
{
    double u1 = 1-u;
    int d, i, m;

    for(d=deg;d>1;d--)
        for(i=0;i<d;i++)
            for(m=0;m<DIM;m++)
                coeff[i*DIM+m] = u*coeff[i*DIM+m] + u1*coeff[(i+1)*DIM+m];

    for(m=0;m<DIM;m++)
        pts[m] = u*coeff[m] + u1*coeff[1*DIM+m];
}



/* get a corner control point */
REAL* QuadBezier::get_v(int s)
{
    switch(s) {
	case 0:
        return get_bb(0, 0);
	case 1:
        return get_bb(degu, 0);
	case 2:
        return get_bb(degu, degv);
	case 3:
        return get_bb(0, degv);
	default:
		return NULL;
	}
}

//
// copy bb coefficients from tight array buf into sparse array bb
//  st: step
//  degu, degv: degrees for bb
//
void BBcopy4( REAL * buf, int degu, int degv, int st, VEC * bb)
{
    int i,j;
    int C;

    // buf is arranged by
    //  columns -- v, rows -- u
    //    p11       p12    .... p1(degv+1)
    //    p21       p22    .... p2(degv+1)
    //     ..       ..     ....    ..
    // p(degu+1)1 p(degu+1)1 .. p(degu+1)(degv+1)

    C = st*degu +1;
    for (i=0; i<=degu; i++) {
        for (j=0; j<=degv; j++) {
            double *v;
            v = buf + (i*(degv+1) + j) * DIM;
            Vcopy(v, bb[(j* st)* C + i*st]);
        }
    }
}
