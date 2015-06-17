/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    rotate.c
 *  Purpose: Mouse event handling (rotating and clipping)
 * -------------------------------------------------------------
 */
/* Modified from a rotating cube demo with trackball in openGL book */
#include "stdheaders.h"
#include "type.h"
#include "patch.h"
#include "glob.h"
#include "menu.h"
#include "util.h"
#include "curvature.h"
#include "highlight.h"
#include "draw.h"
#include "pick.h"
#include "rotate.h"

REAL angle = 0.0, axis[3];

bool 	trackingMouse = false;
bool 	redrawContinue = false;
bool    trackballMove = false;

int     clipping = 0;
int     clip_item = 0;
int     clipping_grp = 1;

int     rotate_plane = 0;
int     shift_plane = 0;
extern REAL* clip_plane;
bool	zoomMotion = false;	
bool	moveMotion = false;

int     scale_moving = -1;  // curvature scale adjusting

int     pick(int x, int y);
void    define_crv();
//void	define_crv_needle();

// two clicks within this time interval considered as double click
int     dcMicroSecond = 300; // microsecond

clock_t  lastmotion = 0; // last motion moment
                        // if user keep the mouse stopped
                        // for more than 1 second, then
                        // no rotation after he releases it.
clock_t last_click =0 ;

void updateProjection();

/*----------------------------------------------------------------------*/
/* 
** These functions implement a simple trackball-like motion control.
*/

REAL lastPos[3] = {0.0, 0.0, 0.0};
int curx, cury;
int startX, startY;

void trackball_ptov(int x, int y, int width, int height, REAL v[3])
{
    REAL d, a;

   /* --- project x,y onto a hemi-sphere centered within width, height */
    v[0] = (2.0*x - width) / width;
    v[1] = -(2.0*y - height) / height; 
	d = v[0]*v[0] + v[1]*v[1];
	d = (d < 1.0) ? d : 1.0; /* keep below or equal to 1 */
    v[2] = sqrt(1-d);
	a = sqrt(d + v[2]*v[2]);
    v[0] /= a;
    v[1] /= a;
    v[2] /= a;
}

void rotate_vector(REAL* v, REAL* axis, REAL angle)
{
	REAL temp[4];
	REAL scr_axis[4];
	REAL d, rad;
	int i;

	Vcopy(axis, scr_axis);
	Normalize(scr_axis);

    double mv_matrix[16];
    glMatrixMode(GL_MODELVIEW);
    glGetDoublev(GL_MODELVIEW_MATRIX, mv_matrix);

	Solve4(mv_matrix, scr_axis);

	VVcross(v, scr_axis, temp);
	d = VVmult(v, scr_axis);
	rad = -angle/180*3.14159;

	for(i=0;i<3;i++)
	{
		v[i] = v[i]*cos(rad) + scr_axis[i] * d * (1-cos(rad)) + temp[i] * sin(rad) ;
	}
}

void zoom(float delta){
    scale_factor *= 1 + delta*0.001f;
}

void motionZoom(int x, int y){
	if(!zoomMotion) {
	    startX = x; startY = y;
		zoomMotion = true;
		return;
	}
    scale_factor *= 1+ (startY-y)*0.005f; //(y-startY)/winHeight;

    startX = x; startY = y;

}
void motionMove(int x, int y){
	if(!moveMotion) {
	    startX = x; startY = y;
		moveMotion = true;
		return;
	}
    ViewCenter[0] += 0.005f* ViewSize * (startX- x);
    ViewCenter[1] -= 0.005f* ViewSize * (startY- y);
    startX = x; startY = y;
	updateProjection();
}


