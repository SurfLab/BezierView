/* ------------------------------------------------------------
 *  BezierView 
 *    SurfLab, Univ Of Florida
 *
 *  File:    light.c
 *  Purpose: set the light and material property of the surface
 * -------------------------------------------------------------
 */

#include <stdheaders.h>
#include "glob.h"

void set_lights();

// each time add/remove a color, change the #define COLORNUM in glob.h
// Note: keep the transparent be the last one and the
//       random be the next to last one.
char mat_name[][20] = {
    "gold", "emerald", "ruby", "silver", "bronze", "yellow plastic", 
	"chrome", "pewter", "cyan plastic", "Jade", "random",
         "transparent"
};

#define ALPHA 0.5
GLfloat mat_ambient[][4] = {
        {0.24725 , 0.1995 , 0.0745, ALPHA}, // gold
        {0.0215, 0.1745,  0.0215,  ALPHA},  // emerald
        {0.1745, 0.01175, 0.01175, ALPHA},  // ruby
        {0.19225, 0.19225, 0.19225,ALPHA }, // silver
        {0.2125, 0.1275, 0.054, ALPHA},     // bronze
        {0.0, 0.0, 0.0, ALPHA},             // yellow plastic
        {0.45, 0.45, 0.45, ALPHA},          // chrome
        {0.10588, 0.058824, 0.113725, ALPHA}, // Pewter
        {0.0, 0.1, 0.06, ALPHA},            // cyan plastic
		{0.135, 0.2225, 0.1575, ALPHA},        // Jade
        {0.0, 0.0, 0.0, ALPHA},             // random
        {0.0, 0.0, 0.0, ALPHA},             // transparent
};

GLfloat mat_diffuse[][4] = {
        {0.75164, 0.60648 ,0.22648, ALPHA}, // gold
        {0.07568, 0.61424, 0.07568, ALPHA}, // emerald
        {0.61424, 0.04136, 0.04136, ALPHA}, // ruby
        {0.50754, 0.50754, 0.50754, ALPHA },// silver
        {0.714,   0.4284,  0.18144, ALPHA },// bronze
        {0.5, 0.5, 0.0, ALPHA},             // yellow plastic
        {0.4, 0.4, 0.4, ALPHA},             // chrome
        {0.427451, 0.470588, 0.541176,  ALPHA}, // Pewter 
        {0.0, 0.51, 0.51, ALPHA},           // cyan plastic
		{0.54, 0.89, 0.630, ALPHA},             // Jade
        {0.0, 0.0, 0.0, 0.0},             // random
        {0.0, 0.0, 0.0, 0.0},             // transparent
};

GLfloat mat_specular[][4] = {
        {0.628281, 0.555802, 0.366065, ALPHA},  // gold
        {0.633, 0.727811, 0.633, ALPHA},        // emerald
        {0.727811, 0.626959, 0.626959, ALPHA},  // ruby
        {0.508273, 0.508273, 0.508273, ALPHA},  // silver
        {0.393548, 0.271906, 0.166721, ALPHA},  // bronze
        {0.6, 0.6, 0.5, ALPHA},                 // yellow plastic
        {0.774597, 0.774597, 0.774597, ALPHA},  // chrome
        {0.3333, 0.3333, 0.521569, ALPHA},      // Pewter
        {0.502, 0.502, 0.502, ALPHA},           // cyan plastic
		{0.316228, 0.316228, 0.316228, ALPHA},     // Jade
        {0.0, 0.0, 0.0, 0.0},                 // random
        {0.0, 0.0, 0.0, 0.0},                 // transparent
};

GLfloat mat_shininess[] = {
    0.4, 0.6, 0.6, 0.4, 0.2, 0.25, 76.8/128, 0.2, 0.25, 0.1, 0.0
};

/* choose a color using its index */
void set_color(int c)
{
    int color =c;
    float MatAmbientBack[]  = {0.0f, 0.2f, 0.0f, 1.0f};
glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    //glMaterialfv(GL_BACK, GL_AMBIENT, mat_ambient[6]);
    //glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular[6]);
    //glMaterialfv(GL_BACK, GL_DIFFUSE, mat_diffuse[6]);
    //glMaterialf(GL_BACK, GL_SHININESS, mat_shininess[6]*128);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient[color]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular[color]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse[color]);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess[color]*128);

 
  glMaterialfv(GL_BACK, GL_AMBIENT, MatAmbientBack);
 
}

/*
 * initial the lighting in the scene
 */
void light_init()
{
    GLfloat light_specular0[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular1[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular2[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_diffuse0[] ={1.0, 1.0, 1.0, 1.0};
    GLfloat light_diffuse1[] ={1.0, 1.0, 1.0, 1.0};
    GLfloat light_diffuse2[] ={1.0, 1.0, 1.0, 1.0};
    GLfloat light_ambient0[]={0.4, 0.4, 0.4, 1.0};
    GLfloat light_ambient1[]={0.2, 0.2, 0.2, 1.0};
    GLfloat light_ambient2[]={0.8, 0.8, 0.8, 1.0};

    GLfloat light_position0[]={3.0,1.0,1.0,0.0};
    GLfloat light_position1[]={-1.0,-3.0,-1.0,0.0};
    GLfloat light_position2[]={0.0,30.0,0.0,0.0};

/* set up ambient, diffuse, and specular components for light 0 & 1*/

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse0);

    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse1);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);

    glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  light_diffuse2);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  light_ambient0);

/* define material proerties for front face of all polygons */

    //set_color(3);

    glEnable(GL_LIGHTING); /* enable lighting */
	light_switch[0] = 0;   /* Disable light 0 */
	light_switch[1] = 0;   /* Disable light 1 */
	light_switch[2] = 1;   /* Enable  light 2 */
	set_lights();


    glEnable(GL_DEPTH_TEST); /* enable z buffer */
	//glDisable(GL_LIGHT_MODEL_TWO_SIDE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
}


/* turn on/off the lights */
void set_lights()
{
	if(light_switch[0])    // light 0
	    glEnable(GL_LIGHT0);  
	else
		glDisable(GL_LIGHT0);

	if(light_switch[1])   // light 1
	    glEnable(GL_LIGHT1);  
	else
		glDisable(GL_LIGHT1);

	if(light_switch[2])   // light 2
	    glEnable(GL_LIGHT2);  
	else
		glDisable(GL_LIGHT2);
}

