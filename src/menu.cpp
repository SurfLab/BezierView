/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    menu.c
 *  Purpose: Menu creating and handling;
 *           keyboard event handling.
 * -------------------------------------------------------------
 */
#include <stdheaders.h>
#include <string.h>
#include <stdlib.h>
#include "glob.h"
#include "menu.h"
#include "draw.h"

void    load_position(int n);
void    save_position(int n);
void    project_init();
void    modelview_init();

void    define_crv();
void	set_lights();
void	export_eps();
void	export_igs();
void	flip_normal();
void    re_evaluate(int substeps);

// in SGOL library
//extern  int  highlight_type;

extern  char   mat_name[][20];
extern  int    clip_item;



/* 
 * menu handling function for color changing
 */
void color_proc(int entry)
{
	int color = entry ;
	if(g_current_grp==0 && (group_num>0) ) {
		for(int i=0; i<=group_num; i++)
			g_Material[i] = color;
	}
	else
		g_Material[g_current_grp] = color;

}

void group_proc(int entry)
{
	g_current_grp = entry;
}

//  each of the individual menu/keyboard handlers
//  ---------------------------------------------------------------
//  Zoom Menu
void zoomin() {
//    glMatrixMode(GL_MODELVIEW);
//    glScalef(1.5, 1.5, 1.5);
	scale_factor *= 1.5;
    ViewDepth *= 1.5;
    project_init(); 
}
void zoomout() {
    //glMatrixMode(GL_MODELVIEW);
    //glScalef(0.67, 0.67, 0.67);
	scale_factor *= 0.67;
    ViewDepth *= 0.67;
    project_init(); 
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
    case ZOOMIN:
		zoomin();
        break;
    case ZOOMOUT:
		zoomout();
        break;
    case ZOOM:
    case ROTATE:
    case MOVE:
		g_mouseMode = entry;
        clip_item = 0;
        break;
	case SMOOTHSHD:
		ToggleMode(g_current_grp, SMOOTH);
		break;
    case MESH:
		ToggleMode(g_current_grp, DRAWMESH);
        break;
    case PATCH:
		ToggleMode(g_current_grp, DRAWPATCH);
        break;
    case POLYMESH:
		ToggleMode(g_current_grp, DRAWPOLYMESH);
        break;
    case POLYPATCH:
		ToggleMode(g_current_grp, DRAWPOLYPATCH);
        break;
//    case SELMESH:
//      drawselmesh = ! drawselmesh;
//       break;
//    case SELPATCH:
//        drawselpatch = ! drawselpatch;
//        break;
    case CURVA:
		ToggleMode(g_current_grp, DRAWCRV);
//		DisableMode(g_current_grp, DRAWPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWPOLYPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWHIGHLIGHT); // disable the highlight display
		DisableMode(g_current_grp, DRAWREFLLINE); // disable the highlight display
		DisableMode(g_current_grp, ENVMAPPING);
		define_crv();
        break;
    case CURVANEEDLE:
		ToggleMode(g_current_grp, DRAWCRVNEEDLE);
		define_crv();
		break;
    case HIGHLIGHT:
		ToggleMode(g_current_grp, DRAWHIGHLIGHT);
//		DisableMode(g_current_grp, DRAWPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWPOLYPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWCRV); // disable the highlight display
		DisableMode(g_current_grp, DRAWREFLLINE); // disable the highlight display
		DisableMode(g_current_grp, ENVMAPPING);
        break;
    case REFLINE:
		ToggleMode(g_current_grp, DRAWREFLLINE);
//		DisableMode(g_current_grp, DRAWPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWPOLYPATCH); // disable the patch display
		DisableMode(g_current_grp, DRAWCRV); // disable the highlight display
		DisableMode(g_current_grp, DRAWHIGHLIGHT); // disable the highlight display
		DisableMode(g_current_grp, ENVMAPPING);
        break;
    case CRVSTYLE0:
        crv_style = 0;
		define_crv();
        break;
    case CRVSTYLE1:
        crv_style = 1;
		define_crv();
        break;
    case CRVSTYLE2:
        crv_style = 2;
		define_crv();
        break;
    case INHLDENSE:
        increase_highlight();
		break;
    case DEHLDENSE:
        decrease_highlight();
		break;
    case GAUSS_CRV:
    case MEAN_CRV:
    case MIN_CRV:
    case MAX_CRV:
    case SPECIAL_CRV:
		crv_choice  =entry ;
		define_crv();
	break;
    case SAVE0:
    case SAVE1:
    case SAVE2:
    case SAVE3:
    case SAVE4:
        save_position(entry-SAVE0);
        break;
    case LOAD0:
    case LOAD1:
    case LOAD2:
    case LOAD3:
    case LOAD4:
        load_position(entry-LOAD0);
        break;
    case RESETP:
		scale_factor = 1.0;
		ViewCenter[0] = ViewCenter[1] = 0;
        modelview_init();
		project_init();
        break;
    case CLIPRIGHT:
    case CLIPLEFT:
    case CLIPNEAR:
    case CLIPFAR:
//        printf("Use the left button to clip; choose \"stop clipping\" to stop.\n");
        clip_item = entry;
		g_mouseMode = ROTATE;
        break;
    case CLIPSTOP:
		ClipNear = - ViewSize;
		ClipFar  = ViewSize;
		project_init();
        break;

    case MENU_LIGHT0:
    case MENU_LIGHT1:
    case MENU_LIGHT2:
		light_switch[entry-MENU_LIGHT0] = !light_switch[entry-MENU_LIGHT0];
        set_lights();
        break;

	case BLACKBACK:
	case BLACKBACK+1:
	case BLACKBACK+2:
	case BLACKBACK+3:
	case BLACKBACK+4:
	case BLACKBACK+5:
	case BLACKBACK+6:
	case BLACKBACK+7:
	case BLACKBACK+8:
	case BLACKBACK+9:
	case BLACKBACK+10:
		back_choice = (entry-BLACKBACK);
		// check if need to redine the display list
		for (i=1; i<=patch_num ; i++)
		{
			Patch* p = &(face[i]);
			int patch_kind = p->type;
			int grp_id = p->group_id;
			int patch_on = (isEnabled(grp_id, DRAWPOLYPATCH) && (patch_kind == POLY) )  ||
						   (isEnabled(grp_id, DRAWPATCH)     && (patch_kind != POLY) );
			int mesh_on  = (isEnabled(grp_id, DRAWPOLYMESH) && (patch_kind == POLY) ) ||
						   (isEnabled(grp_id, DRAWMESH)     && (patch_kind != POLY) );

			if(isEnabled(grp_id, HIDDENLINE) && mesh_on && (!patch_on) )
				g_redisplay = 1;

		}

		break;
	case BLACKPEN:
	case BLACKPEN+1:
	case BLACKPEN+2:
	case BLACKPEN+3:
	case BLACKPEN+4:
	case BLACKPEN+5:
	case BLACKPEN+6:
	case BLACKPEN+7:
	case BLACKPEN+8:
	case BLACKPEN+9:
	case BLACKPEN+10:
		g_PenColor[g_current_grp] = (entry-BLACKPEN);
		if(g_current_grp ==0) 
		for(int i=1; i<= group_num; i++) {
			g_PenColor[i]  = (entry-BLACKPEN);
		}
		g_redisplay = 1;
		break;
	case FLIPNORMAL:
		flip_normal();
		g_redisplay = 1;
		break;
    case ENVMAP:
		ToggleMode(g_current_grp, ENVMAPPING);
		DisableMode(g_current_grp, DRAWHIGHLIGHT); // disable the highlight display
		DisableMode(g_current_grp, DRAWREFLLINE); // disable the highlight display
		break;
	case DRAWBOX:
		drawbox = !drawbox;
		g_redisplay = 1;
		break;
	case SUBST1:
	case SUBST2:
	case SUBST3:
	case SUBST4:
	case SUBST5:
	case SUBST6:
		substeps = entry-SUBST1 +1;
//		printf("new subdivision steps: %d\n", substeps);
		re_evaluate(substeps);
		g_redisplay = 1;
		break;
	case ANTIALIAS:
		g_AntiAlias = ! g_AntiAlias;
		g_redisplay = 1;
		break;
	case HDNLINERMV:
		ToggleMode(g_current_grp, HIDDENLINE);
		break;
	case LINEWIDTH1:
	case LINEWIDTH2:
	case LINEWIDTH3:
	case LINEWIDTH4:
	case LINEWIDTH5:
		for(i=0;i<=group_num;i++)
			g_LineWidth[i] = entry - LINEWIDTH1 +1;
		g_redisplay = 1;
		break;
	case USELISTS:
		use_display_list = !use_display_list;
		g_redisplay =1;
		break;
    case QUIT:
        exit(0);
    default:
        return;
    }

