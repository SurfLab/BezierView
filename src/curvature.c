/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ. Of Florida
 *
 *  File     : curvature.c
 *  Purpose  : Compute the curvature(Gauss, Mean, Max, Min)
 *  		   also: draw the curvature adjust bar
 *  Author   : Xiaobin Wu
 * -------------------------------------------------------------
 */
#include "stdheaders.h"
#include "util.h"

void print_number(double x, double y, real v, real dist, color_t color);

// ......................................................................
//
// The global variables
//

// curvature minimum and maximum 
real* max_crv_value, * min_crv_value;

// current type of curvature
int crv_style  = 1;

// curvature low and high bounds 
// can be manually set by file IN.crvBounds
double low, hi;

// a special curvature ratio requested by Kestutis karciauskas
double ratio_a =1 , ratio_b=0;

// initial curvature key scale points
GLdouble crv_scale[5] = {0, 0.25, 0.5, 0.75, 1.0};

int freshObject = 1;  // if first patch, min and max value of curvatures are assigned
// the top and bottom of the curvature bar
GLdouble leftC=0.7, widthC  = 0.02;
GLdouble topC =0.5, bottomC = 0.9;

// ........................................................................


// core function to compute the curvature, later in the file
double krv(vector v00, vector Deriv[3][3], real* crv_result);


// this is the function to initialize the curvature computation
void init_crv()
{
	freshObject = 1;
}

void set_crv_bound_array(double* max_array, double* min_array)
{
	max_crv_value = max_array;
	min_crv_value = min_array;
	init_crv();
}


/*
 * low and hi values of curvature
 */
void set_crv_scale(double lowc, double highc)
{
    low = lowc;
    hi = highc;
    //printf("curvature scale [%3.10f, %3.10f]\n", low, hi);
    printf("Curvature bound: [%.10f .. %.10f] \n",  low, hi);
}


// a special curvature ratio requested by Kestutis karciauskas
void set_special_curvature(double curvature_ratio_a, double curvature_ratio_b)
{
	ratio_a = curvature_ratio_a;
	ratio_b = curvature_ratio_b;
}

// scale by log
double scalebylog(double h)
{
    double newh;
    if (h > 1.0) newh =  1 + log( h);
    else if (h <-1.0) newh = -1 - log(-h);
    else newh = h;
    return newh;
}

/* 
 * convert absolute curvature to relative value [0, 1]
 */
double crv_conv(double in)
{
    double out;

    if(fabs(hi-low) < tol) {
		return 0.5;
	}

    else if (in > hi) {
        out = 1;
    } else {
        if ( in<low){
            out = 0;
        } else {
            out = (in-low)/(hi-low);
        }
    }
    //printf("in %f , out %f , hi, low: (%f, %f)\n", in, out, hi, low);
    return(out);
}


#define USE_REDBLUE_CURVATURE_SCALE false
/*
 * relative curvature -> RGB value
 */
color_t crv2color(double in)
{
    double h = crv_conv(in);

    if(crv_style == 0) // lines
    {
        if (  (0.101>h && h>0.09) || (0.201>h && h>0.19) ||
              (0.301>h && h>0.29) || (0.401>h && h>0.39) ||
              (0.501>h && h>0.49) || (0.601>h && h>0.59) ||
              (0.701>h && h>0.69) || (0.801>h && h>0.79) ||
              (0.901>h && h>0.89) )
            return mkcolor3(0,0,0);
       else
            return mkcolor3(0.9f, 0.9f, 0.9f);
    }
    else if (crv_style == 1) // colors
    {

        // B -> C -> G -> Y -> R
        color_t c[5] = { {0.0f, 0.0f, 0.85f},  // blue
                            {0.0f, 0.9f, 0.9f},   // cyan
                            {0.0f, 0.75f, 0.0f},  // green
                            {0.9f, 0.9f, 0.0f},   // yellow
                            {0.85f, 0.0f, 0.0f} };// red


        if (h>=crv_scale[4])
            return c[4];
        else if (h<=crv_scale[0])
            return c[0]; // blue
        else
        {
            int i;
            for(i=0;i<4;i++)
                if (crv_scale[i+1] >= h) break;
            double u = (h - crv_scale[i]) /(crv_scale[i+1] - crv_scale[i]);

            return interp(u, c[i], c[i+1]);
        }
    }
    else // gray scale
    {
        float u = (float) (0.9f-0.7f*h);
        // gray scale from 0.2 to 0.9
        return mkcolor3(u,u,u);
    }
}

