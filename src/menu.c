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


// --------------------------------------------------------
//
// main menu handling function  (except for color changing) 
void menu_proc(int entry)
{
	int i, substeps ;
/*	char logstring[255];
	sprintf(logstring, "Menu selected %i ", entry);
	logMessage(logstring);
*/
    switch(entry)
    {
    case MENUCONTROL_ZOOMIN:
		zoomin();
        break;
    case MENUCONTROL_ZOOMOUT:
		zoomout();
        break;
    case MENUCONTROL_ZOOM:
    case MENUCONTROL_ROTATE:
    case MENUCONTROL_MOVE:
		g_mouseMode = entry;
        clip_item = 0;
        break;
    case MENUCONTROL_SMOOTHSHD:
        ToggleMode(g_current_grp, DRAWFLAGS_SMOOTH);
		break;
    case MENUCONTROL_MESH:
       // qDebug()<<"mesh";
        ToggleMode(g_current_grp, DRAWFLAGS_MESH);
        break;
    case MENUCONTROL_PATCH:
       // qDebug()<<"patch";
		ToggleMode(g_current_grp, DRAWFLAGS_PATCH);
        break;
    case MENUCONTROL_POLYMESH:
       // qDebug()<<"polymesh";

		ToggleMode(g_current_grp, DRAWFLAGS_POLYMESH);
        break;
    case MENUCONTROL_POLYPATCH:
       // qDebug()<<"polypatch";
		ToggleMode(g_current_grp, DRAWFLAGS_POLYPATCH);
        break;
//    case MENUCONTROL_SELMESH:
//      drawselmesh = ! drawselmesh;
//       break;
//    case MENUCONTROL_SELPATCH:
//        drawselpatch = ! drawselpatch;
//        break;
    case MENUCONTROL_CURVA:
		ToggleMode(g_current_grp, DRAWFLAGS_CRV);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
		DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
		define_crv();
        break;
    case MENUCONTROL_NORMAL:
          DisableMode(g_current_grp, DRAWFLAGS_CRV);
          ToggleMode(g_current_grp, DRAWFLAGS_NORMAL);

          DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
          DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
          //DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
          break;

    case MENUCONTROL_CURVANEEDLE:
		ToggleMode(g_current_grp, DRAWFLAGS_CRVNEEDLE);
		define_crv();
		break;
    case MENUCONTROL_HIGHLIGHT:
		ToggleMode(g_current_grp, DRAWFLAGS_HIGHLIGHT);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWFLAGS_CRV); // disable the highlight display
		DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);

        draw();

        break;
    case MENUCONTROL_REFLINE:
		ToggleMode(g_current_grp, DRAWFLAGS_REFLLINE);
//		DisableMode(g_current_grp, DRAWFLAGS_PATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWFLAGS_POLYPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWFLAGS_CRV); // disable the highlight display
		DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
        DisableMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
        break;
    case MENUCONTROL_CRVSTYLE0:
        crv_style = 0;
		define_crv();
        break;
    case MENUCONTROL_CRVSTYLE1:
        crv_style = 1;
		define_crv();
        break;
    case MENUCONTROL_CRVSTYLE2:
        crv_style = 2;
		define_crv();
        break;
    case MENUCONTROL_INHLDENSE:
        increase_highlight();
		break;
    case MENUCONTROL_DEHLDENSE:
        decrease_highlight();
		break;
    case MENUCONTROL_GAUSS_CRV:
    case MENUCONTROL_MEAN_CRV:
    case MENUCONTROL_MIN_CRV:
    case MENUCONTROL_MAX_CRV:
    case MENUCONTROL_SPECIAL_CRV:
		crv_choice  =entry ;
		define_crv();
	break;
    case MENUCONTROL_SAVE0:
    case MENUCONTROL_SAVE1:
    case MENUCONTROL_SAVE2:
    case MENUCONTROL_SAVE3:
    case MENUCONTROL_SAVE4:
        save_position(entry-MENUCONTROL_SAVE0);
        break;
    case MENUCONTROL_LOAD0:
    case MENUCONTROL_LOAD1:
    case MENUCONTROL_LOAD2:
    case MENUCONTROL_LOAD3:
    case MENUCONTROL_LOAD4:
        load_position(entry-MENUCONTROL_LOAD0);
        break;
    case MENUCONTROL_RESETP:
		scale_factor = 1.0;
		ViewCenter[0] = ViewCenter[1] = 0;
        updateModelView();
		updateProjection();
        break;
    case MENUCONTROL_CLIPRIGHT:
    case MENUCONTROL_CLIPLEFT:
    case MENUCONTROL_CLIPNEAR:
    case MENUCONTROL_CLIPFAR:
