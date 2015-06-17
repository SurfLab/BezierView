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

// index mapping functions
int b2i_j(int i, int j, int k, int d);
int b2i_i(int i, int j, int k, int d);
int b2i_k(int i, int j, int k, int d);


////////////////////////////////////////////////////////////////
//
// load the Bezier data from a data file
//
int TriBezier::loadFile(FILE* fp, bool art_normal) {
	int i, m;

	//printf("start evaluated : %d\n", evaluated);
    fscanf(fp,"%d", &degu);
    pointCount = (degu+2)*(degu+1) /2;

	// if artificial normal exists, read in the degree 
	if(art_normal) {
        fscanf(fp,"%d", &Ndegu);
        normalCount = (Ndegu+2)*(Ndegu+1) /2;
	}

    // read in all control points
    arrcreate(position, pointCount);
    for (i=0;i<pointCount;i++) {
 	    for (m=0;m<3;m++)
            fscanf(fp,"%lg", &position[i][m]);

		if (DIM==4)
            position[i][3] = 1.0;

        // adjust view volume to contain the point
        enlarge_aabb(position[i][0]/position[i][3],
           position[i][1]/position[i][3], position[i][2]/position[i][3]);

    }

    // read in all coefficients of the normal function
	if(art_normal) {
        arrcreate(normal, normalCount);
        for (i=0;i<normalCount;i++) {
 	    	for (m=0;m<3;m++)
                fscanf(fp,"%lg", &normal[i][m]);

			if (DIM==4)
                normal[i][3] = 1.0;
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
                glNormal3dv(&(eval_N[loc][0]));
                glVertex4dv(&(eval_P[loc][0]));

				loc = b2i_i(i, j, pts-i-j, pts);
                glNormal3dv(&(eval_N[loc][0]));
                glVertex4dv(&(eval_P[loc][0]));
			}
			else
			{
				loc = b2i_i(i, j, pts-i-j, pts);
                glNormal3dv(&(eval_N[loc][0]));
                glVertex4dv(&(eval_P[loc][0]));

				loc = b2i_i(i+1, j, pts-i-j-1, pts);
                glNormal3dv(&(eval_N[loc][0]));
                glVertex4dv(&(eval_P[loc][0]));
			}
		}

		// finish the strip by adding the last triangle
		loc = b2i_i(i, j, pts-i-j, pts);
        glNormal3dv(&(eval_N[loc][0]));
        glVertex4dv(&(eval_P[loc][0]));
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
    int   d = degu-1;

    glBegin(GL_POINTS);
    for(i=0; i<((degu+1)*(degu+2)/2); i++)
       glVertex4dv(&position[i][0]);
    glEnd();

    for(i=0;i<=d;i++)
        for(j=0;j<=d-i;j++)
    {
        k = d-i-j;
        glBegin(GL_LINE_LOOP);
        glVertex4dv(&position[b2i_i(i+1,j,k,degu)][0]);
        glVertex4dv(&position[b2i_i(i,j+1,k,degu)][0]);
        glVertex4dv(&position[b2i_i(i,j,k+1,degu)][0]);
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

    if(degu>MAXDEG) {
		printf("Maximum degree %d reached, please increase the number.\n", MAXDEG);
        exit(0);
    }

    int (*b2i)(int i, int j, int k, int d);

	pts  = 1 << subDepth;

	/* allocate the memory for evaluation */
	size = (pts+1)*(pts+2)/2;
    arrcreate(eval_P, size);
    arrcreate(eval_N, size);
    arrcreate(crv_array, size*4);

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
            for(i=0;i<=degu;i++)
                for(j=0;j<=degu-i;j++)
            {
                k = degu -i-j;
                for( m = 0; m <DIM; m++)
                    DeCastel[(*b2i)(i,j,k, degu)][m] =
                        position[(*b2i)(i,j,k, degu)][m];
            }

            /* de Casteljau algorithm */
            for (d = degu-1 ; d >=1; d--)
            {
                for(k=0;k<=d;k++)
                    for(j=0;j<=d-k;j++)
                {
                    i = d-j-k;
                    for (m=0;m<DIM;m++)
                       DeCastel[(*b2i)(i,j,k,degu)][m] =
                        u* DeCastel[(*b2i)(i+1,j,k,degu)][m] +
                        v* DeCastel[(*b2i)(i,j+1,k,degu)][m] +
                        w* DeCastel[(*b2i)(i,j,k+1,degu)][m];
                }
            }

            /* Last step of de Casteljau algorithm */
            for(m=0;m<DIM;m++)
                 Point[m] = u* DeCastel[(*b2i)(1,0,0, degu)][m] +
                             v* DeCastel[(*b2i)(0,1,0,degu)][m] +
                                w* DeCastel[(*b2i)(0,0,1,degu)][m];

            //V00   = DeCastel[(*b2i)(0,0,0,degu)];
            V00   = Point;
            if (atvtx )   {
                V01   = DeCastel[(*b2i)(0,1,0,degu)];
                V02   = DeCastel[(*b2i)(0,2,0,degu)];
                V10   = DeCastel[(*b2i)(1,0,0,degu)];
                V20   = DeCastel[(*b2i)(2,0,0,degu)];
                V11   = DeCastel[(*b2i)(1,1,0,degu)];
            }
            else if (onbdy )   
            {
                V01   = DeCastel[(*b2i)(1,0,0,degu)];
                V02   = DeCastel[(*b2i)(2,0,0,degu)];
                V10   = DeCastel[(*b2i)(0,0,1,degu)];
                V20   = DeCastel[(*b2i)(0,0,2,degu)];
                V11   = DeCastel[(*b2i)(1,0,1,degu)];
                //printf("On boundary\n");
            }
            else
            {
                V01   = DeCastel[(*b2i)(0,0,1,degu)];
                V02   = DeCastel[(*b2i)(0,0,2,degu)];
                V10   = DeCastel[(*b2i)(0,1,0,degu)];
                V20   = DeCastel[(*b2i)(0,2,0,degu)];
                V11   = DeCastel[(*b2i)(0,1,1,degu)];
            }

			// compute the point and the normal at the (u,v) parameter
            evalPN(V00, V01, V10, &eval_P[loc][0], &eval_N[loc][0]);

			// compute the curvatures (Gaussian, mean, min and max)
			// at the (u,v) parameter
            h = crv3 (V00, V01, V02, V10, V20, V11, degu, &crv_array[loc*4]);

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
            for(i=0;i<=Ndegu;i++)
                for(j=0;j<=Ndegu-i;j++)
            {
                k = Ndegu -i-j;
                for( m = 0; m <DIM; m++)
                    DeCastel[(*b2i)(i,j,k, Ndegu)][m] =
                        normal[(*b2i)(i,j,k, Ndegu)][m];
            }

            /* de Casteljau algorithm */
            for (d = Ndegu-1 ; d >=0; d--)
            {
                for(k=0;k<=d;k++)
                    for(j=0;j<=d-k;j++)
                {
                    i = d-j-k;
                    for (m=0;m<DIM;m++)
                       DeCastel[(*b2i)(i,j,k,Ndegu)][m] =
                        u* DeCastel[(*b2i)(i+1,j,k,Ndegu)][m] +
                        v* DeCastel[(*b2i)(i,j+1,k,Ndegu)][m] +
                        w* DeCastel[(*b2i)(i,j,k+1,Ndegu)][m];
                }
            }
            Vcopy(DeCastel[(*b2i)(0,0,0,Ndegu)], &eval_N[loc][0]);
            Normalize(&eval_N[loc][0]);
			loc++;
		}
		}
	}
	evaluated = true;
	normal_flipped = false;
	//flip_normal(); // why?
}

