#include <stdio.h>
#include <GL/glut.h>
#include "EncQuadBezier.h"
#include "util.h"
#include "SubLiME.h"

//#define FIX_BY_SUBDIVIDE  // fix the problematic case by subdividing

// ................................................................
//
// Debug flags: display intermediate result using openGL
//
#define BOX    1     // boxes show the conponent bounds
#define EXTTRI 2     // extreme triangle
#define SUPNOR 4     // support normal at break points

// Use | to combine the above 3 item
int debugchoice = 0; //SUPNOR | BOX ; 
// ................................................................


// ................................................................
//
// Some functions used from other files
void set_color(int c);  // change the color of output (in light.c)
void DrawTri(REAL v1[], REAL v2[], REAL v3[]);
//void RSubDiv(VEC bb[], int step, int degu, int degv, int sizeu, int sizev);
//int new_patch(int FNum, p_patch Face, int clone_fc);
// ................................................................


// -------------------------------------------------------------------
// intersect a plane with a line
// derive: apply Q = (v0-v1)x(v0-v2) to (N,v0-v1,v0-v2)[l,u,v] = v0-P
double plane_intersect(VEC pt, VEC nor, VEC v0, VEC v1, VEC v2)
{
    double lam,h1,h2; // VVmult();
    VEC hv1, hv2, X;

    VVminus(v0,v1,hv1);
    VVminus(v0,v2,hv2);
    VVcross (hv1,hv2, X);
    VVminus(v0,pt,hv1);
    h1 = VVmult(hv1,X);
    h2 = VVmult(nor,X);
    //lam = VVmult(hv1,X)/ VVmult(nor,X);
    lam = h1/h2;
    return(lam);
}

// -------------------------------------------------------------------
// compute the enclosure for each component of this patch
void EncQuadBezier::make_env() 
{
    int i,j,m;

	int ov  = degv+1;
	int ov2 = segv+1;

	// compute the bounds for each component
	for(m=0;m<DIM;m++)
	    tpSlefe(&coeff[m], ov*DIM, DIM, degu, degv, segu, segv, 
					&o_enc[m], &i_enc[m], ov2*DIM, DIM);

	// draw the little cubes (for illustration)
	if(debugchoice & BOX)
	{
        for (j=0; j<d1; j++) 
	    for (i=0; i<d1; i++) {
		    VEC center;
		    VEC size;
		    for(m=0;m<DIM;m++) {
			    center[m] = (get_enc(1,i,j)[m] + get_enc(0,i,j)[m])/2;
			    size[m] = (get_enc(1,i,j)[m] - get_enc(0,i,j)[m]);
		    }
			glEnable(GL_LIGHTING);
		    set_color(6);
			glMatrixMode(GL_MODELVIEW);
		    glPushMatrix();
		    glTranslated(center[0],center[1],center[2] );
		    glScaled(size[0], size[1], size[2]);
		    glutSolidCube(1.0);
		    glPopMatrix();
	    }
	}
}

// -------------------------------------------------------------------
// compute the support point and the support normal
void EncQuadBezier::make_sup() 
{
    int i,j;

    for (i=0; i<=segu; i++) {  // for each point
        for (j=0; j<=segv; j++) {
            // calculate the correct normal and point
            // no approximation 
            evaluate_point(sup_pt[i][j], sup_nor[i][j], 
                           (double)(segu-i)/segu, (double)(segv-j)/segv);

            //for (m=0; m< DIM; m++) {
                //sup_pt[i][j][m] = (bd[0][i][j][m]+bd[1][i][j][m])/2;
            //}
		}
	}
}

// -------------------------------------------------------------------
// for each bilinear facet compute center normal
// to select the Lbd, Ubd combination;
// determine the curvature (crease direction) and 
// find intersection length lambda[0] lambda[1]

