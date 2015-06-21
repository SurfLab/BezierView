/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    menu.c
 *  Purpose: Menu creating and handling;
 *           keyboard event handling.
 * -------------------------------------------------------------
 */
#include "stdheaders.h"
#include "type.h"
#include "patch.h"
#include "glob.h"
#include "menu.h"
#include "light.h"
#include "draw.h"
#include "export.h"
#include "rotate.h"

void color_proc(int color)
{
    if(g_current_grp==0 && (group_num>0) ) {
        for(int i=0; i<=group_num; i++)
            g_Material[i] = color;
    }
    else
        g_Material[g_current_grp] = color;
    g_redisplay = 1;
}


void color_proc_rgb(color_t c){
    color_proc(-1);
    if(g_current_grp==0 && (group_num>0) ) {
        for(int i=0; i<=group_num; i++){

            g_patchColor[i] = c;
        }
    }
    else
        g_patchColor[g_current_grp]= c;
}


//  each of the individual menu/keyboard handlers
//  ---------------------------------------------------------------
//  Zoom Menu
void zoomin() {
//    glMatrixMode(GL_MODELVIEW);
//    glScalef(1.5, 1.5, 1.5);
	scale_factor *= 1.5;
    ViewDepth *= 1.5;
    updateProjection(); 
}
void zoomout() {
    //glMatrixMode(GL_MODELVIEW);
    //glScalef(0.67, 0.67, 0.67);
    scale_factor *= 0.67f;
    ViewDepth *= 0.67f;
    updateProjection(); 
}


// --------------------------------------------------
//  option Menu
void decrease_highlight()
{
	hl_step *= 2;
	g_redisplay=1;
}
void increase_highlight()
{
	hl_step /= 2;
	g_redisplay=1;
}

void displayProc(int display_flag)
{

    switch(display_flag)
    {
    case DRAWFLAGS_PATCH:
        ToggleMode(g_current_grp, DRAWFLAGS_PATCH);
        break;
    case DRAWFLAGS_MESH:
        ToggleMode(g_current_grp, DRAWFLAGS_MESH);
        break;
    case DRAWFLAGS_POLYPATCH:
        ToggleMode(g_current_grp, DRAWFLAGS_POLYPATCH);
        break;
    case DRAWFLAGS_POLYMESH:
        ToggleMode(g_current_grp, DRAWFLAGS_POLYMESH);
        break;
    case DRAWFLAGS_CRV:
        ToggleMode(g_current_grp, DRAWFLAGS_CRV);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
        define_crv();
        break;
    case DRAWFLAGS_CRVNEEDLE:
        ToggleMode(g_current_grp, DRAWFLAGS_CRVNEEDLE);
        define_crv();
        break;

    case DRAWFLAGS_HIGHLIGHT:
        ToggleMode(g_current_grp, DRAWFLAGS_HIGHLIGHT);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_CRV); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);

        draw();

        break;
    case DRAWFLAGS_REFLLINE:
        ToggleMode(g_current_grp, DRAWFLAGS_REFLLINE);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_CRV); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
        break;
    case DRAWFLAGS_SMOOTH:
        ToggleMode(g_current_grp, DRAWFLAGS_SMOOTH);
        break;
    case DRAWFLAGS_ENVMAPPING:
        ToggleMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
        DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
        break;
    case DRAWFLAGS_BOX:
        ToggleMode(0, DRAWFLAGS_BOX);
        g_redisplay = 1;
        break;
    case DRAWFLAGS_HIDDENLINE:
        ToggleMode(g_current_grp, DRAWFLAGS_HIDDENLINE);
        break;
    case DRAWFLAGS_NORMAL:
          DisableMode(g_current_grp, DRAWFLAGS_CRV);
          ToggleMode(g_current_grp, DRAWFLAGS_NORMAL);

          DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
          DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
          //DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
          break;
    default:
        fprintf(stderr, "Unkown flag %d\n", display_flag);
        break;

    }
}