double wedge(vector a, vector b)
{
    double ret;

    ret = 
    a[0]*a[0]*(b[1]*b[1]+b[2]*b[2])+
    a[1]*a[1]*(b[0]*b[0]+b[2]*b[2])+
    a[2]*a[2]*(b[0]*b[0]+b[1]*b[1])-
    2*(
	a[0]*a[1]*b[0]*b[1] +
	a[0]*a[2]*b[0]*b[2] +
	a[1]*a[2]*b[1]*b[2] 
    );
    return(ret);
}


/* curvature routine for three sided patch
 * Compute the curvature from related coefficients

   input: Bezier control points related to the curvature

         v ^ 
           | v02
           | v01 v11
           | v00 v10 v20
            ----------------> u
   output: curvature at v00
*/
double crv3(vector v00,vector v10,vector v20,vector v01,vector v02,vector v11,int deg, 
		real* crv_result)
{
    vector         Deriv[3][3];
    int         m;
    int         d1 = deg-1;


	// first compute the derivatives 
    for (m=0; m< DIM; m++) {
        Deriv[1][0][m] = deg*(v10[m]-v00[m]);
        Deriv[0][1][m] = deg*(v01[m]-v00[m]);
		if(d1==0) {
			Deriv[0][2][m] = Deriv[2][0][m] = Deriv[1][1][m] =0;
		}
		else 
		{
			Deriv[2][0][m] = deg*d1*(v20[m]-2*v10[m]+v00[m]);
			Deriv[0][2][m] = deg*d1*(v02[m]-2*v01[m]+v00[m]);
			Deriv[1][1][m] = deg*d1*(v11[m]-v01[m]-v10[m]+v00[m]);
		}
    }

	// calculate the curvature based on the Deriv
    return krv(v00, Deriv, crv_result);     
}


/* curvature routine for four sided patch 
   input: Bezier control points related to the curvature
         v ^ 
           | v02
           | v01 v11
           | v00 v10 v20
            ----------------> u
   output: curvature at v00
*/
double crv4(vector v00, vector v01,vector v02, vector v10, vector v20, vector v11,
		int degu, int degv, real* crv_result)
{ 
    vector         Deriv[3][3];
    int m;
    int         degu1 = degu-1;
    int         degv1 = degv-1;

	// first compute the derivatives 
    for (m=0; m< DIM; m++) {
        Deriv[0][1][m] = degu*(v01[m]-v00[m]);
		if(degu1==0)
			Deriv[0][2][m] =0;
		else
			Deriv[0][2][m] = degu*degu1*(v02[m]-2*v01[m]+v00[m]);
        Deriv[1][0][m] = degv*(v10[m]-v00[m]);
		if(degv1==0)
			Deriv[2][0][m] = 0;
		else
			Deriv[2][0][m] = degv*degv1*(v20[m]-2*v10[m]+v00[m]);
        Deriv[1][1][m] = degu*degv*(v11[m]-v01[m]-v10[m]+v00[m]);
    }

	// calculate the curvature based on the Deriv
    return krv(v00, Deriv, crv_result);
}


/* 
 * update the min max of the curvature
 */
