/* ------------------------------------------------------------
 *  BezierView
 *    A simple viewer for Bezier patches
 *    SurfLab, Univ Of Florida
 *
 * 
 *		COMPILATION and EXECUTION sequence
 *  make
 *  bview inputfile [-subst step] 
 *
 *  File:     bview.cpp
 *  Purpose:  the main source file
 *            1. parse the command line
 *            2. initial the window
 *            3. read in the data
 *            4. render the scene
 *            5. enter the event loop
 *
 *            Also included in this file:
 *               current modelview matrix saving/loading
 */
#include "stdheaders.h"
#include "type.h"
#include "curvature.h"
#include "util.h"
#include "light.h"
#include "draw.h"
#include "patch.h"
#include "glob.h"
#include "load.h"

// groups           
int  c_grp; // current group
void set_grp(int gid, char* string);  // set a group name

// Read the next patch type into kind and return 1 on success
int  get_kind(FILE* fp, int *kind);
void set_volumn();


char dataFileName[500];
char programDir[500];

int manualSubDepth;



void flip_normal();


void print_usage_and_exist(const char* exe)
{
    printf("usage: %s <inputfile> \n", exe);
	printf("example: %s data/twg2.bv \n", exe);
	exit(-1);
}

/* -------------------------------------------------------------
 *  parse the arguments
 *
 */
void parse_arg(int argc, char* argv[])
{
    int i, j, name_pos;

	// Get the directory where the executable sits
    for(i=(int)strlen(argv[0])-1; i>=0; i--)
		if (argv[0][i]=='\\' || argv[0][i]=='/' )
		{
//			printf("get %c, at %d of %d \n", argv[0][i], i, strlen(argv[0]));
			break;
		}
	
	programDir[i+1] = 0;
    for(j=0;j<=i;j++){
		programDir[j] = argv[0][j];
    }

	if(argc ==1 )               // Print out the usage
        ;//print_usage_and_exist(argv[0]);
	else if(argc == 2)          // bview datafile
	{
		strcpy(dataFileName, argv[1]);  
		manualSubDepth = 0;

	}
	else 
	{
		if(!strcmp(argv[argc-2],"-subst" ))
		{
	        manualSubDepth = atoi(argv[argc-1]);
			name_pos = argc-3;
		}
		else
			name_pos = argc -1;

		if (name_pos!=1 && argv[1][1]!=':') 
		{
			print_usage_and_exist(argv[0]);
		}

		strcpy(dataFileName, argv[1]);
		for(i=2;i<=name_pos;i++)
		{
			strcat(dataFileName, " ");
			strcat(dataFileName, argv[i]);
		}
	}
}




/* ------------------------------------------------------------------------
 * initialize the projection and modelview matrices
 */
void updateProjection()
{
    /* set up viewing */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//printf("%f %f %f %f %f %f \n", ViewCenter[0]-ViewSize,ViewCenter[0]+ViewSize, (ViewCenter[1]-ViewSize),(ViewCenter[1]+ViewSize), -3*ViewDepth*ViewSize, 3*ViewDepth*ViewSize); 
    double aspect = (double)winWidth/(double)(winHeight);
	
    glOrtho(ViewCenter[0]-ViewSize*aspect,ViewCenter[0]+ViewSize*aspect,
		    (ViewCenter[1]-ViewSize),(ViewCenter[1]+ViewSize), 
            //-3*ViewDepth*ViewSize, 3*ViewDepth*ViewSize);
			 3*ViewDepth*ClipNear, 3*ViewDepth*ClipFar);

}
void updateModelView()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}



/* ---------------------------------------------------------------
 *
 *   Read all the patches from the file then 
 *   define the display list for the scene
 *   (patch, mesh, curvature)
 */ 