// --------------------------------------------------------
//
// main menu handling function  (except for color changing) 
void menu_proc(int entry, int parameter)
{
    switch(entry)
    {
    case MENUCONTROL_ZOOMIN:
        zoomin();
        break;
    case MENUCONTROL_ZOOMOUT:
        zoomout();
        break;

    case MENUCONTROL_RESET_PROJECTION:
        scale_factor = 1.0;
        ViewCenter[0] = ViewCenter[1] = 0;
        updateModelView();
        updateProjection();
        break;
    case MENUCONTROL_INHLDENSE:
        increase_highlight();
        break;
    case MENUCONTROL_DEHLDENSE:
        decrease_highlight();
        break;
    case MENUCONTROL_CLIPSTOP:
        ClipNear = - ViewSize;
        ClipFar  = ViewSize;
        updateProjection();
        break;


    case MENUCONTROL_FLIPNORMAL:
        flip_normal();
        break;


    case MENUCONTROL_QUIT:
        exit(0);
    case MENUCONTROL_ANTIALIAS:
        g_AntiAlias = ! g_AntiAlias;
        break;
    case MENUCONTROL_USELISTS:
        use_display_list = !use_display_list;
        break;
    case MENUCONTROL_GROUP:
        if(parameter <= group_num) g_current_grp = parameter;
        break;
    case MENUCONTROL_COLOR:
        if(parameter < COLORNUM) color_proc(parameter);
    case MENUCONTROL_LINEWIDTH:
        for(int i=0;i<=group_num;i++)
            g_LineWidth[i] = parameter;
        break;
    case MENUCONTROL_SUBST:
        re_evaluate(parameter);
        break;
    case MENUCONTROL_BLACKPEN:
        g_PenColor[g_current_grp] = parameter;
        if(g_current_grp ==0)
        for(int i=1; i<= group_num; i++) {
            g_PenColor[i]  = parameter;
        }
        break;
    case MENUCONTROL_BLACKBACK:
        back_choice = parameter;
        break;
    case MENUCONTROL_LIGHT:
        light_switch[parameter] = !light_switch[parameter];
        updateLights();
        break;
    case MENUCONTROL_CURVATURE_TYPE:
        crv_choice  = parameter ;
        define_crv();
    break;
    case MENUCONTROL_SAVE:
        save_position(parameter);
        break;
    case MENUCONTROL_LOAD:
        load_position(parameter);
        break;
    case MENUCONTROL_CRVSTYLE:
        crv_style = parameter;
        define_crv();
        break;
    case MENUCONTROL_DISPLAY:
        displayProc(parameter);
        break;
    case MENUCONTROL_MOUSEMODE:
        g_mouseMode = parameter;
        break;
    default:
        break;
    }
}


/*
 * keyboard controls
 */
void keyboard(unsigned char key)
{
    static int blend =0;
/*	char logstring[255];
    sprintf(logstring, "Key striked: %c", key);
    logMessage(logstring);
*/
    switch(key)
    {
    case 27:         // ESC or 'q' to quit
    case 'q':
        exit(0);
    case 'z':
        zoomin();
        break;
    case 'Z':
        zoomout();
        break;
    case 'd':
        ToggleMode(g_current_grp, DRAWFLAGS_SMOOTH);
        break;
    case 'm':
        ToggleMode(g_current_grp, DRAWFLAGS_MESH);
        break;
    case 'p':
        ToggleMode(g_current_grp, DRAWFLAGS_PATCH);
        break;
    case 'M':
        ToggleMode(g_current_grp, DRAWFLAGS_POLYMESH);
        break;
    case 'P':
        ToggleMode(g_current_grp, DRAWFLAGS_POLYPATCH);
        break;
    case 'c':
        ToggleMode(g_current_grp, DRAWFLAGS_CRV);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
//		DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
        define_crv();
        break;
    case 'n':
        ToggleMode(g_current_grp, DRAWFLAGS_CRVNEEDLE);
        define_crv();
        break;
    case 'B':
        blend = !blend;                     // Toggle blend TRUE / FALSE
        if(blend)                           // Is blend TRUE?
        {
            glEnable(GL_BLEND);             // Turn Blending On
            glDisable(GL_DEPTH_TEST);       // Turn Depth Testing Off
        }
        else                                // Otherwise
        {
            glDisable(GL_BLEND);            // Turn Blending Off
            glEnable(GL_DEPTH_TEST);        // Turn Depth Testing On
        }
        break;
    case 'h':
        ToggleMode(g_current_grp, DRAWFLAGS_HIGHLIGHT);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
//		DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_CRV); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
        break;
    case 'r':
        ToggleMode(g_current_grp, DRAWFLAGS_REFLLINE);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
//		DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWFLAGS_CRV); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
        break;
    case '-':
    case '_':
        decrease_highlight();
        g_redisplay = 1;
        break;
    case '+':
    case '=':
        increase_highlight();
        g_redisplay = 1;
        break;
    case 'f':
        flip_normal();
        break;
    case 'x':
        export_eps(face, patch_num, ObjectCenter, scale_factor);
        return;
    case 'i':
        export_igs(face, patch_num);
        return;
    case 'e':
        ToggleMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
        break;
    case 'b':
        ToggleMode(0, DRAWFLAGS_BOX);
        break;
    case 'l':
        needle_length *= 2;
        g_redisplay = 1;
        break;
    case 'L':
        needle_length /= 2;
        g_redisplay = 1;
        break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        if( cur_clipping_plane != key-'1')
            cur_clipping_plane = key-'1';
        else
            cur_clipping_plane = -1;
        break;

    default:
        return;
    }
    //m_redisplay = 1;
//	logMessage(" ... Command processed\n");
}


