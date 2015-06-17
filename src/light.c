/* ------------------------------------------------------------
 *  BezierView 
 *    SurfLab, Univ Of Florida
 *
 *  File:    light.c
 *  Purpose: set the light and material property of the surface
 * -------------------------------------------------------------
 */
#include "stdheaders.h"
#include "type.h"
#include "light.h"

void updateLights();

// each time add/remove a color, change the #define COLORNUM in glob.h
// Note: keep the transparent be the last one and the
//       random be the next to last one.
char mat_name[][20] = {
    "gold", "emerald", "ruby", "silver", "bronze", "yellow plastic", 
	"chrome", "pewter", "cyan plastic", "Jade", "random",
         "transparent"
};

#define ALPHA 0.5
float mat_ambient[][4] = {
        {0.24725f,0.1995f,0.0745f,ALPHA},// gold
        {0.0215f,0.1745f, 0.0215f, ALPHA}, // emerald
        {0.1745f,0.01175f,0.01175f,ALPHA}, // ruby
        {0.19225f,0.19225f,0.19225f,ALPHA },// silver
        {0.2125f,0.1275f,0.054f,ALPHA},    // bronze
        {0.0f,0.0f,0.0f,ALPHA},            // yellow plastic
        {0.45f,0.45f,0.45f,ALPHA},         // chrome
        {0.10588f,0.058824f,0.113725f,ALPHA},// Pewter
        {0.0f,0.1f,0.06f,ALPHA},           // cyan plastic
        {0.135f,0.2225f,0.1575f,ALPHA},       // Jade
        {0.0f,0.0f,0.0f,ALPHA},            // random
        {0.0f,0.0f,0.0f,ALPHA},            // transparent
};

float mat_diffuse[][4] = {
        {0.75164f, 0.60648f, 0.22648f, ALPHA}, // gold
        {0.07568f, 0.61424f, 0.07568f, ALPHA}, // emerald
        {0.61424f, 0.04136f, 0.04136f, ALPHA}, // ruby
        {0.50754f, 0.50754f, 0.50754f, ALPHA },// silver
        {0.714f,   0.4284f,  0.18144f, ALPHA },// bronze
        {0.5f, 0.5f, 0.0f, ALPHA},             // yellow plastic
        {0.4f, 0.4f, 0.4f, ALPHA},             // chrome
        {0.427451f, 0.470588f, 0.541176f,  ALPHA}, // Pewter
        {0.0f, 0.51f, 0.51f, ALPHA},           // cyan plastic
        {0.54f, 0.89f, 0.630f, ALPHA},             // Jade
        {0.0f, 0.0f, 0.0f, 0.0},             // random
        {0.0f, 0.0f, 0.0f, 0.0},             // transparent
};

float mat_specular[][4] = {
        {0.628281f, 0.555802f, 0.366065f, ALPHA},  // gold
        {0.633f, 0.727811f, 0.633f, ALPHA},        // emerald
        {0.727811f, 0.626959f, 0.626959f, ALPHA},  // ruby
        {0.508273f, 0.508273f, 0.508273f, ALPHA},  // silver
        {0.393548f, 0.271906f, 0.166721f, ALPHA},  // bronze
        {0.6f, 0.6f, 0.5f, ALPHA},                 // yellow plastic
        {0.774597f, 0.774597f, 0.774597f, ALPHA},  // chrome
        {0.3333f, 0.3333f, 0.521569f, ALPHA},      // Pewter
        {0.502f, 0.502f, 0.502f, ALPHA},           // cyan plastic
        {0.316228f, 0.316228f, 0.316228f, ALPHA},     // Jade
        {0.0f, 0.0f, 0.0f, 0.0},                 // random
        {0.0f, 0.0f, 0.0f, 0.0},                 // transparent
};

float mat_shininess[] = {
    0.4f, 0.6f, 0.6f, 0.4f, 0.2f, 0.25f, 76.8f/128, 0.2f, 0.25f, 0.1f, 0.0
};

/* choose a color using its float rgb values */

void set_colorf(color_t c){

    float MatAmbientBack[]  = {0.0f, 0.2f, 0.0f, 1.0f};
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    float* color = &c.red;

    glMaterialfv(GL_FRONT, GL_AMBIENT, color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular[0]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess[0]*128);


    glMaterialfv(GL_BACK, GL_AMBIENT, MatAmbientBack);
}


/* choose a color using its index */
void set_color(int c)
{
    int color =c;
    float MatAmbientBack[]  = {0.0f, 0.2f, 0.0f, 1.0f};
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);


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
    float light_specular0[]={1.0f, 1.0f, 1.0f, 1.0f};
    float light_specular1[]={1.0f, 1.0f, 1.0f, 1.0f};
    float light_specular2[]={1.0f, 1.0f, 1.0f, 1.0f};
    float light_diffuse0[] ={1.0f, 1.0f, 1.0f, 1.0f};
    float light_diffuse1[] ={1.0f, 1.0f, 1.0f, 1.0f};
    float light_diffuse2[] ={1.0f, 1.0f, 1.0f, 1.0f};
    float light_ambient0[]={0.4f, 0.4f, 0.4f, 1.0f};
    float light_ambient1[]={0.2f, 0.2f, 0.2f, 1.0f};
    float light_ambient2[]={0.8f, 0.8f, 0.8f, 1.0f};

    float light_position0[]={3.0f,1.0f,1.0f,0.0f};
    float light_position1[]={-1.0f,-3.0f,-1.0f,0.0f};
    float light_position2[]={0.0f,30.0f,0.0f,0.0f};

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
    glLightfv(GL_LIGHT2, GL_AMBIENT,  light_ambient2);

/* define material proerties for front face of all polygons */

    //set_color(3);

    glEnable(GL_LIGHTING); /* enable lighting */
	light_switch[0] = 0;   /* Disable light 0 */
	light_switch[1] = 0;   /* Disable light 1 */
	light_switch[2] = 1;   /* Enable  light 2 */
    updateLights();


    glEnable(GL_DEPTH_TEST); /* enable z buffer */
	//glDisable(GL_LIGHT_MODEL_TWO_SIDE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
}


/* turn on/off the lights */
void updateLights()
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