void define_scene(FILE* fp)
{
    int           patch_kind, done,  pat, i;
    clock_t before, after;
    
    // get current time to evaluate the speed of program
    before = clock();
    printf("reading patch.. \n");

    // Start to reading data
    fseek(fp, 0, SEEK_SET);

    done = ! get_kind(fp, &patch_kind);

	g_substs[0] = 0;

    /* load patch one by one */
    for (pat=1; !done && (pat<MAXFACET); pat++) { /* each patch */
      /* Read the degrees and compute number of points depending on the
         patch_kind */

		// assign the patch type and group

        face[pat].type = patch_kind;
        face[pat].group_id = c_grp;

		// patch_kind, 1: polygon, otherwise smooth object
		if(patch_kind==1) has_polygon = 1;
		else              has_patch   = 1;

		// load the patch data from the file
        Patch *p = &face[pat];
        Patch_init(p);
        for(int i=0;i<3;i++) p->location[i] = 0;

        // read in the
        switch(p->type) {

        case POLY:
        {
            /* Load the polyhedral mesh and turn
             * every facet into a separate patch
             */

            /* Read in number of vertices and faces */
            int VNum, FNum;
            fscanf(fp, "%d %d", &VNum, &FNum);

            vector *V; arrcreate(V,VNum);
            int  *F = NULL;

            /* Read vertex data */
            for (int i=0; i<VNum; i++)
            {
                fscanf(fp, "%lf %lf %lf\n", &V[i][0], &V[i][1], &V[i][2]);
                V[i][3] = 1.0;
            }

            /* Read face data */
            for (int i=0; i<FNum; i++)
            {
                int side;
                fscanf(fp, "%d", &side);
                arrresize(F, side);
                for(int j=0;j<side;j++)
                    fscanf(fp, "%d", &F[j]);

                Patch_init(p+i);
                Patch_createSinglePolygon(p + i, side, V, F);
                p[i].type = patch_kind;
                p[i].group_id = c_grp;
                Patch_enlarge_AABB(&p[i],pat+i==1); // increase the bounding box if necessary
            }

            arrdelete(V);
            arrdelete(F);
            /* We loaded FNum patches so we have
             * to increase pat by FNum-1, since the loop has
             * a pat++ at the top
             */
            pat += FNum - 1;
        }
            break;
        case TP:
        case TP_EQ:
        case RATIONAL:
        case PNTP:
            Patch_loadQuadBezier(p, fp);
            break;
        case TRIANG:
        case PNTRI:
            Patch_loadTriBezier(p, fp);
            break;
        default:
            fprintf(stderr,"Unknown patch_kind %d\n", p->type);
            exit(1);
        }
        Patch_enlarge_AABB(&face[pat],pat==1); // increase the bounding box if necessary

		// get the type of next patch
        done = ! get_kind(fp, & patch_kind);

	}

    if(pat == MAXFACET) 
        printf("Warning: Maximum number of faces exceeded: %d \n", pat -1);
    patch_num = pat-1;

    after = clock();
    printf ("Load complete. Total number of patches: %d, time used: %d milliseconds \n", 
			patch_num, (int) (((double) (after-before))/CLOCKS_PER_SEC*1000));

	set_crv_bound_array(max_crv, min_crv);
 

	for(i=0;i<=group_num;i++)
	{
		g_substs[i] = (manualSubDepth>0)? manualSubDepth : 
			((g_substs[i] >0)? g_substs[i]:
            ((patch_num >50)? 3 : ((patch_num >20)? 4 : 5))) ;
	}

	// assign the subDepth to patches
	for(pat=1;pat<=patch_num;pat++) {
		patch_kind = face[pat].type;
		c_grp      = face[pat].group_id;

		if(patch_kind!=1) {
            Patch_evaluate(&face[pat],g_substs[c_grp]);
		}
	}

	// TO BE COMPATIBLE WITH KESTAS
	{
		char iniFileName[1024];
		FILE* inifile;
		char string[255];
		strcpy(iniFileName, programDir);
		strcat(iniFileName, "BezierView.ini");
		if( (inifile= fopen(iniFileName, "r")) !=NULL ) {
//			printf("reading ini file %s\n", iniFileName);
			while(!feof(inifile)) {
				fscanf(inifile, "%255s", string);
				if(!strcmp(string, "FlipNormal"))
				{
					flip_normal();
//					break;
				}
				if(!strcmp(string, "NoDisplayList"))
				{
					use_display_list = 0;
//					break;
				}
			}
			fclose(inifile);
		}
	}

    // set the view volume
    set_volumn();

    before = clock();

    // print out the time used
    after = clock();
    printf ("\nDone. time used: %d milliseconds \n", 
	(int) (((double) (after-before))/CLOCKS_PER_SEC*1000));


}


/* set the group's initial color */
void set_grp_color(int gid, int color)
{
    g_Material[gid] = color ;


}

/* search for the group named gname */
int get_group(char* gname)
{
	int i;

	for(i=1;i<=group_num;i++) 
		if(!strcmp (group[i].name, gname))
			return i;

    group_num++;
    strcpy(group[group_num].name, gname);
	return group_num;
}


/****
 * Read the next kind into *KIND and return TRUE on success
 ***/
int get_kind(FILE* fp, int *kind)
{
    char string[255];
    int  gcolor; // group color

    if(fscanf(fp, "%s", string) !=1) return 0;

    // if there is a group identification, set that group id to 
    //     be the current group
    while (!strcmp(string,"group") || !strcmp(string, "Group") )
    {
	    int sub;
        if(fscanf(fp, "%d", &gcolor) !=1) return 0;
        if(fscanf(fp, "%s", string) !=1) return 0;

        c_grp = get_group(string);

        // if group_id exceeds the limit
        if(gcolor > (COLORNUM-2) ){
            gcolor = gcolor % (COLORNUM-2);  // don't abort, reuse the old group
        }

        set_grp_color(c_grp, gcolor); // set the group color

	    fgets(string, 255, fp);
	    if(sscanf(string, " #%d", &sub) == 1)
		    g_substs[c_grp] = sub;
		else
			g_substs[c_grp] = 0;

       if(fscanf(fp, "%s", string) !=1) return 0;
    }

    // otherwise, convert the string into a number identifying the patch type
    *kind = atoi(string);
    return 1;
}


/* -------------------------------------------------------------- 
 * set the view volume so that the object is inside and center 
 * at the view volume
 */