// return 1 if this patch needs to be subdivided 
// ( triangles intersect the normal to the opposite direction)
// return 0 if ok
int EncQuadBezier::make_lam()
{
    VEC  nor, bend, hv0, hv1; // bend = bdir
    double  lam, h;
    int  i,j, m, sm,sp,s2, idx[4][2], sd, ii,jj,
         nsgn, nnsgn, cr,ncr; // booleans
    VEC  env[2][4]; // env[1]=(upper)=outer

	int  need_subdiv = 0; // return value

    for (i=0; i<d1; i++) 
        for (j=0; j<d1; j++) 
            lambda[i][j][0]= lambda[i][j][1]= 0;
    for (i=0; i<segu; i++) {
        for (j=0; j<segv; j++) {
            // normal at center determines which bilinear
            // to choose, eg +++ in all components means
            //  ubd ubd ubd = outer
            //  lbd lbd lbd = inner
            // determine diagonal:
            // curvature  - +
            //            + -
            VEC mid[2][2];

            for(m=0;m<DIM;m++) {
                mid[0][0][m] =(get_enc(1,i,j)[m]   + get_enc(0,i,j)[m])/2;
                mid[0][1][m] =(get_enc(1,i,j+1)[m] + get_enc(0,i,j+1)[m])/2;
                mid[1][0][m] =(get_enc(1,i+1,j)[m] + get_enc(0,i+1,j)[m])/2;
                mid[1][1][m] =(get_enc(1,i+1,j+1)[m] + get_enc(0,i+1,j+1)[m])/2;
            }

            for (m=0; m<DIM; m++) 
                bend[m] = mid[0][0][m]+mid[1][1][m]-mid[0][1][m]-mid[1][0][m];

            VVminus(mid[1][1],mid[0][0],hv0);
            VVminus(mid[0][1],mid[1][0],hv1);
            VVcross(hv0,hv1, nor);

            // if (crease >= 0)  // triangles |/| or flat
            h = VVmult(sup_nor[i][j],nor);
            if (h < 0) printf("h %lf\n",h);
            h = VVmult(bend,nor);
            cr = (h >=  0); // convex up in normal direction == cr=1

            cralong[i*(segv)+j] = cr; // record crease of bilinear

            // translation table 
            // 3 2
            // 0 1
            idx[0][0] = i; idx[0][1] = j;
            idx[1][0] = i+1; idx[1][1] = j;
            idx[2][0] = i+1; idx[2][1] = j+1;
            idx[3][0] = i; idx[3][1] = j+1;


            // quadrant of the normal decides on choice of bilinear
            // depends on whether the normal is "outward" pointing
            // bd[1]...[x]: x component larger
            for (m=0; m<DIM; m++) {
                nsgn = (nor[m] >= 0); // if 1 then want bd[1] -- except when
                // normal is inward pointing (cube)
                nnsgn = (nsgn+1)%2;
                for (sd=0; sd<4; sd++) {
                    // (nsgn=1) want upper bd in normal dir (env[1]..)
                    ii = idx[sd][0]; jj = idx[sd][1];
                    env[1][sd][m] = get_enc(nsgn,ii,jj)[m]; 
                    env[0][sd][m] = get_enc(nnsgn,ii,jj)[m];
                }
            }

            if(0) // draw the nor (put it in the center of the quad)
            {
                VEC center, sum;
                for(m=0;m<DIM;m++)
                {
                    // center of all four up-enclosure corners
                    center[m] = (env[1][0][m] + env[1][1][m] +
                                 env[1][2][m] + env[1][3][m])/4;
                }
                Normalize(nor);
                VVadd(1.0, center, 20, nor, sum);
                glDisable(GL_LIGHTING);
                glColor3f(0.3,0.0, 1.0);
                glBegin(GL_LINES);
                glVertex4dv(center);
                glVertex4dv(sum);
                glEnd();
                glEnable(GL_LIGHTING);
            }

            // compute lambdas based on crease orientation 
            // cr==1 for ridge 02 in normal direction 
            //       isect with  2 env[1] and 1 env[0]
            for (sd=0; sd<4; sd++) { // intersect with 3 total!

                ii = idx[sd][0]; jj = idx[sd][1];
                sp = (sd+1)%4; s2 = (sd+2)%4; sm = (sd+3)%4;
                ncr = (cr+1)%2;
                //  sm---s2
                //   |    |
                //  sd---sp

                lam = plane_intersect(sup_pt[ii][jj], sup_nor[ii][jj],
                    env[cr][sd], env[cr][sp], env[cr][s2]);

                // Why (cr) selection? if cr then q=nor same dir as bend, as
                // supnor  -- so compare with the two planes of the
                // triangles (cr) and then with the other (ncr)
                //
                if (cr) { if (lam > lambda[ii][jj][1])  lambda[ii][jj][1] = lam;
                } else  { 
                    if (lam < lambda[ii][jj][0])  { lambda[ii][jj][0] = lam; }
                }

                if ((cr && (lam < -tol) ) || (ncr && (lam > tol)))
                {
                    printf("not ok 1 cr %d lam %lf iijj %d %d \n",cr,lam,ii,jj);
					need_subdiv = 1; // need to be subdivied
                }
                    if(debugchoice & EXTTRI) {
                        set_color(ncr);
                        if(!ncr)  // only outter enclosure
                        DrawTri(env[cr][sd], env[cr][sp], env[cr][s2]);
                    }


                //else printf(" ok 1 \n");
                lam = plane_intersect(sup_pt[ii][jj], sup_nor[ii][jj], 
                    env[cr][sd], env[cr][s2], env[cr][sm]);
                if (cr) { if (lam > lambda[ii][jj][1])  lambda[ii][jj][1] = lam;
                } else  { 
                    if (lam < lambda[ii][jj][0]){ lambda[ii][jj][0] = lam; }
                }

                if ((cr && (lam < -tol) ) || (ncr && (lam > tol)))
                {
                    printf("not ok 2 cr %d lam %lf iijj %d %d \n",cr,lam,ii,jj);
					need_subdiv = 1; // need to be subdivied
                }
                    if(debugchoice & EXTTRI) {
                        set_color(ncr);
                        if(!ncr)  // only outter enclosure
                        DrawTri(env[cr][sd], env[cr][s2], env[cr][sm]);
                    }
                //else printf(" ok 2 \n");

                lam = plane_intersect(sup_pt[ii][jj], sup_nor[ii][jj], 
                    env[ncr][sd], env[ncr][sp], env[ncr][sm]);
                if (ncr) { if (lam > lambda[ii][jj][1])  lambda[ii][jj][1] = lam;
                } else  { 
                    if (lam < lambda[ii][jj][0]) { lambda[ii][jj][0] = lam; }
                }


                if ((cr && (lam > tol) ) || (ncr && (lam < -tol)))
                {
                    printf("not ok 3 cr %d lam %lf iijj %d %d \n",cr,lam,ii,jj);
					need_subdiv = 1; // need to be subdivied
                }
                    if(debugchoice & EXTTRI) {
                        set_color(cr);
                        if(!cr)  // only outter enclosure
                        DrawTri(env[ncr][sd], env[ncr][sp], env[ncr][sm]);
                    }

                //else printf(" ok 3 \n");

                cr = ncr;  // crease opposite at next point
            }
            // w...[1] has pos (in normal dir) values
        }
    }

	// subdivide the patch if needed
	return need_subdiv;
}


