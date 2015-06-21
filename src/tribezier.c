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
#include "util.h"
#include "curvature.h"
#include "highlight.h"

#include "type.h"
#include "patch.h"
#include "tribezier.h"

// index mapping functions
int b2i_j(int i, int j, int k, int d);
int b2i_i(int i, int j, int k, int d);
int b2i_k(int i, int j, int k, int d);





// barycentral coordinate mapping to 1d array index
// according to j & k
// i.e. when (i,j,k) has the property: (i+j+k<d)
//             (i,j,k)    will go to 
//              /
//          ((d-j-k), j, k)
// 
// p provides a certain way of overwriting points in DeCastejel
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
// p provides another way of overwriting points in DeCasteljau
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
// p provides the third way of overwriting points in DeCasteljau
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


real* TriBezier_get_bb(Patch*p,int i, int j) {
    return ( &p->position[b2i_i(i,j,p->degu-i-j,p->degu)][0]);
}


int TriBezier_loadFile(Patch *p, FILE *fp, bool art_normal) {
    int i, m;

    //printf("start p->evaluated : %d\n", p->evaluated);
    fscanf(fp,"%d", &p->degu);
    p->pointCount = (p->degu+2)*(p->degu+1) /2;

    // if artificial p->normal exists, read in the degree
    if(art_normal) {
        fscanf(fp,"%d", &p->Ndegu);
        p->normalCount = (p->Ndegu+2)*(p->Ndegu+1) /2;
    }

    // read in all control points
    arrcreate(p->position, p->pointCount);
    for (i=0;i<p->pointCount;i++) {
        for (m=0;m<3;m++)
            fscanf(fp,"%lg", &p->position[i][m]);

        if (DIM==4)
            p->position[i][3] = 1.0;

        // adjust view volume to contain the point
        Bezier_enlarge_aabb(p,p->position[i][0]/p->position[i][3],
                p->position[i][1]/p->position[i][3], p->position[i][2]/p->position[i][3]);

    }

    // read in all coefficients of the p->normal function
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
    return 0;
}


void TriBezier_plot_patch(Patch *p, bool smooth)
{
    int i,j;
    int loc;

    // evaluate the patch first if needed
    if(!p->evaluated) {
        return;
        //evaluate_patch();
    }

    glPushAttrib(GL_LIGHTING_BIT);

    if(!smooth) {
        glShadeModel(GL_FLAT);
    }

    // the plot of the patch consists of a set of triangular strips
    //
    // draw a strip for each i in [0,p->pts)
    for(i=0;i<p->pts;i++) {

        glBegin(GL_TRIANGLE_STRIP);
        for(j=0;j<p->pts-i;j++)
        {

            if(!p->normal_flipped) {  // reverse the orientation of the patch
                loc = b2i_i(i+1, j, p->pts-i-j-1, p->pts);
                glNormal3dv(&(p->eval_N[loc][0]));
                glVertex4dv(&(p->eval_P[loc][0]));

                loc = b2i_i(i, j, p->pts-i-j, p->pts);
                glNormal3dv(&(p->eval_N[loc][0]));
                glVertex4dv(&(p->eval_P[loc][0]));
            }
            else
            {
                loc = b2i_i(i, j, p->pts-i-j, p->pts);
                glNormal3dv(&(p->eval_N[loc][0]));
                glVertex4dv(&(p->eval_P[loc][0]));

                loc = b2i_i(i+1, j, p->pts-i-j-1, p->pts);
                glNormal3dv(&(p->eval_N[loc][0]));
                glVertex4dv(&(p->eval_P[loc][0]));
            }
        }

        // finish the strip by adding the last triangle
        loc = b2i_i(i, j, p->pts-i-j, p->pts);
        glNormal3dv(&(p->eval_N[loc][0]));
        glVertex4dv(&(p->eval_P[loc][0]));
        glEnd();
    }

    glPopAttrib();
}