void minmax(real* curv, int choice, int num)
{
	int i;
	if(max_crv_value == NULL || min_crv_value == NULL) return;

	if(freshObject)
	{
		for(i=choice;i<choice+num;i++)
			max_crv_value[i] = min_crv_value[i] = curv[i-choice];
		freshObject = 0;
	}
	else
	{
		for(i=choice;i<choice+num;i++) {
			if(curv[i-choice] <min_crv_value[i]) min_crv_value[i] = curv[i-choice] ;
			if(curv[i-choice] >max_crv_value[i]) max_crv_value[i] = curv[i-choice] ;
		}
	}
}

/* compute the curvatures using the derivatives 
 * 
 * curvatures are saved into an array: crv_result
 *     in order of: Gauss, Mean, Max, Min
 */
double krv(vector v00, vector Deriv[3][3], real* crv_result)
{
    double x,y,z,d;
    double xu,yu,zu,du, xv,yv,zv,dv;
    double xuu,yuu,zuu,duu, xvv,yvv,zvv,dvv;
    double xuv,yuv,zuv,duv;
    double kes, m1, m2, m3;
    double L, M, N;
    double E, G, F;
    double K, H, Max, Min; // results
    double disc;

	double special_curvature;

    x = v00[0]; y = v00[1];
    z = v00[2]; d = v00[3];

    xu = Deriv[1][0][0]; yu = Deriv[1][0][1];
    zu = Deriv[1][0][2]; du = Deriv[1][0][3];

    xuu = Deriv[2][0][0]; yuu = Deriv[2][0][1];
    zuu = Deriv[2][0][2]; duu = Deriv[2][0][3];

    xv = Deriv[0][1][0]; yv = Deriv[0][1][1];
    zv = Deriv[0][1][2]; dv = Deriv[0][1][3];

    xvv = Deriv[0][2][0]; yvv = Deriv[0][2][1];
    zvv = Deriv[0][2][2]; dvv = Deriv[0][2][3];

    xuv = Deriv[1][1][0]; yuv = Deriv[1][1][1];
    zuv = Deriv[1][1][2]; duv = Deriv[1][1][3];

    L=det4(xuu,yuu,zuu,duu,
             xu,yu,zu,du,
             xv,yv,zv,dv,
             x,y,z,d);
    N=det4(xvv,yvv,zvv,dvv, 
             xu,yu,zu,du,
             xv,yv,zv,dv,
             x,y,z,d);
    M=det4(xuv,yuv,zuv,duv,
             xu,yu,zu,du,
             xv,yv,zv,dv,
             x,y,z,d);

    E = (xu*d-x*du)*(xu*d-x*du) + (yu*d-y*du)*(yu*d-y*du) +
		(zu*d-z*du)*(zu*d-z*du);
    G = (xv*d-x*dv)*(xv*d-x*dv) + (yv*d-y*dv)*(yv*d-y*dv) +
		(zv*d-z*dv)*(zv*d-z*dv);
    F = (xu*d-x*du)*(xv*d-x*dv) + (yu*d-y*du)*(yv*d-y*dv) +
		(zu*d-z*du)*(zv*d-z*dv);

    m1=det3(y,z,d,yu,zu,du,yv,zv,dv);
    m2=det3(x,z,d,xu,zu,du,xv,zv,dv);
    m3=det3(x,y,d,xu,yu,du,xv,yv,dv);
    kes=m1*m1+m2*m2+m3*m3;

	if(0) {//fabs(kes) < tol*tol) {  // temp for Jorg & Kestas & me class A
		K = H = Max = Min = 0;
	}
	else {
		K = d*d*d*d*(L*N-M*M)/(kes*kes);  // Gaussian curvature
		H = d*(L*G-2*M*F+N*E) / sqrt(kes*kes*kes) /2;  // Mean curvature
		disc = H*H - K;
		if (disc < 0) {
			if (disc < -tol)
				printf("[krv] disc %f H %f \n",disc, H);
			Max = Min = H;
		}
		else {
			disc = sqrt(disc);
			Max = H + disc;
			Min = H - disc;
		}
	}

	if(0) {  // scale the result by log?
    	K = scalebylog(K);
    	H = scalebylog(H);
    	Max = scalebylog(Max);
    	Min = scalebylog(Min);
	}

    crv_result[0] = K; // Gaussian curvature
    crv_result[1] = H; // Mean curvature
    crv_result[2] = Max; // max curvature
    crv_result[3] = Min; // min curvature

	// a special 
	special_curvature =  ratio_a*K + ratio_b*H*H;
//	printf("special_curvature : %f\n", special_curvature);

    if( freshObject )
		min_crv_value[4] = max_crv_value[4] = special_curvature;
	else {
		if(special_curvature<min_crv_value[4]) min_crv_value[4] = special_curvature;
		if(special_curvature>max_crv_value[4]) max_crv_value[4] = special_curvature;
	}

	// set the maximum or minimum value of all four curvatures
	minmax(crv_result, GAUSS_CRV, 4);


    return K;
}


