/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    TriBezier.cpp
 *  Purpose: render Triangular Bezier patches
 *  Author   : Xiaobin Wu
 * ------------------------------------------------------------
 */
#include "stdheaders.h"
extern "C" {
#include "util.h"
#include "curvature.h"
#include "highlight.h"
}

#include "TriBezier.h"



////////////////////////////////////////////////////////////////
//
// load the Bezier data from a data file
//
int TriBezier::loadFile(FILE* fp, bool art_normal) {
	int i, m;

	//printf("start evaluated : %d\n", evaluated);
    fscanf(fp,"%d", &deg);
    num_points = (deg+2)*(deg+1) /2;

	// if artificial normal exists, read in the degree 
	if(art_normal) {
        fscanf(fp,"%d", &Ndeg);
        num_normals = (Ndeg+2)*(Ndeg+1) /2;
	}

    // read in all control points
    coeff = alloc_mem_db(num_points*DIM);    // allocate memory 
    for (i=0;i<num_points;i++) {
 	    for (m=0;m<3;m++)
            fscanf(fp,"%lg", &coeff[i*DIM+m]);

		if (DIM==4)
	    	coeff[i*DIM+3] = 1.0;

        // adjust view volume to contain the point
        enlarge_aabb(coeff[i*DIM+0]/coeff[i*DIM+3],
           coeff[i*DIM+1]/coeff[i*DIM+3], coeff[i*DIM+2]/coeff[i*DIM+3]);

    }

    // read in all coefficients of the normal function
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
	return 0;
};


////////////////////////////////////////////////////////////////////
//
//   draw the patch 
//
void TriBezier::plot_patch(bool smooth)
{
	int i,j;
	int loc;

	// evaluate the patch first if needed
	if(!evaluated) {
		return;
		//evaluate_patch();
	}

	glPushAttrib(GL_LIGHTING_BIT);

	if(!smooth) {
		glShadeModel(GL_FLAT);
	}

	// the plot of the patch consists of a set of triangular strips
	// 
	// draw a strip for each i in [0,pts)
	for(i=0;i<pts;i++) {

		glBegin(GL_TRIANGLE_STRIP);
        for(j=0;j<pts-i;j++)
        {

			if(!normal_flipped) {  // reverse the orientation of the patch
				loc = b2i_i(i+1, j, pts-i-j-1, pts);
				glNormal3dv(&(eval_N[loc*DIM]));
				glVertex4dv(&(eval_P[loc*DIM]));

				loc = b2i_i(i, j, pts-i-j, pts);
				glNormal3dv(&(eval_N[loc*DIM]));
				glVertex4dv(&(eval_P[loc*DIM]));
			}
			else
			{
				loc = b2i_i(i, j, pts-i-j, pts);
				glNormal3dv(&(eval_N[loc*DIM]));
				glVertex4dv(&(eval_P[loc*DIM]));

				loc = b2i_i(i+1, j, pts-i-j-1, pts);
				glNormal3dv(&(eval_N[loc*DIM]));
				glVertex4dv(&(eval_P[loc*DIM]));
			}
		}

		// finish the strip by adding the last triangle
		loc = b2i_i(i, j, pts-i-j, pts);
		glNormal3dv(&(eval_N[loc*DIM]));
		glVertex4dv(&(eval_P[loc*DIM]));
		glEnd();
    }

	glPopAttrib();
}

 
/////////////////////////////////////////////////////////////////////////
//
// draw the control polygon
//
// 
void TriBezier::plot_mesh(float* bg_color)
{
    int   i,j,k;
    int   d = deg-1;

    glBegin(GL_POINTS);
    for(i=0; i<((deg+1)*(deg+2)/2); i++)
       glVertex4dv(&coeff[i*DIM]);
    glEnd();

    for(i=0;i<=d;i++)
        for(j=0;j<=d-i;j++)
    {
        k = d-i-j;
        glBegin(GL_LINE_LOOP);
        glVertex4dv(&coeff[b2i_i(i+1,j,k,deg)*DIM]);
        glVertex4dv(&coeff[b2i_i(i,j+1,k,deg)*DIM]);
        glVertex4dv(&coeff[b2i_i(i,j,k+1,deg)*DIM]);
        glEnd();
    }
}