void TriBezier_plot_mesh(Patch *p, color_t bg_color)
{
    int   i,j,k;
    int   d = p->degu-1;

    glBegin(GL_POINTS);
    for(i=0; i<((p->degu+1)*(p->degu+2)/2); i++)
        glVertex4dv(&p->position[i][0]);
    glEnd();

    for(i=0;i<=d;i++)
        for(j=0;j<=d-i;j++)
        {
            k = d-i-j;
            glBegin(GL_LINE_LOOP);
            glVertex4dv(&p->position[b2i_i(i+1,j,k,p->degu)][0]);
            glVertex4dv(&p->position[b2i_i(i,j+1,k,p->degu)][0]);
            glVertex4dv(&p->position[b2i_i(i,j,k+1,p->degu)][0]);
            glEnd();
        }
}


void TriBezier_evaluate_patch(Patch *p, int subDepth)
{
    GLint   i,j,k;
    GLint   d,m;
    GLdouble u, v, w; /* parameter of patch */
    GLint   uu, vv;
    int     loc = 0;   /* increase 1 for each point computed */
    int     size;

    //printf("evaluate starts\n");

    GLdouble DeCastel[(MAXDEG+1)*(MAXDEG+2)/2][DIM];

    if(p->degu>MAXDEG) {
        printf("Maximum degree %d reached, please increase the number.\n", MAXDEG);
        exit(0);
    }

    int (*b2i)(int i, int j, int k, int d);

    p->pts  = 1 << subDepth;

    /* allocate the memory for evaluation */
    size = (p->pts+1)*(p->pts+2)/2;
    arrcreate(p->eval_P, size);
    arrcreate(p->eval_N, size);
    arrcreate(p->crv_array, size*4);

    for (uu=0; uu<=p->pts; uu++)
    {
        for (vv=0;vv<=p->pts-uu;vv++)
        {
            GLdouble Point[DIM];
            double h;
            GLdouble *V00,*V01, *V02,*V10, *V20, *V11;

            int onbdy = (uu==0) ;  // on the boundary
            int atvtx = (uu==0 && vv==0);

            u = (double)uu/p->pts;
            v = (double)vv/p->pts;
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
            for(i=0;i<=p->degu;i++)
                for(j=0;j<=p->degu-i;j++)
                {
                    k = p->degu -i-j;
                    for( m = 0; m <DIM; m++)
                        DeCastel[(*b2i)(i,j,k, p->degu)][m] =
                                p->position[(*b2i)(i,j,k, p->degu)][m];
                }

            /* de Casteljau algorithm */
            for (d = p->degu-1 ; d >=1; d--)
            {
                for(k=0;k<=d;k++)
                    for(j=0;j<=d-k;j++)
                    {
                        i = d-j-k;
                        for (m=0;m<DIM;m++)
                            DeCastel[(*b2i)(i,j,k,p->degu)][m] =
                                    u* DeCastel[(*b2i)(i+1,j,k,p->degu)][m] +
                                    v* DeCastel[(*b2i)(i,j+1,k,p->degu)][m] +
                                    w* DeCastel[(*b2i)(i,j,k+1,p->degu)][m];
                    }
            }

            /* Last step of de Casteljau algorithm */
            for(m=0;m<DIM;m++)
                Point[m] = u* DeCastel[(*b2i)(1,0,0, p->degu)][m] +
                        v* DeCastel[(*b2i)(0,1,0,p->degu)][m] +
                        w* DeCastel[(*b2i)(0,0,1,p->degu)][m];

            //V00   = DeCastel[(*b2i)(0,0,0,p->degu)];
            V00   = Point;
            if (atvtx )   {
                V01   = DeCastel[(*b2i)(0,1,0,p->degu)];
                V02   = DeCastel[(*b2i)(0,2,0,p->degu)];
                V10   = DeCastel[(*b2i)(1,0,0,p->degu)];
                V20   = DeCastel[(*b2i)(2,0,0,p->degu)];
                V11   = DeCastel[(*b2i)(1,1,0,p->degu)];
            }
            else if (onbdy )
            {
                V01   = DeCastel[(*b2i)(1,0,0,p->degu)];
                V02   = DeCastel[(*b2i)(2,0,0,p->degu)];
                V10   = DeCastel[(*b2i)(0,0,1,p->degu)];
                V20   = DeCastel[(*b2i)(0,0,2,p->degu)];
                V11   = DeCastel[(*b2i)(1,0,1,p->degu)];
                //printf("On boundary\n");
            }
            else
            {
                V01   = DeCastel[(*b2i)(0,0,1,p->degu)];
                V02   = DeCastel[(*b2i)(0,0,2,p->degu)];
                V10   = DeCastel[(*b2i)(0,1,0,p->degu)];
                V20   = DeCastel[(*b2i)(0,2,0,p->degu)];
                V11   = DeCastel[(*b2i)(0,1,1,p->degu)];
            }

            // compute the point and the p->normal at the (u,v) parameter
            evalPN(V00, V01, V10, &p->eval_P[loc][0], &p->eval_N[loc][0]);

            // compute the curvatures (Gaussian, mean, min and max)
            // at the (u,v) parameter
            h = crv3 (V00, V01, V02, V10, V20, V11, p->degu, &p->crv_array[loc*4]);

            //printf("value %f at %d \n", h, loc);
            loc ++;
        }
    }

    // evaluate the artificial normals if necessary
    if(p->art_normal ) {
        loc = 0;
        for (uu=0; uu<=p->pts; uu++) {
            for (vv=0;vv<=p->pts-uu;vv++) {
                u = (double)uu/p->pts;
                v = (double)vv/p->pts;
                w = 1-u-v;
                //printf("loc = %d, u = %f, v= %f, w= %f\n", loc, u, v, w);

                /* initialize the DeCastel Array */
                for(i=0;i<=p->Ndegu;i++)
                    for(j=0;j<=p->Ndegu-i;j++)
                    {
                        k = p->Ndegu -i-j;
                        for( m = 0; m <DIM; m++)
                            DeCastel[(*b2i)(i,j,k, p->Ndegu)][m] =
                                    p->normal[(*b2i)(i,j,k, p->Ndegu)][m];
                    }

                /* de Casteljau algorithm */
                for (d = p->Ndegu-1 ; d >=0; d--)
                {
                    for(k=0;k<=d;k++)
                        for(j=0;j<=d-k;j++)
                        {
                            i = d-j-k;
                            for (m=0;m<DIM;m++)
                                DeCastel[(*b2i)(i,j,k,p->Ndegu)][m] =
                                        u* DeCastel[(*b2i)(i+1,j,k,p->Ndegu)][m] +
                                        v* DeCastel[(*b2i)(i,j+1,k,p->Ndegu)][m] +
                                        w* DeCastel[(*b2i)(i,j,k+1,p->Ndegu)][m];
                        }
                }
                Vcopy(DeCastel[(*b2i)(0,0,0,p->Ndegu)], &p->eval_N[loc][0]);
                Normalize(&p->eval_N[loc][0]);
                loc++;
            }
        }
    }
    p->evaluated = true;
    p->normal_flipped = false;
    //flip_normal(); // why?
}