/* to record whether one type of curvature is defined ( created
   as display list )
   So we only define the list when it is needed
	 thus we can avoid the slow launching speed
 */ 
int crvListDefined(int crv_choice)
{
    static int defined[4] = {0,0,0,0};
    //char *type[] = {
    //	  "Gaussian", "Mean", "Max", "Min" };

    if(!defined[crv_choice - GAUSS_CRV])
    {
	    //printf("\ngenerating %s curvature plot .... \n", 
        //		type[crv_choice-GAUSS]);
	    defined[crv_choice - GAUSS_CRV] = 1;
	    return 0;
    }
    return 1;
}

/* the curvature bar 
    input: 
	  color: the color of the numbers and the bars
 */
void draw_crv_bar(color_t color)
{

    GLdouble left_crd  = leftC         ; //  coordinates
    GLdouble right_crd = (leftC + widthC) ;
    GLdouble top_crd   = -topC         ;
    GLdouble bottom_crd = -bottomC      ;
    int i;

    int segments = 20;  // number of segments in the color bar

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1); // simple ortho projection

	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT |GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);

    glBegin(GL_QUADS);    // the color bar (vertical)
    for(i=0;i<segments;i++)
    {
        double u = (double)i/segments, 
               u1= (double)(i+1)/segments;

        double segbottom  = (1-u)*bottom_crd + u*top_crd;
        double segtop     = (1-u1)*bottom_crd + u1*top_crd;

        glColorc( crv2color( (1-u)*low + u*hi ) );
        glVertex2d( right_crd, segbottom);
        glVertex2d( left_crd, segbottom); 
        glColorc( crv2color( (1-u1)*low + u1*hi) );
        glVertex2d( left_crd, segtop);
        glVertex2d( right_crd, segtop);
    }
    glEnd();

    glColorc(color);

    glLineWidth(4.0);  // curvature bars (horizontal)
    glBegin(GL_LINES);
    for(i=0;i<5;i++)
    {
        double u = crv_scale[i];
        double y = (1-u)*bottom_crd + u*top_crd; 
        glVertex2d( leftC-widthC/2, y);
        glVertex2d( leftC+widthC*3/2, y);
    }
    glEnd();

    for(i=0;i<5;i++)   // curvature scale numbers
    {
        double u = (double)i/4;
        print_number ( leftC+widthC*2,
             (1-u)*top_crd + u* bottom_crd, (1-u)*hi+u*low , 0.02, color);
    }
	glPopAttrib();

    glMatrixMode(GL_PROJECTION);   // reset the projection and modelview (don't switch orders)
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


/* print a floating number on the screen */
void print_number(double x, double y, real v, real dist, color_t color)
{
    char string[20];
    glColorc(color);
    snprintf(string, 20, "%.4f", v);
    glDrawText(x, y, 1.0, string);
}


/* check if the mouse clicked on the curvature bar 
   input: x, y mouse position
   return value 
      0-4 : picked one of the curvature scale
      -1  : not in the region 
      -2  : in the region but no picking happened
 */
