/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    polygon.cpp
 *  Purpose: render polygonal objects
 *  Author   : Xiaobin Wu
 * ------------------------------------------------------------
 */
#include "stdheaders.h"
extern "C" {
#include "util.h"
#include "curvature.h"
#include "highlight.h"
}
#include "Polygon.h"


////////////////////////////////////////////////////////////////
////
//// load the polygon data from a data file
////
////   BezierView polygon format (without the first '1' as polygon type)
int PolygonMesh::loadFileBV(FILE* fp) {

    int i,j, side;
    REAL V[DIM];  // read-in buffers

	#define MAXSIDE 50
    int  F[MAXSIDE];

    /* Read in number of vertices and faces */
    fscanf(fp, "%d %d", &VNum, &FNum);

    vertices = new Vertex[VNum];
    faces    = new Facet[FNum];

    /* Read vertex data */
    for (i=0; i<VNum; i++)
    {
        fscanf(fp, "%lf %lf %lf\n", &V[0], &V[1], &V[2]);
        vertices[i].set_p(V[0], V[1], V[2]);

        enlarge_aabb(V[0], V[1], V[2]);
    }

	/* reset tri/quad facet count */
	tri_fnum = quad_fnum = 0;

    /* Read face data */
    for (i=0; i<FNum; i++)
    {
        fscanf(fp, "%d", &side);
		if(side > MAXSIDE)
		{
			printf("Please increase MAXSIDE [Polygon.cpp] to %d",  side+1);
			exit(1);
		}

		if(side ==3)
			tri_fnum ++;
		if(side ==4)
			quad_fnum ++;

        for(j=0;j<side;j++)
            fscanf(fp, "%d", &F[j]);

        faces[i].set_f(side, F);
//        faces[i].set_v(vertices);  // assign the node pointers
    }

	// assign the neighboring information
	auto_neighboring();

    // compute the normals for the vertices
    auto_normal();

//	has_crease = false;
	return 0;
}

////////////////////////////////////////////////////////////////
////
//// load the polygon data from a data file
////
////   POV-ray mesh2 polygon format (file pointer after the )
int PolygonMesh::loadFilePOV(FILE* fp)    // in pov-ray mesh2 format
{
	// not yet implemented
	return 0;
}

///////////////////////////////////////////////////
// swap the elements in the array from start to end
// -- used for soring the faces
void swap_array(int *array, int start, int end)
{
	int i, temp;
	for(i = 0; i <= (end-start)/2 ; i ++)
	{
		temp = array[start+i];
		array[start+i] = array[end-i];
		array[end-i] = temp;
	}
}