///////////////////////////////////////////////////////////
//
// Evaluate the triangular bezier patch
//
void TriBezier::evaluate_patch(int subDepth)
{
    GLint   i,j,k;
    GLint   d,m;
    GLdouble u, v, w; /* parameter of patch */ 
    GLint   uu, vv;
    int     loc = 0;   /* increase 1 for each point computed */
	int     size;

    //printf("evaluate starts\n");

    GLdouble DeCastel[(MAXDEG+1)*(MAXDEG+2)/2][DIM];

    if(deg>MAXDEG) {
		printf("Maximum degree %d reached, please increase the number.\n", MAXDEG);
        exit(0);
    }

    int (*b2i)(int i, int j, int k, int d);

	pts  = 1 << subDepth;

	/* allocate the memory for evaluation */
	size = (pts+1)*(pts+2)/2;
	eval_P = alloc_mem_db(size*DIM);
	eval_N = alloc_mem_db(size*DIM);
	crv_array = alloc_mem_db(size*4);

    for (uu=0; uu<=pts; uu++)
    {
        for (vv=0;vv<=pts-uu;vv++)
        {
            GLdouble Point[DIM];
            double h;
            GLdouble *V00,*V01, *V02,*V10, *V20, *V11;

            int onbdy = (uu==0) ;  // on the boundary
            int atvtx = (uu==0 && vv==0); 

            u = (double)uu/pts;
            v = (double)vv/pts;
            w = 1-u-v;

            // use two different mapping functions
            //  for the interior and the boundary
            if (atvtx)
                b2i = b2i_k;
            else if (onbdy)    
                b2i = b2i_j;
            else
                b2i = b2i_i;

            /* initialize the DeCastel Array */
            for(i=0;i<=deg;i++)
                for(j=0;j<=deg-i;j++)
            {
                k = deg -i-j;
                for( m = 0; m <DIM; m++)
                    DeCastel[(*b2i)(i,j,k, deg)][m] = 
                        coeff[(*b2i)(i,j,k, deg)*DIM+m]; 
            }

            /* de Casteljau algorithm */
            for (d = deg-1 ; d >=1; d--)
            {
                for(k=0;k<=d;k++)
                    for(j=0;j<=d-k;j++)
                {
                    i = d-j-k;
                    for (m=0;m<DIM;m++)
                       DeCastel[(*b2i)(i,j,k,deg)][m] =
                        u* DeCastel[(*b2i)(i+1,j,k,deg)][m] +
                        v* DeCastel[(*b2i)(i,j+1,k,deg)][m] +
                        w* DeCastel[(*b2i)(i,j,k+1,deg)][m];
                }
            }

            /* Last step of de Casteljau algorithm */
            for(m=0;m<DIM;m++)
                 Point[m] = u* DeCastel[(*b2i)(1,0,0, deg)][m] +
                             v* DeCastel[(*b2i)(0,1,0,deg)][m] +
                                w* DeCastel[(*b2i)(0,0,1,deg)][m];

            //V00   = DeCastel[(*b2i)(0,0,0,deg)];
            V00   = Point;
            if (atvtx )   {
                V01   = DeCastel[(*b2i)(0,1,0,deg)];
                V02   = DeCastel[(*b2i)(0,2,0,deg)];
                V10   = DeCastel[(*b2i)(1,0,0,deg)];
                V20   = DeCastel[(*b2i)(2,0,0,deg)];
                V11   = DeCastel[(*b2i)(1,1,0,deg)];
            }
            else if (onbdy )   
            {
                V01   = DeCastel[(*b2i)(1,0,0,deg)];
                V02   = DeCastel[(*b2i)(2,0,0,deg)];
                V10   = DeCastel[(*b2i)(0,0,1,deg)];
                V20   = DeCastel[(*b2i)(0,0,2,deg)];
                V11   = DeCastel[(*b2i)(1,0,1,deg)];
                //printf("On boundary\n");
            }
            else
            {
                V01   = DeCastel[(*b2i)(0,0,1,deg)];
                V02   = DeCastel[(*b2i)(0,0,2,deg)];
                V10   = DeCastel[(*b2i)(0,1,0,deg)];
                V20   = DeCastel[(*b2i)(0,2,0,deg)];
                V11   = DeCastel[(*b2i)(0,1,1,deg)];
            }

			// compute the point and the normal at the (u,v) parameter
            evalPN(V00, V01, V10, &eval_P[loc*DIM], &eval_N[loc*DIM]);

			// compute the curvatures (Gaussian, mean, min and max)
			// at the (u,v) parameter
            h = crv3 (V00, V01, V02, V10, V20, V11, deg, &crv_array[loc*4]);

            //printf("value %f at %d \n", h, loc);
            loc ++;
        }
    }

	// evaluate the artificial normals if necessary
	if(art_normal ) {
	    loc = 0;
        for (uu=0; uu<=pts; uu++) {
        for (vv=0;vv<=pts-uu;vv++) {
            u = (double)uu/pts;
            v = (double)vv/pts;
            w = 1-u-v;
			//printf("loc = %d, u = %f, v= %f, w= %f\n", loc, u, v, w);

            /* initialize the DeCastel Array */
            for(i=0;i<=Ndeg;i++)
                for(j=0;j<=Ndeg-i;j++)
            {
                k = Ndeg -i-j;
                for( m = 0; m <DIM; m++)
                    DeCastel[(*b2i)(i,j,k, Ndeg)][m] = 
                        norm[(*b2i)(i,j,k, Ndeg)*DIM+m]; 
            }

            /* de Casteljau algorithm */
            for (d = Ndeg-1 ; d >=0; d--)
            {
                for(k=0;k<=d;k++)
                    for(j=0;j<=d-k;j++)
                {
                    i = d-j-k;
                    for (m=0;m<DIM;m++)
                       DeCastel[(*b2i)(i,j,k,Ndeg)][m] =
                        u* DeCastel[(*b2i)(i+1,j,k,Ndeg)][m] +
                        v* DeCastel[(*b2i)(i,j+1,k,Ndeg)][m] +
                        w* DeCastel[(*b2i)(i,j,k+1,Ndeg)][m];
                }
            }
			Vcopy(DeCastel[(*b2i)(0,0,0,Ndeg)], &eval_N[loc*DIM]);
			Normalize(&eval_N[loc*DIM]);
			loc++;
		}
		}
	}
	evaluated = true;
	normal_flipped = false;
	//flip_normal(); // why?
}