TriBezier::TriBezier(int degree) {
    evaluated = false;
    degu = degree; art_normal = false;
    pointCount = (degu+2)*(degu+1) /2;
    arrcreate(position, pointCount);
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
            eval_N[i][m] = -eval_N[i][m];

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
            glVertex4dv(&(eval_P[loc][0]));

		    loc = b2i_i(i+1, j, pts-i-j-1, pts);
		    h = get_crv(crv_array, loc, crv_choice);

			// switch the sign if normal flipped except for Gaussian curvature
			if(normal_flipped && crv_choice != GAUSS_CRV && crv_choice != SPECIAL_CRV)
				h = -h;

//			printf("h=%f\n", h);
		    glColor3fv( crv2color(h));
            glVertex4dv(&(eval_P[loc][0]));
		}

		// finish the strip by adding the last triangle
		loc = b2i_i(i, j, pts-i-j, pts);
	    h = get_crv(crv_array, loc, crv_choice);

		// switch the sign if normal flipped except for Gaussian curvature
		if(normal_flipped && crv_choice != GAUSS_CRV  && crv_choice != SPECIAL_CRV)
			h = -h;

		glColor3fv( crv2color(h));
        glVertex4dv(&(eval_P[loc][0]));
		glEnd();
    }

    glPopAttrib();
}


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

            if(normal_clipping && !point_clipped(&eval_P[loc][0])) {

		    h = get_crv(crv_array, loc, crv_choice);
            VVadd(1.0, &(eval_P[loc][0]), h*length, &(eval_N[loc][0]),
				sum);                              
			glColor3fv( crv2color(h));      

			glBegin(GL_LINES);                         
            glVertex3dv(&(eval_P[loc][0]));
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
            Vcopy( &eval_P[loc[k]][0], &P[k*DIM]);
            Vcopy( &eval_N[loc[k]][0], &N[k*DIM]);
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
            Vcopy( &eval_P[loc[k]][0], &P[k*DIM]);
            Vcopy( &eval_N[loc[k]][0], &N[k*DIM]);
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
    return ( &position[b2i_i(i,j,degu-i-j,degu)][0]);
}