// -------------------------------------------------------------------
// generate the final triangulated enclosures
void EncQuadBezier::make_tri()
{
    int i,j, nordir;
    REAL*  hv;

	// compute the enclosure
    for (i=0; i<=segu; i++) {
        for (j=0; j<=segv; j++) { // for each point 
            for (nordir=0; nordir<2; nordir++) { // 1 = normal dir
                hv = get_enc(nordir, i,j);
                VVadd(1.0,sup_pt[i][j],
                      lambda[i][j][nordir], sup_nor[i][j],
                      hv);

		        if(debugchoice & SUPNOR)
		        {
		            glDisable(GL_LIGHTING);
		            glColor3f(1.0, 0.0, 0.0);
	                glBegin(GL_LINES);
		               glVertex3dv(sup_pt[i][j]);
		               glVertex3dv(hv);
	                glEnd();

	                glPointSize(3.0);
		               glBegin(GL_POINTS);
		               glVertex3dv(sup_pt[i][j]);
		               glVertex3dv(hv);
	                glEnd();
		            glEnable(GL_LIGHTING);
		        }
		    }
        }
    }
}


/* find the neighbor of face fp at side sd,
 * return the index of the neighbor and the side of the neighbor
 *
int find_neighbor_pat(int fc, int sd, int* nfc, int* nsd, 
         p_patch Face, int FNum)
{
	p_patch fp = &Face[fc];
	p_patch fpn;
	int sides = get_n_pat(fp); // 4 sides? 3 sides?

	REAL* v1 = get_v_pat(fp, sd);
	REAL* v2 = get_v_pat(fp, (sd+1)%sides);
	int i, s;
	REAL * nv1, * nv2;

	for(i=1; i<=FNum; i++)
	{
		if(i==fc) continue;  // same face
		fpn = &Face[i];
	    int sidesn = get_n_pat(fp); // 4 sides? 3 sides?

		for(s=0;s<sidesn;s++) {
           nv1 = get_v_pat(fpn, s);
	       nv2 = get_v_pat(fpn, (s+1)%sidesn);
		   if(VVequal(nv1,v2) && VVequal(nv2,v1))
		   {
			   *nfc = i;
			   *nsd = s;
			   return 1;
		   }
		}
	}
	return 0;
}
*/


