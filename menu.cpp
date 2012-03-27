/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    menu.c
 *  Purpose: Menu creating and handling;
 *           keyboard event handling.
 * -------------------------------------------------------------
 */
#define FREEGLUT_STATIC 1
#include <GL/glut.h>
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

/* menu creation */

void addCheckedMenu(const char* item, int entry, int checked)
{
    char string[255];
	if(checked) 
		sprintf(string, " v  %s", item);
	else 
		sprintf(string, "     %s", item);
	glutAddMenuEntry(string,  entry);
}

void menu_init()
{
    int i,c;
    static int mainmenu, colormenu, displaymenu, crvmenu, 
          loadmenu, savemenu, clipmenu, crvstylemenu, bckgndmenu, 
		  optionmenu, hldensemenu, evalmenu, grpmenu, pencolormenu,
		  linewidthmenu;
    static int first_run=1;

//	char dummy_string0[] = "______________";
	char dummy_string0[] = "~~~~~~~~~~";
//	char dummy_string1[] = "     __________________";
	char dummy_string1[] = "     ~~~~~~~~~~~~";

	if(!first_run) {   // if not the first time, delete the menus first
		glutDestroyMenu(mainmenu);
		glutDestroyMenu(optionmenu);
		glutDestroyMenu(colormenu);
		glutDestroyMenu(displaymenu);
		glutDestroyMenu(crvmenu);
		glutDestroyMenu(loadmenu);
		glutDestroyMenu(savemenu);
		glutDestroyMenu(clipmenu);
		glutDestroyMenu(crvstylemenu);
		glutDestroyMenu(bckgndmenu);
		glutDestroyMenu(hldensemenu);
		glutDestroyMenu(evalmenu);
		glutDestroyMenu(pencolormenu);
		glutDestroyMenu(linewidthmenu);
 	    if(group_num >0)  glutDestroyMenu(grpmenu);
	}
	else
		first_run=0;


    // create/re-create the menus
    mainmenu     = glutCreateMenu(menu_proc);
    optionmenu   = glutCreateMenu(menu_proc);
    colormenu    = glutCreateMenu(color_proc);
    crvstylemenu = glutCreateMenu(menu_proc);
    displaymenu  = glutCreateMenu(menu_proc);
    crvmenu      = glutCreateMenu(menu_proc);
    loadmenu     = glutCreateMenu(menu_proc);
    savemenu     = glutCreateMenu(menu_proc);
    clipmenu     = glutCreateMenu(menu_proc);
    bckgndmenu   = glutCreateMenu(menu_proc);
    hldensemenu  = glutCreateMenu(menu_proc);
	evalmenu     = glutCreateMenu(menu_proc);
	pencolormenu = glutCreateMenu(menu_proc);
	linewidthmenu = glutCreateMenu(menu_proc);

    if(group_num >0) // if there are more than one group
    {
        // all group
        grpmenu = glutCreateMenu(group_proc);
        glutSetMenu(grpmenu);
	    addCheckedMenu ("All Groups", 0, g_current_grp==0);
        for(i=1;i<=group_num; i++)
		    addCheckedMenu (group[i].name, i, g_current_grp==i );
    }

    glutSetMenu(colormenu);
      for(c=0;c<COLORNUM; c++)
	    addCheckedMenu (mat_name[c], c, g_Material[g_current_grp] ==c);

    glutSetMenu(displaymenu);

	if(has_patch)  {  // if there is a smooth patch
	    addCheckedMenu ("Patch  \t (p)", PATCH, isEnabled(g_current_grp, DRAWPATCH));
	    addCheckedMenu ("Control Mesh \t (m)", MESH, isEnabled(g_current_grp, DRAWMESH));
	}

	if(has_polygon)  {  // if there is a polygon
	    addCheckedMenu ("Polygon Face   \t (P)", POLYPATCH, isEnabled(g_current_grp, DRAWPOLYPATCH));
	    addCheckedMenu ("Polygon Mesh   \t (M)", POLYMESH, isEnabled(g_current_grp, DRAWPOLYMESH));
	}

    glutAddMenuEntry(dummy_string1, DUMMY);

	if(has_patch)  {  // if there is a smooth patch
	    addCheckedMenu ("Curvature   \t (c)", CURVA, 
			isEnabled(g_current_grp, DRAWCRV));
	    addCheckedMenu ("Curvature Needles \t (n)", CURVANEEDLE, 
			isEnabled(g_current_grp, DRAWCRVNEEDLE));
	}

    addCheckedMenu ("Highlight Lines \t (r)", HIGHLIGHT, 
						isEnabled(g_current_grp, DRAWHIGHLIGHT));
    addCheckedMenu ("Reflection Lines \t (r)", REFLINE, 
						isEnabled(g_current_grp, DRAWREFLLINE));
    addCheckedMenu ("Environment Mapping \t (e)", ENVMAP, 
						isEnabled(g_current_grp, ENVMAPPING));
    addCheckedMenu ("Bounding Box \t (b)", DRAWBOX, drawbox);

    glutSetMenu(crvmenu);
      addCheckedMenu ("Gaussian", GAUSS_CRV, crv_choice == GAUSS_CRV);
      addCheckedMenu ("Mean", MEAN_CRV, crv_choice == MEAN_CRV);
      addCheckedMenu ("Max" , MAX_CRV , crv_choice == MAX_CRV );
      addCheckedMenu ("Min" , MIN_CRV , crv_choice == MIN_CRV );

	  if(special_curv){
	    char string[255];
		if(crv_choice == SPECIAL_CRV)
			sprintf(string , " v  (%.1f)*Gauss+(%.1f)*Mean^2", curvature_ratio_a, curvature_ratio_b);
		else
			sprintf(string , "     (%.1f)*Gauss+(%.1f)*Mean^2 ", curvature_ratio_a, curvature_ratio_b);
		glutAddMenuEntry(string,  SPECIAL_CRV);
	  }

    glutSetMenu(crvstylemenu);
      addCheckedMenu ("Color Shade", CRVSTYLE1, crv_style==1);
      addCheckedMenu ("Gray Shade" , CRVSTYLE2, crv_style==2);
      addCheckedMenu ("Curvature lines" , CRVSTYLE0, crv_style==0);

    glutSetMenu(hldensemenu);
      glutAddMenuEntry("Increase density   (+)", INHLDENSE);
      glutAddMenuEntry("Decrease density   (-)", DEHLDENSE);

	glutSetMenu(bckgndmenu);
  	  for(i=0;i<10;i++)
	     addCheckedMenu (g_BackColorNames[i], BLACKBACK+i, i==back_choice);

	glutSetMenu(pencolormenu);
  	  for(i=0;i<10;i++)
	     addCheckedMenu (g_penColorNames[i], BLACKPEN+i, i==g_PenColor[g_current_grp]);

	glutSetMenu(linewidthmenu);
      addCheckedMenu( " 1 ",    LINEWIDTH1, g_LineWidth[g_current_grp]==1);
      addCheckedMenu( " 2 ",    LINEWIDTH2, g_LineWidth[g_current_grp]==2);
      addCheckedMenu( " 3 ",    LINEWIDTH3, g_LineWidth[g_current_grp]==3);
      addCheckedMenu( " 4 ",    LINEWIDTH4, g_LineWidth[g_current_grp]==4);
      addCheckedMenu( " 5 ",    LINEWIDTH5, g_LineWidth[g_current_grp]==5);


    glutSetMenu(optionmenu);  //  Option   ->
 
	  addCheckedMenu ("Smooth Shading  \t (d)", SMOOTHSHD, isEnabled(g_current_grp, SMOOTH));

												//  Flip Normal
      glutAddMenuEntry( "     Flip Normals !  \t (f)",  FLIPNORMAL);


      glutAddMenuEntry(dummy_string1, DUMMY);   // -----------

	  addCheckedMenu("Use display lists", USELISTS, use_display_list);
												//  Background
	  addCheckedMenu ("Light 0 ", MENU_LIGHT0, light_switch[0]);
	  addCheckedMenu ("Light 1 ", MENU_LIGHT1, light_switch[1]);
	  addCheckedMenu ("Light 2 ", MENU_LIGHT2, light_switch[2]);

      glutAddMenuEntry(dummy_string1, DUMMY);   // -----------
	  addCheckedMenu ("Hidden Line Removal ", HDNLINERMV, isEnabled(g_current_grp, HIDDENLINE));
	  addCheckedMenu ("Anti Aliasing ", ANTIALIAS, g_AntiAlias);
	  
      glutAddSubMenu( "     Line Width", linewidthmenu);

      glutAddMenuEntry(dummy_string1, DUMMY);   // -----------
												//  Highlight Line Density ->
      glutAddSubMenu( "     Highlight Line Density", hldensemenu);
												//  Curvature Style        ->
      glutAddSubMenu( "     Curvature style",  crvstylemenu);


    glutSetMenu(savemenu);   //  Save Position   ->
      glutAddMenuEntry("0", SAVE0);             //  0
      glutAddMenuEntry("1", SAVE1);             //  1
      glutAddMenuEntry("2", SAVE2);             //  2
      glutAddMenuEntry("3", SAVE3);             //  3
      glutAddMenuEntry("4", SAVE4);             //  4

    glutSetMenu(loadmenu);   //  Load Position   ->
      glutAddMenuEntry("0", LOAD0);             //  0
      glutAddMenuEntry("1", LOAD1);             //  1
      glutAddMenuEntry("2", LOAD2);             //  2
      glutAddMenuEntry("3", LOAD3);             //  3
      glutAddMenuEntry("4", LOAD4);             //  4

    glutSetMenu(clipmenu);  //   Clip           -> 
	  addCheckedMenu( "Clip Near", CLIPNEAR, clip_item == CLIPNEAR);  // Clip Near
      addCheckedMenu( "Clip Far", CLIPFAR, clip_item == CLIPFAR);  // Clip Near
      addCheckedMenu("Clear Clipping", CLIPSTOP, false); // Stop

	glutSetMenu(evalmenu);
      addCheckedMenu( "2x2 ",      SUBST1, g_substs[g_current_grp]==1);
      addCheckedMenu( "4x4 ",      SUBST2, g_substs[g_current_grp]==2);
      addCheckedMenu( "8x8 ",      SUBST3, g_substs[g_current_grp]==3);
      addCheckedMenu( "16x16 ",    SUBST4, g_substs[g_current_grp]==4);
      addCheckedMenu( "32x32 ",    SUBST5, g_substs[g_current_grp]==5);
      addCheckedMenu( "64x64 ",    SUBST6, g_substs[g_current_grp]==6);


    glutSetMenu(mainmenu);
//    glutAddMenuEntry( "Zoom In  \t(z)", ZOOMIN);
//    glutAddMenuEntry( "Zoom Out \t(Z)", ZOOMOUT);
    addCheckedMenu( "Zoom", ZOOM,   g_mouseMode == ZOOM && clip_item ==0);
    addCheckedMenu( "Rotate", ROTATE, g_mouseMode == ROTATE&& clip_item ==0);
    addCheckedMenu( "Move", MOVE,   g_mouseMode == MOVE&& clip_item ==0);
    glutAddSubMenu( "     Clipping",  clipmenu);
    glutAddMenuEntry(dummy_string0, DUMMY);
	if(group_num >0)  glutAddSubMenu( "Group",  grpmenu);
    glutAddSubMenu( "Display",  displaymenu);
	if(has_patch) glutAddSubMenu( "Patch Detail", evalmenu);
    glutAddSubMenu( "Meterial   ", colormenu);
    glutAddSubMenu( "Line Color",  pencolormenu);
    glutAddSubMenu( "Background",  bckgndmenu);
    glutAddSubMenu( "Advanced Options       ", optionmenu);
    glutAddSubMenu( "Curvature type",  crvmenu);
    glutAddMenuEntry(dummy_string0, DUMMY);
    glutAddSubMenu( "Save position",  savemenu);
    glutAddSubMenu( "Load position",  loadmenu);
    glutAddMenuEntry( "Reset position", RESETP);
    glutAddMenuEntry(dummy_string0, DUMMY);
//    glutAddSubMenu( "Clipping",  clipmenu);
    //glutAddMenuEntry( string,  BACKGND);
    glutAddMenuEntry( "Quit     \t(q)", QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

}


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

	menu_init();
	g_redisplay =1;
    glutPostRedisplay();
}

void group_proc(int entry)
{
	g_current_grp = entry;
	menu_init();
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

	menu_init();   // re-create the menu
//	logMessage(" ... Command processed\n");
    glutPostRedisplay();
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
	menu_init();   // re-create the menu
//	logMessage(" ... Command processed\n");
    glutPostRedisplay();
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
	glutPostRedisplay();
}

