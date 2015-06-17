#ifndef POLYGON_H_2002_10_20
#define POLYGON_H_2002_10_20

#include "type.h"
#include "Object.h"



//////////////////////////////////////////////////////////////////////
//
//  class  Vertex
//
//  A vertex with a normal vector associated with it
//  
struct Vertex {
  double  p[DIM];    // co-ordinates 
  double  n[DIM];    // normal vector
//  int     N;         // index of this node

  int     valence;  // number of facets that include this vertex 
                    //  (different to number of edges! when this vertex is on boundary)
  int     *F_nbr;     // index of the neighboring facets
  int     *F_ind;     // index of this vertex inside the neighboring facets
  int     array_length;  // length of the above arrays
  int     on_boundary;
  int     valid; // is the vertex in the mesh

 
  // == constructor ======================
	void init() {
  	  valence = 0;
	  F_nbr = F_ind =0;
	}

	Vertex() {init();};
	~Vertex() {
		if(F_nbr) free(F_nbr);
		if(F_ind) free(F_ind);
	}


  // == accessors =========================
  int get_valence() {return valence;};

  // position
  void set_p(double x, double y, double z) {
      p[0] = x; p[1]=y; p[2]=z; 
	  if (DIM==4) p[3] =1;
  }
  double* get_p() {return p;}

  // normal
  void set_n(double x, double y, double z) {
      n[0] = x; n[1]=y; n[2]=z;
  }
  double* get_n() { return n;};

  int find_f(int fc)
  {
	  int i;
	  for(i = 0; i<valence; i++)
	  {
		  if (F_nbr[i] == fc) return i;
	  }
	  return -1;
  }
};

typedef Vertex *p_vertex;


/////////////////////////////////////////////////////////////////
//  
//  class Polygon 
//      a polygonal patch
struct PolygonMesh :public Object{

	Vertex * vertices;
	int    VNum;     // Total vertex number

	int evaluated;

	void free_mem() {
		if(vertices) delete [] vertices;
		vertices = NULL;
	};

	void plot_mesh(float* bg_color);                      // plot the bezier patch
	void plot_patch(bool smooth);          // plot the control polygon
	void plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type);    // plot the highlight lines
	void flip_normal();
}; 



#endif