// -------------------------------------------------------------------
// main function:
// Compute the surface envelope
// 
void EncQuadBezier::compute_enclosure()
{
    //int tr[4][4][2];   // translation from xy to ccw 
	int need_subdiv;

	// allocate the memory storing the results
    o_enc = alloc_mem_db(d1*d1*DIM);
    i_enc = alloc_mem_db(d1*d1*DIM);
    cralong = (int *) allocate (sizeof (int) * segu*segv );

	// compute the bilinear envelope
	make_env();

	// determine the support points and normals
	make_sup();

	// average normals along the boundary between two neighboring
	// patch. (there is no affect for C1 surfaces)
	//
	//average_nor_PN(fp, sup_nor[fc], tr);  // temp hack!!: use PN average Norm
	
	// compute and store intersection lambdas in w's
	need_subdiv = make_lam();

#ifdef FIX_BY_SUBDIVIDE

	if(need_subdiv)
	{
		int sizeu = dg*2+1; // 
		int sizev = dg*2+1; // 
		REAL bb[sizeu*sizev][DIM];  // space for subdivision

		// subdivide the patch
		for(i=0;i<d1;i++) {
		    for(j=0;j<d1;j++) {
				Vcopy( get_bb[i][j], bb[(i*2)*sizev+(j*2)]);
			}
		}
		RSubDiv(bb, 2, dg, dg, sizeu-1, sizev-1);

		/*
		for(i=0;i<sizeu;i++)
		   for(j=0;j<sizev;j++) {
		    printf("v: %f %f %f \n", bb[i*sizev+j][0], bb[i*sizev+j][1],
							bb[i*sizev+j][2]);
		}
		*/

		// si and sj are the starting (i,j) position for subpatches
		for(si = 0; si<=dg; si+=dg)
		for(sj = 0; sj<=dg; sj+=dg) {
			int sub_fc; // where to place this new subdivied patch

			// use the first subdivided patch to overwrite 
			// the original patch
			if(si==0 && sj==0) 
				sub_fc = fc;   
			else
			{
			    sub_fc = new_patch(FNum, Face, fc);
				FNum++;
				printf("Add a new face %d, now %d faces\n", sub_fc, FNum);
			}

			for(i=0;i<d1;i++)
			  for(j=0;j<d1;j++)
			    Vcopy(bb[(i+si)*sizev+(j+sj)],
								&(Face[sub_fc].buf[(i*d1+j)*DIM]));
		}

	    // increase number of patches by 3
	    object[index].patch_num +=3;
	    fc --;  // move back one spot to recompute the subdivided one
	    printf("fc = %d\n", fc);

		// add four patches into the array
		// disable the current patch (quick way to delete)  
	}
#endif

	// this should after global lambda fix
    make_tri(); 

	enc_computed = true;
}



// ..................................................................
//
//       ploting and output routines                

// plot the enclosure polygon
void EncQuadBezier::plot_enc_patch(int nordir)
{
    int i,j;	
	REAL* p1, *p2, *p3, *p4;
	int cr;

	if(!enc_computed)
		compute_enclosure();

	//return;
    //set_color(1);
	//glEnable(GL_AUTO_NORMAL);
	glEnable(GL_LIGHTING);
	//glShadeModel(GL_FLAT);
    for (i=0; i<segu; i++) {
        for (j=0; j<segv; j++) { //per bilinear facet 

        	p1 = get_enc(nordir, i,j);
        	p2 = get_enc(nordir, i,j+1);
        	p3 = get_enc(nordir, i+1,j+1);
        	p4 = get_enc(nordir, i+1,j);

        	cr = (cralong[i*(segv)+j]+nordir)%2;  // opposite creases in/out
    
        	if(cr==0) {
				DrawTri(p1, p3, p2);
				DrawTri(p3, p1, p4);

        	}else if(cr ==1) {
				DrawTri(p1, p4, p2);
				DrawTri(p2, p4, p3);
        	}
		}
    } 
	//glShadeModel(GL_SMOOTH);
}