////////////////////////////////////////////////////////////////
////
//// set the neighboring information
void PolygonMesh::auto_neighboring() {

	int i, v, f, val, n; 
	int f1, f2, nxt, prv;
	int nbr, ind;
	  // initial length of a vertex's neighbors
	const int INITSIDE = 40;
	Vertex* vtx;

	for(v=0;v<VNum;v++) {
	    vtx= &vertices[v];
		if(vtx->F_nbr) free(vtx->F_nbr);
		if(vtx->F_ind) free(vtx->F_ind);

		vtx->array_length = INITSIDE;
	    vtx->F_nbr   = alloc_mem_int(INITSIDE);
	    vtx->F_ind   = alloc_mem_int(INITSIDE);
		vtx->valence = 0; 

	}

	// 1. finding all the neighboring facets for every vertex
    for(f=0;f<FNum;f++) {
        n = faces[f].get_n(); // number of vertices on this face
        for(i=0;i<n;i++) {
            v = faces[f].get_v_ind(i); 
			vtx = &vertices[v];

			// record the facet and vertex index in this facet
			vtx->F_nbr[vtx->valence] = f;
			vtx->F_ind[vtx->valence] = i;
			vtx->valence ++; 

			if(vtx->valence >= vtx->array_length) {  
			    vtx->F_nbr = realloc_mem_int(vtx->F_nbr, vtx->array_length, vtx->array_length *2);
			    vtx->F_ind = realloc_mem_int(vtx->F_ind, vtx->array_length, vtx->array_length *2);
				vtx->array_length *= 2 ;
			}
		}
	}

	/////////////////////////////////////////////////////////////
	// 2. sort the neighboring facets for each vertex (arrange them counter-clockwise)
    for(v=0;v<VNum;v++) {
		vtx = &vertices[v];

		val = vtx->valence; // valence of the vertex

		// val=0 : this vertex doesn't appear in the mesh
		if(val == 0) {           
			vtx->valid = 0;
//			invalid_num ++;
		}
		else
			vtx->valid = 1;

		// preset this vertex not on boundary
		vtx->on_boundary = 0;  

		for(f1 = 0; f1 < val; f1 ++)  // f1 = 0...n is vertex v's all neighboring facets
		{
			// vertex v belongs the facet nbr, at idx index
			nbr = vtx->F_nbr[f1];
			ind = vtx->F_ind[f1];

			// get the 'prv' vertex followed by v (counter clock wise)
			//prv = mesh_get_v(&Data->Triangles[nbr], (ind+2)%n );
			n    = faces[nbr].get_n();
			prv  = faces[nbr].get_v_ind( (ind+n-1)%n );

			// find the facet contains prv
			for(f2 = f1+1; f2 < val; f2 ++)
			{
				nbr = vtx->F_nbr[f2];
				ind = vtx->F_ind[f2];
				//nxt = mesh_get_v(&Data->Triangles[nbr], (ind+1)%n );
				n    = faces[nbr].get_n();
				nxt  = faces[nbr].get_v_ind( (ind+1)%n );

				if(prv == nxt) break;
			}

			if(f2 != val) {   // found 
				// swap f1+1 with f2
				vtx->F_nbr[f2] = vtx->F_nbr[f1+1];
				vtx->F_ind[f2] = vtx->F_ind[f1+1];
				vtx->F_nbr[f1+1] = nbr;
				vtx->F_ind[f1+1] = ind;
			}

			else if(f1==val-1) {  // if f1 is the last one, compare with first facet
				nbr = vtx->F_nbr[0];
				ind = vtx->F_ind[0];
				//nxt = mesh_get_v(&Data->Triangles[nbr], (ind+1)%3 );
				n    = faces[nbr].get_n();
				nxt  = faces[nbr].get_v_ind( (ind+1)%n );

				if(prv != nxt) { // on boundary 
					vtx->on_boundary = 1;
				}
			}
			else {
				vtx->on_boundary = 1;
				break;  // this vertex is on boundary
			}
		}


		// if f1!=val, v is a boundary vertex and the sorting is unfinished
		if(f1!=val) 
		{
			int flast = f1;

			// swap the F_nbr & F_ind array in range [0..f1]
			swap_array(vtx->F_nbr, 0, f1);
			swap_array(vtx->F_ind, 0, f1);

			for(f1 = flast; f1 < val; f1 ++)  // sort the rest of the facets
			{
				nbr  = vtx->F_nbr[f1];
				ind  = vtx->F_ind[f1];
				//nxt  = mesh_get_v(&Data->Triangles[nbr], (ind+1)%3);
				n    = faces[nbr].get_n();
				nxt  = faces[nbr].get_v_ind( (ind+1)%n );


				// find the facet contains prv
				for(f2 = f1+1; f2 < val; f2 ++)
				{
					nbr = vtx->F_nbr[f2];
					ind = vtx->F_ind[f2];
					//prv = mesh_get_v(&Data->Triangles[nbr], (ind+2)%3 );
					n  = faces[nbr].get_n();
					prv = faces[nbr].get_v_ind( (ind+2)%n);

					if(prv == nxt) break;
				}

				if(f2 != val) {   // found 
					// swap f1+1 with f2
					vtx->F_nbr[f2] = vtx->F_nbr[f1+1];
					vtx->F_ind[f2] = vtx->F_ind[f1+1];
					vtx->F_nbr[f1+1] = nbr;
					vtx->F_ind[f1+1] = ind;
				}
				else if(f1!=val-1) {  
					printf("Warning: non-manifold mesh. \n");
				}
			}

			// swap the F_nbr & F_ind array in range [0..val-1]
			swap_array(vtx->F_nbr, 0, val-1);
			swap_array(vtx->F_ind, 0, val-1);
		}
	}


	// Get number of vertices on the boundary
	Bnd_Vertex_Num = 0;  
	invalid_num = 0;
	for(i=0;i<VNum;i++) {
		if(vertices[i].on_boundary) 
				Bnd_Vertex_Num ++;
		if(!vertices[i].valid) 
				invalid_num ++;
	}

}

