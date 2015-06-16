#ifndef POLYGON_H_2002_10_20
#define POLYGON_H_2002_10_20

#include "type.h"
#include "util.h"
#include "Object.h"



//////////////////////////////////////////////////////////////////////
//
//  class  Vertex
//
//  A vertex with a normal vector associated with it
//  
class Vertex {
private:
  double  p[DIM];    // co-ordinates 
  double  n[DIM];    // normal vector
//  int     N;         // index of this node

public:
  int     valence;  // number of facets that include this vertex 
                    //  (different to number of edges! when this vertex is on boundary)
  int     *F_nbr;     // index of the neighboring facets
  int     *F_ind;     // index of this vertex inside the neighboring facets
  int     array_length;  // length of the above arrays
  int     on_boundary;
  int     valid; // is the vertex in the mesh

 
public:
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

/////////////////////////////////////////////////////////////////////
//
// class Facet
// 
class Facet {
  public:
    int sides;      // number of vertices/edges in this face

    int *V_ind; // the index of the vertices in this facet
    double  normal[3];    // normal vector

  public:
	REAL *crease_edge;

    // == Memeory allocater ========================================
	void initMem() {
		V_ind = alloc_mem_int(sides);

		crease_edge = alloc_mem_db(sides);
		for(int i=0;i<sides;i++)
			crease_edge[i] = 0.0;
	}


    // == Constructors ========================================
	Facet() {V_ind = NULL;crease_edge=NULL;}
    Facet(int s, int* V) {
        sides = s;
		initMem();
        for(int i = 0; i<sides; i++)
           V_ind[i] = V[i];
    }
    Facet(int v1, int v2, int v3)  {
        sides = 3; 
		initMem();
        V_ind[0] = v1; V_ind[1] = v2; V_ind[2] = v3;
    }
    Facet(int v1, int v2, int v3, int v4)  {
        sides = 4;
		initMem();
        V_ind[0] = v1; V_ind[1] = v2; 
        V_ind[2] = v3; V_ind[3] = v4;
    }

    // == Initializors ========================================
	// 
    void set_f(int s, int* V) {
        sides = s;
		initMem();
        for(int i = 0; i<sides; i++)
            V_ind[i] = V[i];
    }
    void set_f(int v1, int v2, int v3)  {
        sides = 3; 
		initMem();
        V_ind[0] = v1; V_ind[1] = v2; V_ind[2] = v3;
    }
	void set_f(int v1, int v2, int v3,int v4)  {
        sides = 4; 
		initMem();
        V_ind[0] = v1; V_ind[1] = v2; V_ind[2] = v3; V_ind[3] = v4;
    }
    // == Destructors ========================================
	virtual ~Facet()
	{
		if(V_ind) free(V_ind);
		if(crease_edge) free(crease_edge);
	}

    // == Accessors ===========================================
    int get_v_ind(int i)   const { return V_ind[i]; };
    int operator [](int i) const { return get_v_ind(i); }
    int get_n()            const { return sides;};

    void set_normal(REAL* nm) { normal[0] = nm[0]; normal[1] = nm[1]; normal[2] = nm[2];};
    REAL* get_normal() { return normal;};
};

typedef Facet *p_face;



/////////////////////////////////////////////////////////////////
//  
//  class Polygon 
//      a polygonal patch
class PolygonMesh :public Object{

protected:
	Vertex * vertices;
public: // temp for saving pov fix later
	Facet  * faces;
public:
	int    VNum;     // Total vertex number
	int    FNum;     // Total facet number

	// additional vertex information
	int invalid_num; // number of vertices that are not in the mesh (isolated vertices)
	int Bnd_Vertex_Num;   // number of vertices on the boundary

	// additional facet information
	int  tri_fnum;   // number of triangle faces 
	int quad_fnum;   // number of quad faces

//	bool has_crease;  // if this polygon has crease edges

	int evaluated;
protected:
	void auto_normal();
	void get_gauss_curvature();//by Jianhua fan
	void get_mean_curvature();

	void auto_neighboring();
	//void writeout_patch();

public:
    // == Constructors ========================================
	PolygonMesh() {		evaluated = false;
		vertices = NULL; faces = NULL;}

	// == Destructors =========================================

	// == Loading  ==============================================
	int loadFileBV(FILE* fp);    // in BezierView polygon format
	int loadFilePOV(FILE* fp);    // in pov-ray mesh2 format

	// == Un-loading ==========================================
	void free_mem() {
		if(vertices) delete [] vertices;
		if(faces) delete [] faces;
		vertices = NULL;
		faces    = NULL;
	};

	// == Accessors ========================================
	Vertex* get_vertex(int i) { return &vertices[i];}
	void    getFaceNeighbor(int f, int ind, int* nbr, int* nbr_ind);
	int     getVertexNeighbor(int v, int i);

    // == Plot routines ========================================
	void plot_mesh(float* bg_color);                      // plot the bezier patch
	void plot_patch(bool smooth);          // plot the control polygon
	void plot_crv(int crv_choice);					   // plot the curvature
	void plot_crv_needles(int crv_choice, REAL length);   // plot the curvature of the quadrilateral patch
	void plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type);    // plot the highlight lines

	// == Misc Routines ========================================
	void flip_normal();
	void compute_crv(); // compute the curvature
}; 



#endif