int clickon_crv_bar(int x, int y, int winWidth, int winHeight)
{
    double pick_size = 0.01;
    int centerX, centerY;
    double dx, dy;
    int i;
    
    centerX = winWidth/2;
    centerY = winHeight/2;
    
    dx = (double)(x - centerX)/ centerX ;
    dy = (double)(y - centerY)/ centerY ;

    // printf("checking: %d, %d : %f, %f\n", x, y, dx, dy);

    // check if the (x,y) is in the region of the curvature bar */
    if ( (dx < leftC - widthC/2 - pick_size)  ||
         (dx > leftC + widthC*3/2 + pick_size)  ||
         (dy < (topC-bottomC)*crv_scale[4]+bottomC - pick_size)  ||
         (dy > (topC-bottomC)*crv_scale[0]+bottomC + pick_size) )

         return -1;

    // check y direction
    for(i=0;i<5;i++)
    {
        double scale_y = (1-crv_scale[i]) * bottomC + crv_scale[i]* topC;
        if( (dy<(scale_y+pick_size)) && (dy>(scale_y-pick_size )) )
        {
            // printf("click on scale %d \n", i);
            return i;
        }
    }
    return -2;   // no picking
}

/* compute the normal and the point of
 * a bezier patch corner from the close-by control points 
 */ 
void evalPN(vector v00, vector v01, vector v10, vector P, vector N)
{
	vector hv1, hv2, Normal;
	int i;

   	VVminus(v10 , v00, hv1); 
   	VVminus(v01 , v00, hv2); 
    VVcross(hv1, hv2, Normal);
	Normalize(Normal);
	Vcopy(Normal, N); 

	for(i=0;i<DIM;i++) // nomalize the rational point
		P[i] = v00[i]/v00[3];

//	for(i=0;i<3;i++) // temp
//		N[i] = -N[i];
}

/* modifying the no. i scale according to the mouse position winy */
void adjust_scale(int i, int winy, int winHeight)
{
    int centerY = winHeight/2;
    double dy = (double)(winy - centerY)/ centerY;
    double scale =  (dy-bottomC)/(topC-bottomC);

    // printf("adjusting scale no. %d to %f\n", i, scale);
    if(i>0 && i<4)  // intermediate scales, forbidden to exceed the boundary
    {
        if( scale < (crv_scale[i+1]-0.01) &&
                scale > (crv_scale[i-1]+0.01) ) 
        crv_scale[i] = scale;
    }
    else    // end point scales , keep intermediate scale ratio fixed
    {
        int s;
        double ratio[5];

        // the top scale and bottom scale are not allowed to be too close
        if ( (i==0 && scale > crv_scale[4] - 0.05) || 
             (i==4 && scale < crv_scale[0] + 0.05) )
         return; 

        // compute the intermediate scales
        for(s = 1; s<4; s++)
            ratio[s] = (crv_scale[s] - crv_scale[0]) /
                          (crv_scale[4]- crv_scale[0]);

        crv_scale[i] = scale;
        // re-compute all three intermediate scales
        for(s = 1; s<4; s++)
            crv_scale[s] = ratio[s]*(crv_scale[4]- crv_scale[0]) + 
                              crv_scale[0];
    }
}


//////////////////////////////////////////////////
// get the curvature value inside the array
//  crv_choice: GAUSS, MEAN, MAX, MIN
real get_crv(real *crv_array, int loc, int crv_choice)
{
	if(crv_choice != SPECIAL_CRV)
		return crv_array[loc*4+ crv_choice];

	else  // special curvature 
	{
		real K, H; // Gauss, Mean curvature
		K  = crv_array[loc*4];
		H  = crv_array[loc*4+1];
		return ratio_a*K+ ratio_b*H*H;
	}
}


//////////////////////////////////////////////////
// For normal clipping -- Added May 06 2004
int num_clipping_planes=0;
real* clip_plane;
real* clip_width;
int normal_clipping = 1;
 
/* 
 * -- kick out the comments in a data file
 */