////////////////////////////////////////////////////////////////
//
// automatically generate vertex normals from the polygon data:
// Heuristics: 
//     the normal at a vertex is approximately the average of 
//     the normals of its neighboring faces
void PolygonMesh::auto_normal()
{
    int i,j, pt, prv, nxt;
    REAL V1[DIM], V2[DIM], V3[DIM]; 

    // initialize 
    for(i=0;i<VNum;i++)
        vertices[i].set_n(0, 0, 0);

    // for each face, compute its normal and accumulate on its vertices
    for(j=0;j<FNum;j++) {

		if(faces[j].sides <3) {
			VEC one = {1, 0, 0, 1};
			faces[j].set_normal(one); 
			continue;
		}

        prv = faces[j].get_v_ind(0); 
        nxt = faces[j].get_v_ind(2);
        pt  = faces[j].get_v_ind(1);

        //printf("face %d, n %d, pt:%d %d %d",j,n,prv,pt,nxt);
        VVminus(vertices[prv].get_p(), vertices[pt].get_p(), V1);
        VVminus(vertices[nxt].get_p(), vertices[pt].get_p(), V2);

        VVcross(V2, V1, V3);

        int n = faces[j].get_n(); // number of vertices on this face
        for(i=0;i<n;i++) {
 			pt  = faces[j].get_v_ind(i);
            VVadd(1.0, vertices[pt].get_n(), 1.0, V3, vertices[pt].get_n());
        }

        Normalize(V3); // some suggested that using unnormalized version in vertex
		               // would give a better result.
		faces[j].set_normal(V3); 
    }

    for(i=0;i<VNum;i++) /* Normalize the final normal */
        Normalize(vertices[i].get_n());
}


// compute the curvatures (first Mean, Gauss, then Max and Min)
void PolygonMesh::compute_crv() 
{
	// not implemented
	return;
}    	  


//////////////////////////////////////////////////////////
// flip the normal
void PolygonMesh::flip_normal()
{
	REAL* normal;
	int i;
    // initialize 
    for(i=0;i<VNum;i++) {
		normal = vertices[i].get_n();
        vertices[i].set_n(-normal[0], -normal[1], -normal[2]);
	}
    // initialize 
    for(i=0;i<FNum;i++) {
		normal = faces[i].get_normal();
		normal[0] = -normal[0];
		normal[1] = -normal[1];
		normal[2] = -normal[2];
        //faces[i].set_normal(-normal[0], -normal[1], -normal[2]);
	}
	normal_flipped = !normal_flipped;
}



////////////////////////////////////////////////////////////////
//
//  plot the polygon

void PolygonMesh::plot_patch(bool smooth)
{
    int i, j, pt;

	//
	if(!evaluated) {
		evaluated = true;
		compute_crv();
	}

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);

    //glShadeModel(GL_SMOOTH);
    for(i=0;i<FNum;i++) {
        int n = faces[i].get_n(); // number of vertices on this face
        glBegin(GL_POLYGON);
        for (j=0; j<n; j++) {
			if(!normal_flipped)  // reverse the orientation of the polygon
	            pt =  faces[i].get_v_ind(j);
			else
				pt =  faces[i].get_v_ind(n-1-j);

			if(smooth)
				glNormal3dv(vertices[pt].get_n());
			else
				glNormal3dv(faces[i].get_normal());

			double size = 64.0;
			glTexCoord2f((vertices[pt].get_p())[0]/size,(vertices[pt].get_p())[2]/size);
            glVertex4dv(vertices[pt].get_p());
        }
        glEnd();
	}

	glPopAttrib();
}

