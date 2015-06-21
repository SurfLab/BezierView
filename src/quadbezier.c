/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    QuadBezier.cpp
 *  Purpose: render Tensor-product Bezier patches
 *  Author   : Xiaobin Wu
 * ------------------------------------------------------------
 */
#include "stdheaders.h"
#include "type.h"
#include "util.h"
#include "curvature.h"
#include "highlight.h"
#include "type.h"
#include "patch.h"
#include "quadbezier.h"

// functions prototypes -------------------------

// quad Bezier patch subdivision 
void     RSubDiv(vector bb[], int step, int degu, int degv, int sizeu, int sizev);
void     BBcopy4(vector *buf, int degu, int degv, int st, vector * bb);

// Decastel algorithms for 1-D and 2-D Bezier
void     DeCastel2(vector *position, int degu, int degv,
                double u, double v, vector pts);
void     DeCastel1(vector *position, int deg, double u, vector pts);

// global variables -------------------------


int use_art_normal = 1;    // allow PN quads and PN triangles

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
void RSubDiv(vector bb[], int step, int degu, int degv, int sizeu, int sizev)
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
void DeCastel2(vector *position, int degu, int degv,
                double u, double v, vector pts)
{
	int i,j;
    vector *Ubuffer;
    arrcreate(Ubuffer, degu+1);
    vector *Vbuffer;
    arrcreate(Vbuffer, degv+1);

	// calculate Du
    for (i= 0;  i<=degu; i++)
    {
        for (j= 0; j<=degv; j++)
            Vcopy(&position[(i*(degv+1)+j)][0], &Vbuffer[j][0]);

        DeCastel1(Vbuffer, degv, v, Ubuffer[i]);
    }
    DeCastel1(Ubuffer, degu, u, pts);
    arrdelete(Ubuffer);
    arrdelete(Vbuffer);
}

/* De Casteljau algorithm for 1 variable function */
void DeCastel1(vector *position, int deg, double u, vector pts)
{
    double u1 = 1-u;
    int d, i, m;

    for(d=deg;d>1;d--)
        for(i=0;i<d;i++)
            for(m=0;m<DIM;m++)
                position[i][m] = u*position[i][m] + u1*position[(i+1)][m];

    for(m=0;m<DIM;m++)
        pts[m] = u*position[0][m] + u1*position[1][m];
}



/* get a corner control point */
real* QuadBezier_get_v(Patch*p,int s)
{
    switch(s) {
	case 0:
        return QuadBezier_get_bb(p,0, 0);
	case 1:
        return QuadBezier_get_bb(p,p->degu, 0);
	case 2:
        return QuadBezier_get_bb(p,p->degu, p->degv);
	case 3:
        return QuadBezier_get_bb(p,0, p->degv);
	default:
		return NULL;
	}
}

//
// copy bb coefficients from tight array buf into sparse array bb
//  st: step
//  degu, degv: degrees for bb
//
void BBcopy4( vector * buf, int degu, int degv, int st, vector * bb)
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
            vector * v = buf + (i*(degv+1) + j);
            Vcopy(v[0], bb[(j* st)* C + i*st]);
        }
    }
}


void QuadBezier_plot_highlights(Patch *p, vector A, vector H, real hl_step, int highlight_type)
{
    int  i,j,k;
    int  loc[4];
    vector P[4], N[4];

    // evaluate the patch first if needed
    if(!p->evaluated) {
        return;
        //		evaluate_patch();
    }

    //glDisable(GL_LIGHTING);  // highlight is shown with light off
    for(i=0;i<p->pts;i++)
        for(j=0;j<p->pts;j++)       // p loop will draw the quad
        {
            if(p->normal_flipped) {
                loc[0] = i*(p->pts+1)+j;          // (i,j)----(i, j+1)
                loc[1] = i*(p->pts+1)+(j+1);      //   |         |
                loc[2] = (i+1)*(p->pts+1)+(j+1);  //   |         |
                loc[3] = (i+1)*(p->pts+1)+j;      //   |         |
                // (i+1,j)--(i+1, j+1)
            }
            else
            {
                loc[3] = i*(p->pts+1)+j;          // (i,j)----(i, j+1)
                loc[2] = i*(p->pts+1)+(j+1);      //   |         |
                loc[1] = (i+1)*(p->pts+1)+(j+1);  //   |         |
                loc[0] = (i+1)*(p->pts+1)+j;      //   |         |
                // (i+1,j)--(i+1, j+1)
            }

            for (k=0; k<4; k++) {
                Vcopy( p->eval_P[loc[k]], P[k]);
                Vcopy( p->eval_N[loc[k]], N[k]);
            }

            Highlight(4, P, N, A, H, hl_step, highlight_type);
        }

    //glEnable(GL_LIGHTING);  // turn the light back on
}


