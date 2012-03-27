/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    pick.c
 *  
 *  Purpose:   
 *       patch picking (not available right now)
 * -------------------------------------------------------------
 */

#include <stdio.h>
#include <GL/glut.h>
#include "type.h" // GAUSS
#include "glob.h"

#define BUFSIZE 512


/* picking */
int pick(int x, int y)
{

    GLuint hits = 0;
    GLint viewport[4];
    GLuint selectBuf[BUFSIZE];
    GLuint *ptr;
    GLuint i,j;
    GLint  k;
    GLint picked = -1;
	REAL mv_matrix[16];

    GLuint names, z1, z2, zmax;
    
    glMatrixMode(GL_MODELVIEW);
    glGetDoublev( GL_MODELVIEW_MATRIX, mv_matrix);
        
    glGetIntegerv( GL_VIEWPORT, viewport);
    glSelectBuffer(BUFSIZE, selectBuf);
    
    glRenderMode(GL_SELECT);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix( (GLdouble) x, (GLdouble) (viewport[3] - y), 2.0,2.0,
    			viewport);
    glOrtho(ViewCenter[0]-ViewSize,ViewCenter[0]+ViewSize, 
		    ViewCenter[1]-ViewSize,ViewCenter[1]+ViewSize, 
            -3*ViewDepth*ViewSize, 3*ViewDepth*ViewSize);

    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    glLoadMatrixd(mv_matrix);
	glTranslated(-ObjectCenter[0], -ObjectCenter[1], -ObjectCenter[2]);

    glInitNames();
    glPushName((unsigned) -1);

    for (k= 1; k <=patch_num  ; k++)
    {
        glLoadName(k);
//		glCallList(patchList(k));
    }
    
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    hits = glRenderMode(GL_RENDER);
    //printf("hits %d \n", hits);

    ptr = selectBuf;

    for (i = 0; i < hits; i++) { /*  for each hit  */
      names = *ptr;

      if(names !=1 ){
          //printf("something wrong?\n"); 
          break;  // wierd case, openGL bug? 
      }
      
      ptr++;
      z1 = *ptr;
      ptr++;
      z2 = *ptr;
      ptr++;

      // printf("names %d \n", names);
      
      if( i ==0 || z2 > zmax )
      {
          //printf("z: %ud \n", z2);
          zmax = z2;

        
          for (j = 0; j < names; j++) {     /*  for each name */
             if ( (*ptr <= (GLuint) patch_num) && (*ptr >= 1) )
             {
                picked = *ptr;
//                return picked;
             }
             ptr++;
          }
       }
    }
   
    return picked;
}