void TriBezier::flip_normal()
{
	int     i, m, size;

	if(!evaluated) {
		return;
	}

	size = (pts+1)*(pts+2)/2;

	for(i=0;i<size;i++)
		for(m=0;m<3;m++)
			eval_N[i*DIM+m] = -eval_N[i*DIM+m];

	normal_flipped = !normal_flipped;
}

//////////////////////////////////////////////////////////////////////// 
//
//  plot_crv : plot the curvature for the triangle
void TriBezier::plot_crv(int crv_choice)
{
    int   i,j;
    int     loc;  
	REAL h;

	glPushAttrib(GL_ENABLE_BIT);

	// evaluate the patch first if needed
	if(!evaluated) {
		return;
	}
//		evaluate_patch();

    glDisable(GL_LIGHTING);  // curvature is shown with lighting off

	for(i=0;i<pts;i++) {
		glBegin(GL_TRIANGLE_STRIP);
        for(j=0;j<pts-i;j++)
        {
		    loc = b2i_i(i, j, pts-i-j, pts);
			// pick out the curvature from the curvature array
		    h = get_crv(crv_array, loc, crv_choice);

			// switch the sign if normal flipped except for Gaussian curvature
			if(normal_flipped && crv_choice != GAUSS_CRV && crv_choice != SPECIAL_CRV) 
				h = -h;

//			printf("h=%f\n", h);
		    glColor3fv( crv2color(h));
		    glVertex4dv(&(eval_P[loc*DIM]));

		    loc = b2i_i(i+1, j, pts-i-j-1, pts);
		    h = get_crv(crv_array, loc, crv_choice);

			// switch the sign if normal flipped except for Gaussian curvature
			if(normal_flipped && crv_choice != GAUSS_CRV && crv_choice != SPECIAL_CRV)
				h = -h;

//			printf("h=%f\n", h);
		    glColor3fv( crv2color(h));
		    glVertex4dv(&(eval_P[loc*DIM]));
		}

		// finish the strip by adding the last triangle
		loc = b2i_i(i, j, pts-i-j, pts);
	    h = get_crv(crv_array, loc, crv_choice);

		// switch the sign if normal flipped except for Gaussian curvature
		if(normal_flipped && crv_choice != GAUSS_CRV  && crv_choice != SPECIAL_CRV)
			h = -h;

		glColor3fv( crv2color(h));
		glVertex4dv(&(eval_P[loc*DIM]));
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
void TriBezier::plot_crv_needles(int crv_choice, REAL length=1.0)
{
	int i,j, loc;
	REAL h;             

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);  // curvature is shown with lighting off
	for(i=0;i<=pts;i++) {
		for(j=0;j<=pts-i;j++)     
		{
			VEC sum;
			loc = b2i_i(i, j, pts-i-j, pts);

			if(normal_clipping && !point_clipped(&eval_P[loc*DIM])) {

		    h = get_crv(crv_array, loc, crv_choice);
			VVadd(1.0, &(eval_P[loc*DIM]), h*length, &(eval_N[loc*DIM]),
				sum);                              
			glColor3fv( crv2color(h));      

			glBegin(GL_LINES);                         
			glVertex3dv(&(eval_P[loc*DIM]));
			glVertex3dv(sum);  
			glEnd();                                   
			}
		}                                              
	}       
	glPopAttrib();
}                                                   

////////////////////////////////////////////////////////////////////
//
//   draw the highlight lines
//
void TriBezier::plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type)
{
	REAL P[3*DIM];
	REAL N[3*DIM];
	int i, j, k;
	int loc[3];

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);
	for(i=0;i<pts;i++)
        for(j=0;j<pts-i;j++)
    {
		if(normal_flipped) {
			loc[0] = b2i_i(i, j, pts-i-j, pts);
			loc[1] = b2i_i(i+1, j, pts-i-j-1, pts);
			loc[2] = b2i_i(i,j+1, pts-i-j-1, pts); 
		}
		else {
			loc[2] = b2i_i(i, j, pts-i-j, pts);
			loc[1] = b2i_i(i+1, j, pts-i-j-1, pts);
			loc[0] = b2i_i(i,j+1, pts-i-j-1, pts); 
		}

        for (k=0; k<3; k++) {
			Vcopy( &eval_P[loc[k]*DIM], &P[k*DIM]);
			Vcopy( &eval_N[loc[k]*DIM], &N[k*DIM]);
        }
		Highlight(3, P, N, A, H, hl_step, highlight_type);
	}
    
    for(i=0;i<pts-1;i++)
        for(j=0;j<pts-1-i;j++)
    {
		if(normal_flipped) {
			loc[0] = b2i_i(i, j+1, pts-i-j-1, pts);
			loc[1] = b2i_i(i+1,j, pts-i-j-1, pts);
			loc[2] = b2i_i(i+1, j+1, pts-i-j-2, pts);
		}
		else {
			loc[2] = b2i_i(i, j+1, pts-i-j-1, pts);
			loc[1] = b2i_i(i+1,j, pts-i-j-1, pts);
			loc[0] = b2i_i(i+1, j+1, pts-i-j-2, pts);
		}

        for (k=0; k<3; k++) {
			Vcopy( &eval_P[loc[k]*DIM], &P[k*DIM]);
			Vcopy( &eval_N[loc[k]*DIM], &N[k*DIM]);
        }
		Highlight(3, P, N, A, H, hl_step, highlight_type);
	}

	glPopAttrib();
}