void mouseMotion(int x, int y, KeyboardModifier modifiers )
{
    REAL curPos[3], dx, dy, dz;
	REAL plane_angle;
	REAL plane_axis[3];
    lastmotion =clock();  // record the last motion time
   
    trackball_ptov(x, y, winWidth, winHeight, curPos);

    if(modifiers==ShiftModifier) {
		motionZoom(x,y);
		return;
	}

    if(modifiers==AltModifier) {
		motionMove(x,y);
		return;
	}

    if(! (modifiers==ControlModifier) ) {
		if(g_mouseMode == ZOOM ) {
			motionZoom(x,y);
			return;
		}

		if(g_mouseMode == MOVE ) {
			motionMove(x,y);
			return;
		}
	}

	if(rotate_plane)
    {
		dx = curPos[0] - lastPos[0];
		dy = curPos[1] - lastPos[1];
		dz = curPos[2] - lastPos[2];

		if (dx || dy || dz) {
	    	plane_angle = 90.0 * sqrt(dx*dx + dy*dy + dz*dz);

	    	/* axis = Pold x P */
	    	plane_axis[0] = lastPos[1]*curPos[2] - lastPos[2]*curPos[1];
	    	plane_axis[1] = lastPos[2]*curPos[0] - lastPos[0]*curPos[2];
	    	plane_axis[2] = lastPos[0]*curPos[1] - lastPos[1]*curPos[0];

	    	lastPos[0] = curPos[0];
	    	lastPos[1] = curPos[1];
	    	lastPos[2] = curPos[2];
			rotate_vector( &clip_plane[cur_clipping_plane*4], plane_axis, plane_angle);
			//define_crv_needle();
		}

	}
	else if(shift_plane)
	{
		dx = curPos[0] - lastPos[0];
		dy = curPos[1] - lastPos[1];
		dz = curPos[2] - lastPos[2];
		if (dx || dy || dz) {
	    	lastPos[0] = curPos[0];
	    	lastPos[1] = curPos[1];
	    	lastPos[2] = curPos[2];
		}
		clip_plane[cur_clipping_plane*4+3] += dy*10;
//		printf("plane %d moved: %f from %f", cur_clipping_plane, clip_plane[cur_clipping_plane*4+3], 
//			clip_plane[cur_clipping_plane*4+3] - dy*100);

		//define_crv_needle();

//		printf("plane %d moved: %f %f %f\n", cur_clipping_plane, dx, dy, dz);

	}
    else if(scale_moving >=0) // mouse is adjusting the scale of the curvature
    {
        adjust_scale(scale_moving, y, winHeight); 
    }
    else if(trackingMouse)
    {
		dx = curPos[0] - lastPos[0];
		dy = curPos[1] - lastPos[1];
		dz = curPos[2] - lastPos[2];

		if (dx || dy || dz) {
	    	angle = 90.0 * sqrt(dx*dx + dy*dy + dz*dz);

	    	/* axis = Pold x P */
	    	axis[0] = lastPos[1]*curPos[2] - lastPos[2]*curPos[1];
	    	axis[1] = lastPos[2]*curPos[0] - lastPos[0]*curPos[2];
	    	axis[2] = lastPos[0]*curPos[1] - lastPos[1]*curPos[0];

	    	lastPos[0] = curPos[0];
	    	lastPos[1] = curPos[1];
	    	lastPos[2] = curPos[2];
		}
    }
    else if(clipping)
    {

        switch(clip_item)
        {
        case CLIPNEAR:
			//group[clipping_grp].ViewNear = (double)y/winHeight;
            ClipNear = -((float)y/winHeight - 0.5f)*ViewSize/2;
			break;
        case CLIPFAR:
			//group[clipping_grp].ViewFar = 
            ClipFar = -((float)y/winHeight - 0.5f)*ViewSize/2;
			break;
        default:
			return;
        }
        updateProjection();
    }
  //  draw();
    
}

void startMotion(int x, int y)
{
    trackingMouse = true;
    redrawContinue = false;
    startX = x; startY = y;
    curx = x; cury = y;
    trackball_ptov(x, y, winWidth, winHeight, lastPos);
    trackballMove=true;
	axis[0]=axis[1]=axis[2] = 0;
}

void startZoom(int x, int y)
{
    startX = x; startY = y;
}

void startMove(int x, int y)
{
    startX = x; startY = y;
}