//	logMessage(" ... Command processed\n");
}


/*
 * keyboard controls
 */
void keyboard(unsigned char key, int x, int y)
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
        ToggleMode(g_current_grp, SMOOTH);
        break;
    case 'm':
        ToggleMode(g_current_grp, DRAWMESH);
        break;
    case 'p':
        ToggleMode(g_current_grp, DRAWPATCH);
        break;
    case 'M':
        ToggleMode(g_current_grp, DRAWPOLYMESH);
        break;
    case 'P':
        ToggleMode(g_current_grp, DRAWPOLYPATCH);
        break;
    case 'c':
        ToggleMode(g_current_grp, DRAWCRV);
//		DisableMode(g_current_grp, DRAWPATCH); // disable the patch display
//		DisableMode(g_current_grp, DRAWPOLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWHIGHLIGHT); // disable the highlight display
        DisableMode(g_current_grp, DRAWREFLLINE); // disable the highlight display
        DisableMode(g_current_grp, ENVMAPPING);
        define_crv();
        break;
    case 'n':
        ToggleMode(g_current_grp, DRAWCRVNEEDLE);
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
        ToggleMode(g_current_grp, DRAWHIGHLIGHT);
//		DisableMode(g_current_grp, DRAWPATCH); // disable the patch display
//		DisableMode(g_current_grp, DRAWPOLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWCRV); // disable the highlight display
        DisableMode(g_current_grp, DRAWREFLLINE); // disable the highlight display
        DisableMode(g_current_grp, ENVMAPPING);
        break;
    case 'r':
        ToggleMode(g_current_grp, DRAWREFLLINE);
//		DisableMode(g_current_grp, DRAWPATCH); // disable the patch display
//		DisableMode(g_current_grp, DRAWPOLYPATCH); // disable the patch display
        DisableMode(g_current_grp, DRAWCRV); // disable the highlight display
        DisableMode(g_current_grp, DRAWHIGHLIGHT); // disable the highlight display
        DisableMode(g_current_grp, ENVMAPPING);
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
        export_eps();
        return;
    case 'i':
        export_igs();
        return;
    case 'e':
        ToggleMode(g_current_grp, ENVMAPPING);
        break;
    case 'b':
        drawbox = !drawbox;
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


void advkeyboard(int key, int x, int y)
{
    switch(key) {
    case 101: // up
        ViewCenter[1] -= 0.1*ViewSize;
        break;
    case 100: // left
        ViewCenter[0] += 0.1*ViewSize;
        break;
    case 103:  // down
        ViewCenter[1] += 0.1*ViewSize;
        break;
    case 102:  // right
        ViewCenter[0] -= 0.1*ViewSize;
        break;
    default:
        return;
    }
    project_init();
}