void kick_comments(FILE* fp)
{
    char ch;
	const char END_OF_LINE  = 10;
    ch = fgetc(fp);

    while (1)
    {
        if(ch == '#')
        {
            do{
                 ch = fgetc(fp);
            }while (ch!= END_OF_LINE);
            ch = fgetc(fp);
        }
        else if( ch == ' ' || ch == END_OF_LINE || ch == '\t' || ch == ':')
        {
            do{
                ch = fgetc(fp);
            }while(ch==' ' || ch == END_OF_LINE || ch == '\t' || ch == ':');
        }
        else
		{
			fseek(fp, -1, SEEK_CUR);
            return ;
		}
    }
}

//////////////////////////////////////////////////
// reading clipping planes
void read_clipping(const char* filename) 
{
	FILE* fp;
	int i;
	if ( (fp = fopen (filename, "r")) == NULL) {
		num_clipping_planes = 0;
		return;
	}

	kick_comments(fp); // kick out the comments

	if ( fscanf(fp, "%d", &num_clipping_planes) !=1)
	{
		num_clipping_planes = 0;
		return;
	}

	clip_plane = (real *) malloc( sizeof(real) * 4* num_clipping_planes);
	clip_width = (real *) malloc( sizeof(real) * num_clipping_planes);
	for(i=0;i<num_clipping_planes; i++)
	{
		double norm;
		if( fscanf(fp, "%lf %lf %lf %lf %lf ", &clip_plane[i*4], &clip_plane[i*4+1], 
			&clip_plane[i*4+2], &clip_plane[i*4+3], &clip_width[i])  !=5 ) {
			num_clipping_planes = i;
			fclose(fp);
			return;
		}

		norm = Normalize(&clip_plane[i*4]);
		if(norm!=0)
			clip_plane[i*4+3] /= norm;
	}

	printf("done reading clipping planes. (%d)\n", num_clipping_planes);
	fclose(fp);
}

///////////////////////////////////////////////////
// release the memory
void release_clipping()
{
	if(num_clipping_planes!=0) {
		free(clip_plane);
		free(clip_width);
	}
}

//////////////////////////////////////////////////
// if a point is cliped : 1-- clipped, 0 -- not clipped
int point_clipped(real* point)
{
	int i;

	if(num_clipping_planes==0) {
		return 0;
	}

	for(i=0;i<num_clipping_planes;i++)
	{
		real dist;
		dist = VVmult(point, &clip_plane[i*4]) - clip_plane[i*4+3] ;
		if( fabs(dist) < clip_width[i] ) 
			return 0;
	}
	return 1;
}


//////////////////////////////////////////////////
// draw the clipping plane
void draw_clipping_plane(int index, int mode, real  size)
{
//	int i;

	if(index >= num_clipping_planes) return; // no such plane exists

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
//    glLoadIdentity();
//    glOrtho(-1, 1, -1, 1, -1, 1); // simple ortho projection

	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
//    glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT|GL_LINE_BIT );
    glDisable(GL_LIGHTING);

	glLineWidth(2.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(size, 0, 0);
	glEnd();
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, size, 0);
	glEnd();
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, size);
	glEnd();


	real* pl = &clip_plane[index*4];

	if(pl[2]!=1)
		glRotated(acos(pl[2])*180/3.14159, -pl[1], pl[0], 0 );

	glTranslated(0, 0, pl[3] );

	if(mode == 0) // rotate
	{
		glLineWidth(2.0);
		glColor3f(0.5, 0.5, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex3d(size/2, size/2, 0);
		glVertex3d(-size/2, size/2, 0);
		glVertex3d(-size/2, -size/2, 0);
		glVertex3d(size/2, -size/2, 0);
		glEnd();
		glLineWidth(1.0);
		glColor3f(0.5, 0.5, 1.0);
		glBegin(GL_LINES);
		glVertex3d(size/2, size/2, 0);
		glVertex3d(-size/2, -size/2, 0);
		glVertex3d(-size/2, size/2, 0);
		glVertex3d(size/2, -size/2, 0);
		glEnd();
	}

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
	glPopAttrib();
}