// barycentral coordinate mapping to 1d array index
// according to j & k
// i.e. when (i,j,k) has the property: (i+j+k<d)
//             (i,j,k)    will go to 
//              /
//          ((d-j-k), j, k)
// 
// this provides a certain way of overwriting points in DeCastejel
// algorithm, so that the intermediate values can be used to
// calculate derivatives and curvatures.

/*
//                  /\ C(i,j,k+1)
//                 /  \
//                /    \
//               /      \      P will overwrite A
//              /    .   \
//             / P(i,j,k) \
//  A(i+1,j,k)/____________\ B(i,j+1,k)
*/

int b2i_i (int i, int j, int k, int d)
{
    int lk = 0;
    int kk = 0;

    for (kk = 0 ; kk <k; kk++)
    {
        lk += (d+1-kk);
    }
    return lk+j;
}

/* mapping function according to i & k
// i.e. when (i,j,k) has the property: (i+j+k<d)
//             (i,j,k)    will go to
//                \ 
//             (i, (d-i-k), k)
//  
// this provides another way of overwriting points in DeCasteljau
//  algorithm.
//  
//  P will overwrite B in same graph in b2i_i
*/
int b2i_j (int i, int j, int k, int d)
{
    int lk = 0;
    int kk = 0;

    // d = i+j+k;
    for (kk = 0 ; kk <k; kk++)
    {
        lk += (d+1-kk);
    }
    return lk+ (d-i-k);
} 

// mapping function according to j & i
// i.e. when (i,j,k) has the property: (i+j+k<d)
//                (i,j,(d-i-j))
//                 /
//             (i,j,k)    will go to
//
// this provides the third way of overwriting points in DeCasteljau
//  algorithm.
//  
//  P will overwrite C in same graph in b2i_i
//
int b2i_k (int i, int j, int k, int d)
{
    int lk = 0;
    int kk = 0;
    k = d-i-j;

    for (kk = 0 ; kk <k; kk++)
    {
        lk += (d+1-kk);
    }
    return lk+j;
}


REAL* TriBezier::get_bb(int i, int j) {
    return ( &coeff[b2i_i(i,j,deg-i-j,deg)*DIM]);
}