void QuadBezier_plot_crv_needles(Patch *p, int crv_choice, real length)
{
    int i,j, loc;
    real h;
    int st = 1; //pts;

    glDisable(GL_LIGHTING);  // curvature is shown with light off
    for(i=0;i<=p->pts;i+=st) {
        for(j=0;j<=p->pts;j+=st)
        {
            vector sum;
            loc = i*(p->pts+1)+j;
            if(normal_clipping && !point_clipped(&p->eval_P[loc][0])) {

                h = get_crv(p->crv_array, loc, crv_choice);

                glColorc( crv2color(h));   // use the color of the curvature
                glBegin(GL_LINES);
                //glVertex3dv(&(p->eval_P[loc][0]));

                VVadd(1.0, &(p->eval_P[loc][0]), 0.00, &(p->eval_N[loc][0]),sum);
                glVertex3dv(sum);
                VVadd(1.0, &(p->eval_P[loc][0]), h*length, &(p->eval_N[loc][0]),
                        sum);
                glVertex3dv(sum);
                glEnd();
            }
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);  // curvature is shown with light off
}


void QuadBezier_plot_crv(Patch *p, int crv_choice)
{
    int   i,j;
    int     loc;
    real h;

    // evaluate the patch first if needed
    if(!p->evaluated) {
        return;
        //		evaluate_patch();
    }

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);  // curvature is shown with light off

    // almost same as plot_patch expect for use curvature as color
    for(i=0;i<p->pts;i++) {

        glBegin(GL_QUAD_STRIP);
        for(j=0;j<=p->pts;j++)       // p loop will draw the quad strip:
        {
            loc = i*(p->pts+1)+j;                // (i,0)----(i, 1) -- ...
            h = get_crv(p->crv_array, loc, crv_choice); //      |

            // switch the sign if p->normal flipped   |         |
            // except for Gaussian curvature
            if(p->normal_flipped && crv_choice != GAUSS_CRV  && crv_choice != SPECIAL_CRV)
                h = -h;

            glColorc( crv2color(h));        //   |         |
            glVertex4dv(&(p->eval_P[loc][0]));  //   |         |
            //   |         |
            loc = (i+1)*(p->pts+1)+j;            // (i+1,0)--(i+1,1) -- ...
            h = get_crv(p->crv_array, loc, crv_choice);

            // switch the sign if p->normal flipped
            // except for Gaussian curvature
            if(p->normal_flipped && ( crv_choice != GAUSS_CRV && crv_choice != SPECIAL_CRV ))
                h = -h;

            glColorc( crv2color(h));
            glVertex4dv(&(p->eval_P[loc][0]));
        }
        glEnd();
    }

    glPopAttrib();
}


void QuadBezier_plot_mesh(Patch *p, color_t bg_color)
{
    int   i,j;
    int loc;

    glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT);
    if(bg_color.alpha != 0.0f) {   // if hidden line removal
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 1.0);
        glColorc(bg_color);

        for(i=0;i<p->degu;i++) {
            glBegin(GL_QUADS);
            for(j=0;j<p->degv;j++)       // p loop will draw the quad strip:
            {
                loc = (i)*(p->degv+1)+j;            // (i+1,j)--(i+1, j+1) -- ...
                glVertex4dv(&(p->position[loc][0]));
                loc = (i+1)*(p->degv+1)+j;                // (i,j)----(i, j+1) -- ...
                glVertex4dv(&(p->position[loc][0]));  //   |         |
                loc = (i+1)*(p->degv+1)+j+1;            // (i+1,j)--(i+1, j+1) -- ...
                glVertex4dv(&(p->position[loc][0]));
                loc = i*(p->degv+1)+j+1;                // (i,j)----(i, j+1) -- ...
                glVertex4dv(&(p->position[loc][0]));  //   |         |
            }
            glEnd();
        }
        glDisable(GL_POLYGON_OFFSET_FILL);
        glPopAttrib();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for(i=0;i<p->degu;i++) {
        glBegin(GL_QUADS);
        for(j=0;j<p->degv;j++)       // p loop will draw the quad strip:
        {
            loc = (i)*(p->degv+1)+j;            // (i+1,j)--(i+1, j+1) -- ...
            glVertex4dv(&(p->position[loc][0]));
            loc = (i+1)*(p->degv+1)+j;                // (i,j)----(i, j+1) -- ...
            glVertex4dv(&(p->position[loc][0]));  //   |         |
            loc = (i+1)*(p->degv+1)+j+1;            // (i+1,j)--(i+1, j+1) -- ...
            glVertex4dv(&(p->position[loc][0]));
            loc = i*(p->degv+1)+j+1;                // (i,j)----(i, j+1) -- ...
            glVertex4dv(&(p->position[loc][0]));  //   |         |
        }
        glEnd();
    }
    glPopAttrib();
}


