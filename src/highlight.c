/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    highlight.cpp
 *  Purpose: render highlights for bezier patches
 *  Author   : Xiaobin Wu
 * ------------------------------------------------------------
 */
#include "stdheaders.h"
#include "util.h"
#include "type.h"

#define HIGHLIGHTLINE 0
#define REFLECTLINE   1

int hl_error;

real calc_D(vector P, vector N, vector A, vector H){
    // // according to Beier/Chen's 1994 CAD paper
	/*

				 [ H X N(u,v)] dot [A - P(u,v)]
		D(u,v) = --------------------------------
							|| H X N(u,v) ||
        N and P are normals and locations on the surface at (u,v)
        H is the normalized direction of the light source line
        A is the starting point of the light source line

    */

    vector SA;
    vector temp;
    int m;
    real div;

    for(m=0;m<DIM;m++)
        SA[m] = A[m]-P[m];

    VVcross(H, N, temp);
	div = Norm(temp);    // according to Beier/Chen's 1994 CAD paper

    if(fabs(div)< tol)
	{
//		printf("Warning: divided by zero\n");
		hl_error = 1;
        return 0;
	}
    else {
		hl_error = 0;
        return (VVmult(temp,SA) / div);
    }
}

// 
real calc_ref_line(vector P, vector N, vector A, vector H, vector eye)
{
	vector RefN;
	real th;
    vector SA;
	int m;
    
	for(m=0;m<DIM;m++)
        SA[m] = A[m]-P[m];

	Normalize(SA);
	th = VVmult(SA, N);

	for(m=0;m<DIM;m++)
        RefN[m] = 2*(th*N[m])-SA[m];
	return calc_D( P, RefN, A, H);
}



/* initialize the texture used in reflection line ploting */
void init_texture(GLubyte* forecolor, GLubyte* backcolor)
{
	int j;
 
	#define	stripeImageWidth 32
	GLubyte stripeImage[4*stripeImageWidth];

	/* texture image */
    for (j = 0; j < stripeImageWidth; j++) {
		// foreground color
	    if(j<2.0/3*stripeImageWidth && j>1.0/3*stripeImageWidth) {
			stripeImage[4*j]   = forecolor[0];
			stripeImage[4*j+1] = forecolor[1];
	        stripeImage[4*j+2] = forecolor[2];;
		}
		else {		// background color
		    stripeImage[4*j]   = backcolor[0];
		    stripeImage[4*j+1] = backcolor[1];
		    stripeImage[4*j+2] = backcolor[2];
		}
      stripeImage[4*j+3] = (GLubyte) 255;
   }
   
   glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // <--- stripImage: 4bytes

   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // We don't combine the color with the original surface color, use only the texture map.
   //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // We don't combine the color with the original surface color, use only the texture map.

}

////////////////////////////////////////////////////////////////
//
//  plot the high light
//
void Highlight(int n, vector* P, vector* N, vector A, vector H, real hl_step, int highlight_type) {

	real func[40];
    int i;

	real eye[3] = {0, 0, 1000};
	// calculate the highlight values according to point and normal
	for(i = 0; i<n; i++) 
	{
		if(highlight_type == HIGHLIGHTLINE) {
            func[i] = calc_D( P[i], N[i], A, H);
            //if (calc_D( P[i], N[i], A, H, &func[i]))
			if (hl_error)	
				return; // return if the patch is numerically unstable,
		}
		else {
            func[i] = calc_ref_line( P[i], N[i], A, H, eye);
            //if (calc_ref_line( P[i], N[i], A, H, eye, &func[i]))
			if (hl_error)	
				return; // return if the patch is numerically unstable,
		}
	}


    glEnable(GL_TEXTURE_1D);
    glDisable(GL_LIGHTING);
    glEnable(GL_LIGHTING);

	glBegin(GL_POLYGON);
    for(i=0;i<n;i++) {
		real f = func[i];
		real color;

		color = f/hl_step;

		glTexCoord1d(color);
        glNormal3dv(N[i]);
        glVertex4dv(P[i]);
	}
	glEnd();

    glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_1D);
	
}



// solve a 4x4 linear system
// i.e. Ay=x where A is a 4x4 matrix, x is a length 4 vector
//
// USED for: solving the correct light source direction after rotation.
//  i.e. A : current modelview matrix,
//       x : initial light source
//    return x: current light source
void Solve4(double * A, double* x)
{
	double B[16];
	int i,j;
	double y[4];
	double det, dem;

	det = det4( A[0], A[4], A[8], A[12],
				A[1], A[5], A[9], A[13],
				A[2], A[6], A[10], A[14],
				A[3], A[7], A[11], A[15]);

	for(i=0;i<4;i++) {
		for(j=0;j<16;j++)
			B[j] = A[j];
		
		for(j=0;j<4;j++) 
			B[i*4+j] = x[j];

		dem = det4( B[0], B[4], B[8], B[12],
				B[1], B[5], B[9], B[13],
				B[2], B[6], B[10], B[14],
				B[3], B[7], B[11], B[15]);
		y[i] = dem/det;
	}

	for(i=0;i<4;i++)
		x[i] = y[i];
}



