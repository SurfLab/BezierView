#ifndef PATCH_H_2002_10_20
#define PATCH_H_2002_10_20

#include "Object.h"

#define MAXFACET 6500
#define MAXGROUP 6500

class Patch{

public:
    int type;  // the type of the patch, see type.h

	Object*  object;

    int group_id;    // which group this face belongs to 
 //   int color;       // if the group has random color, record face color here

	REAL position[3];

public:
	Patch() {
		for(int i=0;i<3;i++) position[i] = 0;
	}
	int  loadFile(FILE* fp);
	void enlarge_AABB(int first);
	void translate(REAL* center);
}; 

// Group structure: attach faces to a group 
//                    therefore can specify same color to many faces
typedef struct _Group
{   
    char name[255];  // name of the group
    int color_index; // color of the group
//	int   subDepth;    // subdivision steps of the current group

} Group;

#endif