void TriBezier_flip_normal(Patch *p)
{
    int     i, m, size;

    if(!p->evaluated) {
        return;
    }

    size = (p->pts+1)*(p->pts+2)/2;

    for(i=0;i<size;i++)
        for(m=0;m<3;m++)
            p->eval_N[i][m] = -p->eval_N[i][m];

    p->normal_flipped = !p->normal_flipped;
}


void TriBezier_plot_crv(Patch *p, int crv_choice)
{
    int   i,j;
    int     loc;
    real h;

    glPushAttrib(GL_ENABLE_BIT);

    // evaluate the patch first if needed
    if(!p->evaluated) {
        return;
    }
    //		evaluate_patch();

    glDisable(GL_LIGHTING);  // curvature is shown with lighting off

    for(i=0;i<p->pts;i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for(j=0;j<p->pts-i;j++)
        {
            loc = b2i_i(i, j, p->pts-i-j, p->pts);
            // pick out the curvature from the curvature array
            h = get_crv(p->crv_array, loc, crv_choice);

            // switch the sign if p->normal flipped except for Gaussian curvature
            if(p->normal_flipped && crv_choice != GAUSS_CRV && crv_choice != SPECIAL_CRV)
                h = -h;

            //			printf("h=%f\n", h);
            glColorc( crv2color(h));
            glVertex4dv(&(p->eval_P[loc][0]));

            loc = b2i_i(i+1, j, p->pts-i-j-1, p->pts);
            h = get_crv(p->crv_array, loc, crv_choice);

            // switch the sign if p->normal flipped except for Gaussian curvature
            if(p->normal_flipped && crv_choice != GAUSS_CRV && crv_choice != SPECIAL_CRV)
                h = -h;

            //			printf("h=%f\n", h);
            glColorc( crv2color(h));
            glVertex4dv(&(p->eval_P[loc][0]));
        }

        // finish the strip by adding the last triangle
        loc = b2i_i(i, j, p->pts-i-j, p->pts);
        h = get_crv(p->crv_array, loc, crv_choice);

        // switch the sign if p->normal flipped except for Gaussian curvature
        if(p->normal_flipped && crv_choice != GAUSS_CRV  && crv_choice != SPECIAL_CRV)
            h = -h;

        glColorc( crv2color(h));
        glVertex4dv(&(p->eval_P[loc][0]));
        glEnd();
    }

    glPopAttrib();
}