void stopMotion(int x, int y)
{
    /* get the current time */
    clock_t cur;
    cur = clock();  // used at the end of this function
    
    trackingMouse = false;

    if ( startX != x || startY !=y) {
        redrawContinue = true;
    } else {
		angle = 0.0;
		redrawContinue = false;
		trackballMove = false;
    }

	angle = 0.0;
	redrawContinue = false;
	trackballMove = false;

	zoomMotion = false;	
	moveMotion = false;

    if( ((double) (cur-lastmotion))/CLOCKS_PER_SEC >= 0.1) // if user stopped the mouse for .2 second
                           // no rotation after he releases it.
    {
	    angle = 0.0;
		redrawContinue = false;
		trackballMove  = false;
    }
}

/*----------------------------------------------------------------------*/

void display(void)
{
    /* Use to save the previous modelview matrix */
    GLdouble modelview[16];

    glClearColor(g_BackColor[back_choice][0], g_BackColor[back_choice][1],
                    g_BackColor[back_choice][2], 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    /* modelview transform */
    if (trackballMove)
    {
        glMatrixMode(GL_MODELVIEW);
        glGetDoublev( GL_MODELVIEW_MATRIX, modelview);
        glLoadIdentity();

        glRotated(angle*1.5, axis[0], axis[1], axis[2]);
        glMultMatrixd(modelview);
    }
    draw();
}

/*----------------------------------------------------------------------*/

void mouseButton(int button, int state, int x, int y, KeyboardModifier modifiers)
{
    //int picked;
	clock_t cur = 0;
    // two clicks within this time interval considered as double click
    double  dcTime = 0.3; // microsecond


    /*   button to popup menu */
    if(button==2){

    }
    if(button==1)  {  //left button

       if(state == 1)
       {

		   if(clip_item) {
             clipping = 1;
			 return;
		   }
       }
       else
          clipping = 0;
    }

    /* left button to pick */
    if(button==1)
    switch(state) 
    {
    case 1:

        if((modifiers==ControlModifier) && (cur_clipping_plane <0) )
        {
			startMotion(x,y);
			return;
        }
        if( (ShiftModifier==modifiers) && (cur_clipping_plane <0) )
		{
			startZoom(x,y);
			return;
		}
        if((modifiers==AltModifier) && (cur_clipping_plane <0) )
		{
			startMove(x,y);
			return;
		}

        // first, check if the mouse is clicked on the curvature bar
        if ( (isDisplayFlagEnabled(g_current_grp, DRAWCRV)|| isDisplayFlagEnabled(g_current_grp, DRAWCRVNEEDLE) )
			&& ((scale_moving=clickon_crv_bar(x,y,winWidth,winHeight)) != -1)) 
        {
            return; 
        }

        // double click check
        if(!redrawContinue) {
		   cur = clock();
		   if( ((double) (cur-last_click))/CLOCKS_PER_SEC <= dcTime) 
		   { 
			   static int selItem = -1;
               int picked = pick(x, y);
               if(picked > 0)
               {
                  // de-select 
                  if( selItem == picked)
                  {
                     selItem = -1;
                  }
                  // select 
                  else
                  {
                     selItem = picked;
                     //printf("patch %d selected\n", selItem);
                  }
               }
			}
		}


        if((modifiers==ControlModifier) && (cur_clipping_plane >=0) )
		{
			rotate_plane = 1;
		}
		else 
			rotate_plane = 0;
        if( (modifiers==ShiftModifier) && (cur_clipping_plane >=0) )
		{
			shift_plane = 1;
		}
		else 
			shift_plane = 0;

		if(g_mouseMode ==	ROTATE)
			startMotion( x,y);
		else if(g_mouseMode ==	MOVE)
			startMove( x,y);
		else
			startZoom( x,y);

        break;
    case 2:  //mouse release
//		if(g_mouseMode ==	ROTATE || g_mouseMode ==	MOVE)
//			return;

        if(scale_moving >=0 )  //  curvature scale adjusting stops
        {
            define_crv();   // re draw the curvature
            scale_moving = -1;
        }
        else {                  // track ball rotation control stops
            stopMotion( x,y);
		}
		cur = clock();
		last_click = cur;
        break;
    }
}

void spin()
{
 //  if (redrawContinue) updateGL();
}