////////////////////////////////////////////////////////////////
//
// plot the mesh 
//
void PolygonMesh::plot_mesh(float* bg_color)
{
    int i, j, pt;
	
    //glShadeModel(GL_SMOOTH);
	if(bg_color) {   // if hidden line removal
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT );
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glColor3fv(bg_color);

		for(i=0;i<FNum;i++) {
			int n = faces[i].get_n(); // number of vertices on this face
			glBegin(GL_POLYGON);
			for (j=0; j<n; j++) {
				pt =  faces[i].get_v_ind(j);
				glVertex4dv(vertices[pt].get_p());
			}
			glEnd();
		}
		glDisable(GL_POLYGON_OFFSET_FILL);	
		glPopAttrib();
	}

	for(i=0;i<FNum;i++) {
        int n = faces[i].get_n(); // number of vertices on this face

		for (j=0; j<n; j++) {
			if( (faces[i].crease_edge)[j] ) {
				glPushAttrib(GL_CURRENT_BIT ); // save current pen color
				glColor3f(1.0, 0.0, 0.0);      // use red to draw the crease edge
			}

			glBegin(GL_LINES); 
				pt =  faces[i].get_v_ind(j);
				glVertex4dv(vertices[pt].get_p());
				pt =  faces[i].get_v_ind((j+1)%n);
				glVertex4dv(vertices[pt].get_p());
			glEnd();

			if( (faces[i].crease_edge)[j] ) {
				glPopAttrib();                 // restore previous pen color
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////// 
//
//  plot_crv : plot the curvature for the polygon mesh
//
void PolygonMesh::plot_crv(int crv_choice)
{
	return;
}


void PolygonMesh::plot_crv_needles(int crv_choice, REAL length=1.0)
{
	return;
}


////////////////////////////////////////////////////////////////
//
// plot the highlight lines
//
void PolygonMesh::plot_highlights(VEC A, VEC H, REAL hl_step, int highlight_type)
{
    int i, j, pt;

	REAL P[MAXSIDE*DIM];
	REAL N[MAXSIDE*DIM];

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);
    for(i=0;i<FNum;i++) {
        int n = faces[i].get_n(); // number of vertices on this face
        for (j=0; j<n; j++) {
            pt =  faces[i].get_v_ind(j);
			Vcopy(vertices[pt].get_p(), &P[j*DIM]);
			Vcopy(vertices[pt].get_n(), &N[j*DIM]);
        }
		Highlight(n, P, N, A, H, hl_step, highlight_type);
	}
	glPopAttrib();
}



/////////////////////////////////////////////////////////////////////
// Get vertex v's ith neighbor
int  PolygonMesh::getVertexNeighbor(int v, int i)
{
	Vertex* vtx;
	int val, fc, ind, n;

	vtx = &(vertices[v]);
	val = vtx->valence;

	if(i>=0 && i<val)
	{
	    fc  = vtx->F_nbr[i];
	    ind = vtx->F_ind[i];
		n = faces[fc].get_n();
	    return faces[fc].get_v_ind( (ind+1)%n);  // the neighbor
	}
	else if(i==val && vtx->on_boundary) {
		fc  = vtx->F_nbr[val-1];
		ind = vtx->F_ind[val-1];
		n = faces[fc].get_n();
		return faces[fc].get_v_ind( (ind+n-1)%n);  // the neighbor
	}
	else return -1;
}


void PolygonMesh::getFaceNeighbor(int f, int ind, int* nbr, int* nbr_ind)
{
	/////////////////////////////////////////////////////////////////////
	// Get the neighboring face of f across edge ind: (0,1 .. side-1)

        int f_ind, n;
        Vertex* vtx;
        int v0, v1;//, v2;
        Facet* fp = &(faces[f]);
        int side;

        side = fp->get_n();
        v0 = fp->get_v_ind(ind);
        v1 = fp->get_v_ind((ind+1)%side);
        //v2 = fp->get_v_ind((ind+2)%3);

        vtx = &(vertices[v0]);
        n = vtx->valence;
        f_ind = vtx->find_f(f);

        if(vtx->on_boundary && f_ind==0) {  // can't use v0 to get the neighbor
            vtx = &(vertices[v1]);
            n = vtx->valence;
            f_ind = vtx->find_f(f);

            if(vtx->on_boundary && f_ind == (n-1)) {   // can't use v1 to get the neighbor
                    *nbr = -1; // this facet has no neighbor across edge v0-v1
            }
            else {  // use v1 to get the neighbor
                    *nbr = vtx->F_nbr[ (f_ind+1)%n ];
                    *nbr_ind = vtx->F_ind[ (f_ind+1)%n];
            }
        }
        else {           // use v0 to get the neighbor
                
			*nbr = vtx->F_nbr[ (f_ind+n-1)%n ];
            side = faces[*nbr].get_n();
            //*nbr_ind = vtx->F_ind[ (f_ind+n-1)%n ];
			*nbr_ind = (vtx->F_ind[ (f_ind+n-1)%n ]+ (side-1))%side ;
        }
}