// plot the mesh for the enclosure 
void EncQuadBezier::plot_enc_mesh(int nordir)
{
    int i,j;	
	REAL* p1, *p2, *p3, *p4;
	int cr;

	if(!enc_computed)
		compute_enclosure();

    for (i=0; i<segu; i++) {
        for (j=0; j<segv; j++) { //per bilinear facet 

        p1 = get_enc(nordir, i,j);
        p2 = get_enc(nordir, i,j+1);
        p3 = get_enc(nordir, i+1,j+1);
        p4 = get_enc(nordir, i+1,j);

        cr = (cralong[i*segv+j]+nordir)%2;  // opposite creases in/out
    
        if(cr==0) {
		    glBegin(GL_LINE_LOOP);
			glVertex3dv(p1); glVertex3dv(p3); glVertex3dv(p2);
			glEnd();

		    glBegin(GL_LINE_LOOP);
			glVertex3dv(p3); glVertex3dv(p1); glVertex3dv(p4);
			glEnd();

        }else if(cr ==1) {
		    glBegin(GL_LINE_LOOP);
			glVertex3dv(p1); glVertex3dv(p4); glVertex3dv(p2);
			glEnd();

		    glBegin(GL_LINE_LOOP);
			glVertex3dv(p4); glVertex3dv(p3); glVertex3dv(p2);
			glEnd();
        }
	}
    } 
}


// create a new patch
/*
int new_patch(int FNum, p_patch Face, int fc)
{
	int new_fc;
	new_fc = FNum+1;
    p_patch new_pat, clone_pat;
	int num_points;
	int size2;
    
	new_pat = &Face[new_fc];
	clone_pat = &Face[fc];

	new_pat->type = clone_pat->type;
	new_pat->degu = clone_pat->degu;
	new_pat->degv = clone_pat->degv;
	num_points = new_pat->num_points = clone_pat->num_points;

	new_pat->buf = alloc_mem_db(num_points*DIM);
	new_pat->o_enc = alloc_mem_db(num_points*DIM);
	new_pat->i_enc = alloc_mem_db(num_points*DIM);

	size2 = new_pat->degu * new_pat->degv;  // one int for each bi-linear facet
	new_pat->cralong = (int *) allocate (sizeof (int) * size2 );

	return new_fc;
}
*/



//////////////////////////////////////////////////////////////////////////
//
// output a BezierView format file contains the enclosure 
//
void EncQuadBezier::outputtofile(FILE* fpw) 
{
    int i,j, nordir, cr;
    int p1, p2, p3, p4;
    REAL* v;

    // output enclosures (set of triangles) 
    for(nordir=0; nordir<2; nordir++)
    {
		if(nordir)
           fprintf(fpw, "group %d %s\n", nordir+1, "outter_enclosure" );
		else
           fprintf(fpw, "group %d %s\n", nordir+1, "inner_enclosure" );

        fprintf(fpw, "1 %d %d\n", (segu+1)*(segv+1), segu*segv*2);

        for (i=0; i<=segu; i++) {
            for (j=0; j<=segv; j++) { //per bilinear facet
                v = get_enc( nordir, i,j);
                fprintf(fpw,"%f %f %f \n", v[0], v[1], v[2]);
            }
        }

        for (i=0; i<segu; i++) {
            for (j=0; j<segv; j++) { //per bilinear facet 

                cr = (cralong[i*segv+j]+nordir)%2;  // opposite creases in/out
                p1 = i*(segv+1) + j; 
                p2 = p1 + 1;
                p3 = p2 + (segv+1); 
                p4 = p1 + (segv+1);
    
                if(cr==0) {
                    fprintf(fpw, "3 %d %d %d\n", p1, p3, p2);
                    fprintf(fpw, "3 %d %d %d\n", p3, p1, p4);
                }else if(cr ==1) {
                    fprintf(fpw, "3 %d %d %d\n", p1, p4, p2);
                    fprintf(fpw, "3 %d %d %d\n", p4, p3, p2);
                }
            }
        }
    } 

    // export bi3 patches
    fprintf(fpw, "group 3 surface\n");
    fprintf(fpw,"5\n %d %d \n", degu, degv);

    for (i=0; i<=degu; i++) {
        for (j=0; j<=degv; j++) { 
            v = get_bb(i,j);
	   		fprintf(fpw,"%f %f %f \n", v[0],v[1],v[2]);
        }
    }
    fprintf(fpw,"\n");
}
// ..................................................................