void set_volumn()
{
	FILE* fp;
    double sizey, sizez;

    // the center of the object
    ObjectCenter[0] = (ViewLeft+ViewRight)/2;
    ObjectCenter[1] = (ViewBottom+ViewTop)/2;
    ObjectCenter[2] = (ViewNear+ViewFar)/2;

    // compute the maximum difference in three directions
	fp = fopen("ViewSize.in", "r");
	
	if(fp!=NULL)   // Manual size
	{
        if( fscanf(fp, "%lf\n", &ViewSize)==1 && ViewSize>0)
            printf("Manual Viewing Size: %lf\n", ViewSize);
		fclose(fp);
		return; 
	}
	
	//otherwise, use the object size
	ViewSize = (ViewRight - ViewLeft);
	sizey = ViewTop - ViewBottom;
	sizez = ViewNear - ViewFar;
	if( ViewSize < sizey) ViewSize = sizey;
	if( ViewSize < sizey) ViewSize = sizey;
	ClipNear = - ViewSize;
	ClipFar  = ViewSize;
}


/* Print out control keys */
void printkeys()
{
    printf(" How to use BezierView? \n");
    printf(" -- Right button to pop up menu: \n");
    printf(" -- Double click left button to select a patch: \n");
    printf(" -- or use control keys: \n");
    printf("   Esc      : quit\n");
    printf("   'Z', 'z' : zoom out and zoom in\n");
    printf("   'm'      : switch mesh on/off\n");
    printf("   'p'      : switch patch on/off\n");
    printf("   'C'      : change patch color\n");
    printf("   'c'      : add curvature plot\n\n");
}


/* Save Modelview Matrix */
void save_position(int n)
{
    FILE* posfile;
    char  filename[20];
    double matrix[16];
    int i;

    sprintf(filename, ".possav%d", n);
    if( (posfile = fopen(filename, "w")) == NULL)
    {
        printf("can\'t save position %d \n", n);
        return;
    }

    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
//	glScaled(scale_factor, scale_factor, scale_factor);
    glGetDoublev( GL_MODELVIEW_MATRIX, matrix);
    for ( i = 0; i<16;i++)
       fprintf (posfile, "%f\n", matrix[i]);
    fprintf (posfile, "%f %f\n", ViewCenter[0], ViewCenter[1]);
	fprintf(posfile, "%f\n", scale_factor);
    fclose(posfile);
	glPopMatrix();
    printf("position %d saved\n", n);
}

/* Load Modelview Matrix */
void load_position(int n)
{
    FILE* posfile;
    char  filename[20];
    double matrix[16];
    int i;

    sprintf(filename, ".possav%d", n);
    if( (posfile = fopen(filename, "r")) == NULL)
    {
        printf("can't load position %d \n", n);
        return;
    }

    for ( i = 0; i<16;i++)
       fscanf (posfile, "%lf\n", &matrix[i]);

    fscanf (posfile, "%lf %lf ", &ViewCenter[0], &ViewCenter[1]);
    fscanf (posfile, "%lf", &scale_factor);
    updateProjection();
    fclose(posfile);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixd(matrix);
    printf("position %d loaded\n", n);
}


/* read in manual curvature bounds */
void readin_curv_bounds()
{
	char string[255];
	int ret;
	FILE* crvfile;

    if((crvfile = fopen ("IN.crvBounds", "r")) !=0)
    {
 
		while(!feof(crvfile)) {
			if( (ret=fscanf(crvfile, "%s", string))!=1) 
				return;
			if( !strcmp(string, "min:")){
                ret = fscanf(crvfile, "%lf", &manual_low);
				manual_curvature_low =1;
			}
			else if( !strcmp(string, "max:")){
                ret = fscanf(crvfile, "%lf", &manual_high);
				manual_curvature_high =1;
			}
			// added in June 31th 2003, for Kestas's request on
			// looking at a*mean^2+b*gaussian 
			else if( !strcmp(string, "ratio:")){
                ret = fscanf(crvfile, "%lf %lf", &curvature_ratio_a, &curvature_ratio_b) -1;
				set_special_curvature(curvature_ratio_a, curvature_ratio_b);
				special_curv = 1;
			}

			if(ret!=1) {
				printf("File IN.crvBounds has incorret format.\n");
				fclose(crvfile);
				return;
			}
		}
	    fclose(crvfile);
	}
}



void loadDataFile(const char* fn){
    char buffer[2048];
    /* read in all objects and define objects */
    FILE  *fp = fopen(fn,"r");
    if(fp == 0){
        snprintf(buffer, 2048, "Loading the BezierView file '%s'", fn);
        log_error(buffer, "File might be nonexistent or unreadable." );
        return;
    }

    define_scene(fp);
    fclose(fp);

    read_clipping("IN.Clipping");
    init_flags();
    updateProjection();
    updateModelView();

}

void initGL(){
    light_init(); /* initialize the lights */
    glShadeModel(GL_SMOOTH); /* enable smooth shading */
    glEnable(GL_NORMALIZE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


}

void init_bezierview(int argc, char* argv[]){
    /* parse arguments */

    parse_arg(argc, argv);



}
