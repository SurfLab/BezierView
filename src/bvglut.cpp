#include "stdheaders.h"
#include "glob.h"
#include "bview.h"
#include "draw.h"


/* ------------------------------------------------------------------
 *    Main function
 *
 */

int glut_main( int argc, char *argv[])
{
    init_bezierview(argc,argv);
    /* glut initialization */
    windowinit(argc, argv);

    initGL();

    /* create menus */
    //menu_init();

    glutMainLoop(); /* enter event loop */

    return 0;
}

void glut_display(){
    display();
    glutSwapBuffers();
}


/* ------------------------------------------------------------------
 *  initialize the window
 */
void windowinit(int argc, char *argv[])
{

    char title[255];
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(winWidth,winHeight);  /* 700 x 700 pixel window */
    glutInitWindowPosition(80,80);  /* place window top left on display */
    sprintf(title, "%s - SurfLab (ESC to quit)", argv[0]);
    glutCreateWindow(title); /* window title */

    glutKeyboardFunc(keyboard);   /* set glut callback functions */
    glutSpecialFunc(advkeyboard);   /* set glut callback functions */
    glutIdleFunc(spin);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutDisplayFunc(glut_display);


}