void TriBezier_plot_crv_needles(Patch *p, int crv_choice, real length)
{
    int i,j, loc;
    real h;

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);  // curvature is shown with lighting off
    for(i=0;i<=p->pts;i++) {
        for(j=0;j<=p->pts-i;j++)
        {
            vector sum;
            loc = b2i_i(i, j, p->pts-i-j, p->pts);

            if(normal_clipping && !point_clipped(&p->eval_P[loc][0])) {

                h = get_crv(p->crv_array, loc, crv_choice);
                VVadd(1.0, &(p->eval_P[loc][0]), h*length, &(p->eval_N[loc][0]),
                        sum);
                glColorc( crv2color(h));

                glBegin(GL_LINES);
                glVertex3dv(&(p->eval_P[loc][0]));
                glVertex3dv(sum);
                glEnd();
            }
        }
    }
    glPopAttrib();
}


void TriBezier_plot_highlights(Patch *p, vector A, vector H, real hl_step, int highlight_type)
{
    real P[3*DIM];
    real N[3*DIM];
    int i, j, k;
    int loc[3];

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);
    for(i=0;i<p->pts;i++)
        for(j=0;j<p->pts-i;j++)
        {
            if(p->normal_flipped) {
                loc[0] = b2i_i(i, j, p->pts-i-j, p->pts);
                loc[1] = b2i_i(i+1, j, p->pts-i-j-1, p->pts);
                loc[2] = b2i_i(i,j+1, p->pts-i-j-1, p->pts);
            }
            else {
                loc[2] = b2i_i(i, j, p->pts-i-j, p->pts);
                loc[1] = b2i_i(i+1, j, p->pts-i-j-1, p->pts);
                loc[0] = b2i_i(i,j+1, p->pts-i-j-1, p->pts);
            }

            for (k=0; k<3; k++) {
                Vcopy( &p->eval_P[loc[k]][0], &P[k*DIM]);
                Vcopy( &p->eval_N[loc[k]][0], &N[k*DIM]);
            }
            Highlight(3, P, N, A, H, hl_step, highlight_type);
        }

    for(i=0;i<p->pts-1;i++)
        for(j=0;j<p->pts-1-i;j++)
        {
            if(p->normal_flipped) {
                loc[0] = b2i_i(i, j+1, p->pts-i-j-1, p->pts);
                loc[1] = b2i_i(i+1,j, p->pts-i-j-1, p->pts);
                loc[2] = b2i_i(i+1, j+1, p->pts-i-j-2, p->pts);
            }
            else {
                loc[2] = b2i_i(i, j+1, p->pts-i-j-1, p->pts);
                loc[1] = b2i_i(i+1,j, p->pts-i-j-1, p->pts);
                loc[0] = b2i_i(i+1, j+1, p->pts-i-j-2, p->pts);
            }

            for (k=0; k<3; k++) {
                Vcopy( &p->eval_P[loc[k]][0], &P[k*DIM]);
                Vcopy( &p->eval_N[loc[k]][0], &N[k*DIM]);
            }
            Highlight(3, P, N, A, H, hl_step, highlight_type);
        }

    glPopAttrib();
}
