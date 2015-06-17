#include "stdheaders.h"
#include "type.h"
#include "patch.h"
#include "util.h"
#include "export.h"
#include "polygon.h"
#include "quadbezier.h"
#include "tribezier.h"

///////////////////////////////////////
//
// output to eps file
void export_eps(Patch face[], int patch_num, float ObjectCenter[3], double scale_factor)
{
    int i;
    FILE* fp;
    int j;
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
            Patch * poly = p;

            for (j=0; j<poly->pointCount; j++) {
                gluProject(
                    poly->position[j][0],
                    poly->position[j][1],
                    poly->position[j][2],
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
    fprintf(fp, "showpage\n");

    glPopMatrix();
    fclose(fp);
}



///////////////////////////////////////
// output igs file
int knots(int dg1, int bbase, int ffctr, FILE* fp, int per_line);

void export_igs(Patch face[], int patch_num)

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
            Patch * tri = p;
            if(tri->degu == 1) {
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

            Patch * quad = p;

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
            Patch * tri = p;

            if(tri->degu == 1) {
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
                            hv=TriBezier_get_bb(tri,0,1);
                        else
                            hv=TriBezier_get_bb(tri,i,j);

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

            Patch * quad = p;

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
                    hv=QuadBezier_get_bb(quad,i,j);
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