void QuadBezier_flip_normal(Patch *p)
{
    int     i, m, C, size;
    if(!p->evaluated) {
        return;
    }

    // size of the p->normal array
    C    = p->pts+1;
    size = C*C;

    // reverse the normals
    for(i=0;i<size;i++)
        for(m=0;m<3;m++)
            p->eval_N[i][m] = -p->eval_N[i][m];

    p->normal_flipped = !p->normal_flipped;
}


void QuadBezier_evaluate_patch(Patch *p, int subDepth)
{
    int     size;
    int     Cu, Cv, st, C;
    int     sizeu, sizev, bigstepu, bigstepv;
    int     i, r, rs, r1, r2, c, loc;
    double  h;
    vector*    bb;

    p->pts  = 1 << subDepth;

    // allocate the memory for the result of evaluation
    C    = p->pts+1;
    size = C*C;            // how big should the array be
    arrcreate(p->eval_P, size);
    arrcreate(p->eval_N, size);
    arrcreate(p->crv_array, size*4);

    // allocate a temporary memory to perform subdivision
    //    subdivision VS de Casteljau ??
    //
    //    now subdivision because the code already exists,
    //
    //    De Casteljau clearly do not need p temporary memory,
    //    but maybe slower compare to p code.
    //
    st = p->pts;         // original space between two coefficients

    // it is set to 'p->pts' so that after subdivision
    // the memory becomes tight

    sizeu = st*p->degu;  // size for both directions
    sizev = st*p->degv;
    Cu = sizeu+1;       // 0,0     ..  0,sizeu
    Cv = sizev+1;       // sizev,0 .. sizev, sizev

    arrcreate(bb, Cu * Cv);

    // BBcopy4(PAcopy4) -- copy the original data into the sparse array
    BBcopy4( p->position, p->degu, p->degv, p->pts, bb);

    // subdivision
    for (i=0; i <subDepth; i++)
    {
        RSubDiv(bb, st, p->degu, p->degv, sizeu,sizev);
        st = st/2;  // distance halves after each subdivision
    }

    //    bigstepu = st*p->degu;	/* distance between patches -> column direction */
    //    bigstepv = st*p->degv;	/* distance between patches -> row direction */

    bigstepu = p->degu;	/* distance between patches -> column direction */
    bigstepv = p->degv;	/* distance between patches -> row direction */

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
                    bb[r1+c],bb[r2+c],bb[r1+c+st],p->degu, p->degv, &p->crv_array[loc*4]);

            evalPN(bb[rs+c], bb[r1+c], bb[rs+c+st], &p->eval_P[loc][0],
                    &p->eval_N[loc][0]);
            //printf (" %d %d %d %d %d %d \n", rs+c, rs+c+st, rs+c+2*st,
            //      r1+c, r2+c, r1+c+st);
        }

        // last col _| note: stencil is rotated by 90 degrees c = sizeu;
        loc = (c/bigstepu*C + r/bigstepv) ;
        h =crv4(bb[rs+c],bb[r1+c],bb[r2+c], bb[rs+c-st],
                bb[rs+c-2*st],bb[r1+c-st],p->degv, p->degu, &p->crv_array[loc*4]);

        evalPN(bb[rs+c], bb[rs+c-st], bb[r1+c], &p->eval_P[loc][0],
                &p->eval_N[loc][0]);

    }
    // top row |-
    r = sizev;
    rs = r*Cu;
    r1 = (r-st)*Cu;
    r2 = (r-2*st)*Cu;
    for (c = 0; c<sizeu; c += bigstepu) {
        loc = (c/bigstepu*C + r/bigstepv) ;
        h =crv4(bb[rs+c],bb[r1+c],bb[r2+c], bb[rs+c+st],  	// curvature
                bb[rs+c+2*st],bb[r1+c+st],p->degv, p->degu, &p->crv_array[loc*4]);

        evalPN(bb[rs+c], bb[rs+c+st], bb[r1+c], &p->eval_P[loc][0],
                &p->eval_N[loc][0]);

    }

    // top right -|
    c = sizeu;
    loc = (c/bigstepu*C + r/bigstepv) ;
    h = crv4(bb[rs+c],bb[rs+c-st],bb[rs+c-2*st], bb[r1+c],  // curvature
            bb[r2+c], bb[r1+c-st],p->degu, p->degv, &p->crv_array[loc*4]);

    evalPN(bb[rs+c], bb[r1+c], bb[rs+c-st],  &p->eval_P[loc][0],
            &p->eval_N[loc][0]);

    arrdelete(bb);  // free the space used for subdivision

    // evaluate the artificial normals if necessary
    if(p->art_normal && use_art_normal) {
        printf("using artificial normals\n");
        for(r=0;r<=p->pts;r++) {
            double u = 1-(double)r/p->pts;
            for(c=0;c<=p->pts;c++) {
                double v = 1-(double)c/p->pts;
                loc = r*C+c;
                //printf("loc = %d, u=%f, v=%f\n", loc, u, v);
                DeCastel2(p->normal, p->Ndegu, p->Ndegv, u, v, p->eval_N[loc]);
                Normalize(&p->eval_N[loc][0]);
            }
        }
    }

    // set the p->evaluated flag to be true
    p->evaluated = true;
    p->normal_flipped = false;
}