//        printf("Use the left button to clip; choose \"stop clipping\" to stop.\n");
        clip_item = entry;
        g_mouseMode = MENUCONTROL_ROTATE;
        break;
    case MENUCONTROL_CLIPSTOP:
		ClipNear = - ViewSize;
		ClipFar  = ViewSize;
		updateProjection();
        break;

    case MENUCONTROL_LIGHT0:
    case MENUCONTROL_LIGHT1:
    case MENUCONTROL_LIGHT2:
        light_switch[entry-MENUCONTROL_LIGHT0] = !light_switch[entry-MENUCONTROL_LIGHT0];
        updateLights();
        break;

    case MENUCONTROL_BLACKBACK:
    case MENUCONTROL_BLACKBACK+1:
    case MENUCONTROL_BLACKBACK+2:
    case MENUCONTROL_BLACKBACK+3:
    case MENUCONTROL_BLACKBACK+4:
    case MENUCONTROL_BLACKBACK+5:
    case MENUCONTROL_BLACKBACK+6:
    case MENUCONTROL_BLACKBACK+7:
    case MENUCONTROL_BLACKBACK+8:
    case MENUCONTROL_BLACKBACK+9:
    case MENUCONTROL_BLACKBACK+10:
        back_choice = (entry-MENUCONTROL_BLACKBACK);
		break;
    case MENUCONTROL_BLACKPEN:
    case MENUCONTROL_BLACKPEN+1:
    case MENUCONTROL_BLACKPEN+2:
    case MENUCONTROL_BLACKPEN+3:
    case MENUCONTROL_BLACKPEN+4:
    case MENUCONTROL_BLACKPEN+5:
    case MENUCONTROL_BLACKPEN+6:
    case MENUCONTROL_BLACKPEN+7:
    case MENUCONTROL_BLACKPEN+8:
    case MENUCONTROL_BLACKPEN+9:
    case MENUCONTROL_BLACKPEN+10:
        g_PenColor[g_current_grp] = (entry-MENUCONTROL_BLACKPEN);
		if(g_current_grp ==0) 
		for(int i=1; i<= group_num; i++) {
            g_PenColor[i]  = (entry-MENUCONTROL_BLACKPEN);
		}
		g_redisplay = 1;
		break;
    case MENUCONTROL_FLIPNORMAL:
		flip_normal();
		g_redisplay = 1;
		break;
    case MENUCONTROL_ENVMAP:
        ToggleMode(g_current_grp, DRAWFLAGS_ENVMAPPING);
		DisableMode(g_current_grp, DRAWFLAGS_HIGHLIGHT); // disable the highlight display
		DisableMode(g_current_grp, DRAWFLAGS_REFLLINE); // disable the highlight display
		break;
    case MENUCONTROL_DRAWBOX:
        ToggleMode(0, DRAWFLAGS_BOX);
		g_redisplay = 1;
		break;
    case MENUCONTROL_SUBST1:
    case MENUCONTROL_SUBST2:
    case MENUCONTROL_SUBST3:
    case MENUCONTROL_SUBST4:
    case MENUCONTROL_SUBST5:
    case MENUCONTROL_SUBST6:
        substeps = entry-MENUCONTROL_SUBST1 +1;
//		printf("new subdivision steps: %d\n", substeps);
		re_evaluate(substeps);
		g_redisplay = 1;
		break;
    case MENUCONTROL_ANTIALIAS:
		g_AntiAlias = ! g_AntiAlias;
		g_redisplay = 1;
		break;
    case MENUCONTROL_HDNLINERMV:
        ToggleMode(g_current_grp, DRAWFLAGS_HIDDENLINE);
		break;
    case MENUCONTROL_LINEWIDTH1:
    case MENUCONTROL_LINEWIDTH2:
    case MENUCONTROL_LINEWIDTH3:
    case MENUCONTROL_LINEWIDTH4:
    case MENUCONTROL_LINEWIDTH5:
		for(i=0;i<=group_num;i++)
            g_LineWidth[i] = entry - MENUCONTROL_LINEWIDTH1 +1;
		g_redisplay = 1;
		break;
    case MENUCONTROL_USELISTS:
		use_display_list = !use_display_list;
		g_redisplay =1;
		break;
    case MENUCONTROL_COLOR0:
    case MENUCONTROL_COLOR1:
    case MENUCONTROL_COLOR2:
    case MENUCONTROL_COLOR3:
    case MENUCONTROL_COLOR4:
    case MENUCONTROL_COLOR5:
    case MENUCONTROL_COLOR6:
    case MENUCONTROL_COLOR7:
    case MENUCONTROL_COLOR8:
    case MENUCONTROL_COLOR9:
    case MENUCONTROL_COLOR10:
    case MENUCONTROL_COLOR11:
        color_proc(entry - MENUCONTROL_COLOR0);
       break;
    case MENUCONTROL_QUIT:
        exit(0);
    default:
        if(entry >= MENUCONTROL_ALLGROUPS && entry <= MENUCONTROL_ALLGROUPS + group_num)
            g_current_grp = entry - MENUCONTROL_ALLGROUPS;
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


