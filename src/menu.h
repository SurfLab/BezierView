#ifndef MENU_H_2002_10_20
#define MENU_H_2002_10_20

// menu controls 

#define ZOOMIN			1001
#define ZOOMOUT			1002
#define QUIT			1003
#define RESETP			1004
#define BACKGND			1005
#define FLIPNORMAL		1006
#define ZOOM			1007
#define ROTATE			1008
#define MOVE			1009
#define DUMMY			1100

#define COLOR0          100
#define COLOR1			101
#define COLOR2			102
#define COLOR3			103
#define COLOR4			104
#define COLOR5			105
#define COLOR6			106
#define COLOR7			107
#define COLOR8			108
#define COLOR9          109
#define COLOR10         110
#define COLOR11         111

#define PATCH			201
#define MESH			202
#define POLYPATCH		203
#define POLYMESH		204
#define SELPATCH		205
#define SELMESH			206
#define CURVA			207
#define CURVANEEDLE		208
#define HIGHLIGHT		209
#define REFLINE			210
#define SMOOTHSHD		211
#define MIDPATCH		212
#define INHLDENSE		220
#define DEHLDENSE		221
#define PATCHINDENSE    230
#define PATCHDEDENSE    231
#define ENVMAP          232  
#define DRAWBOX         234

#define SAVE0			300
#define SAVE1			301
#define SAVE2    		302
#define SAVE3    		303
#define SAVE4    		304

#define LOAD0    		400
#define LOAD1    		401
#define LOAD2    		402
#define LOAD3    		403
#define LOAD4    		404

#define CLIPNEAR 		501
#define CLIPFAR  		502
#define CLIPLEFT 		503
#define CLIPRIGHT 		504
#define CLIPSTOP 		505
#define MENU_LIGHT0		506
#define MENU_LIGHT1		507
#define MENU_LIGHT2		508

#define CRVSTYLE0		601
#define CRVSTYLE1		602
#define CRVSTYLE2		603
#define BLACKBACK		701
#define BLACKPEN		721

#define SUBST1			801
#define SUBST2			802
#define SUBST3			803
#define SUBST4			804
#define SUBST5			805
#define SUBST6			806

#define LINEWIDTH1		821
#define LINEWIDTH2		822
#define LINEWIDTH3		823
#define LINEWIDTH4		824
#define LINEWIDTH5		825

#define ANTIALIAS       831
#define HDNLINERMV      832
#define USELISTS		833


#define ALLGROUPS       3000
#define GROUP1          3001

// menu handling procedures

void    menu_proc(int entry);
void color_proc_rgb(float rgb[]);
void keyboard(unsigned char key);
void advkeyboard(int key);

#endif