void QuadBezier_plot_patch(Patch *p, bool smooth)
{
    int i,j;
    int loc;

    if(!p->evaluated)
        return;

    glPushAttrib(GL_LIGHTING_BIT);

    if(!smooth) {
        glShadeModel(GL_FLAT);
    }

    // the filling patch
    // the plot of the patch consists of a set of quad strips
    //
    // draw a strip for each i in [0,pts)
    for(i=0;i<p->pts;i++) {

        glBegin(GL_QUAD_STRIP);
        for(j=0;j<=p->pts;j++)       // p loop will draw the quad strip:
        {
            if(!p->normal_flipped) { // reverse the orientation of the patch
                loc = i*(p->pts+1)+j;                // (i,j)----(i, j+1) -- ...
                glNormal3dv(&(p->eval_N[loc][0]));  //   |         |
                glVertex4dv(&(p->eval_P[loc][0]));  //   |         |
                loc = (i+1)*(p->pts+1)+j;            // (i+1,j)--(i+1, j+1) -- ...
                glNormal3dv(&(p->eval_N[loc][0]));
                glVertex4dv(&(p->eval_P[loc][0]));
            }
            else {
                loc = (i+1)*(p->pts+1)+j;            // (i+1,j)--(i+1, j+1) -- ...
                glNormal3dv(&(p->eval_N[loc][0]));
                glVertex4dv(&(p->eval_P[loc][0]));
                loc = i*(p->pts+1)+j;                // (i,j)----(i, j+1) -- ...
                glNormal3dv(&(p->eval_N[loc][0]));  //   |         |
                glVertex4dv(&(p->eval_P[loc][0]));  //   |         |
            }
        }
        glEnd();
    }

    glPopAttrib();
}


int QuadBezier_create(Patch *p, int degu, int degv) {
    p->degu = degu;
    p->degv = degv;
    p->pointCount = (degu+1)*(degv+1);
    arrcreate(p->position, p->pointCount);
    return 0;
}


void QuadBezier_loadFile(Patch *p, FILE *fp, bool equal_deg, bool rational, bool art_normal) {
    int i, m;
    int degu, degv;

    fscanf(fp,"%d", &degu);
    if(equal_deg)
        degv = degu;
    else
        fscanf(fp,"%d", &degv);

    // create the patch ( allocate memory, assign degrees)
    QuadBezier_create(p, degu, degv);

    if(art_normal)   // if p is a PN quad Bezier patch
    {
        fscanf(fp,"%d", &p->Ndegu);
        fscanf(fp,"%d", &p->Ndegv);
        p->normalCount = (p->Ndegu+1)*(p->Ndegv+1);
    }

    // read in all control points

    for (i=0;i<p->pointCount;i++) {
        for (m=0;m<3;m++)
            fscanf(fp,"%lg", &p->position[i][m]);

        if(rational)
            fscanf(fp,"%lg", &p->position[i][3]);
        else
            p->position[i][3] = 1.0;

        // adjust view volume to contain the point
        Bezier_enlarge_aabb(p, p->position[i][0]/p->position[i][3],
                p->position[i][1]/p->position[i][3], p->position[i][2]/p->position[i][3]);
    }

    // read in all coefficients of the p->normal function, if PN quad
    if(art_normal) {
        arrcreate(p->normal, p->normalCount);
        for (i=0;i<p->normalCount;i++) {
            for (m=0;m<3;m++)
                fscanf(fp,"%lg", &p->normal[i][m]);

            if (DIM==4)
                p->normal[i][3] = 1.0;
        }
    }

    p->art_normal = art_normal;

}


real *QuadBezier_get_bb(Patch *p, int i, int j) {
    return &p->position[(i*(p->degv+1)+j)][0];
}
