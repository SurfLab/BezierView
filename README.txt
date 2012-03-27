============================================================
 BezierView -- a simple openGL viewer for the Bezier patches.
 Version 2.11b

 Author(s)    : Xiaobin Wu, xwu@cise.ufl.edu
                SurfLab, University of Florida
 Last updated : Oct 05th, 2004

 This file explains the usage of the program.

 (see FORMAT.txt for specification of data format)
============================================================

I) Compilation

   o Unix/Linux:
     % make

   o Windows
     open the .dsw in Visual Studio and build.

II) Usage :

   o Unix/Linux:
     bview anyfile.bv

   o Windows
     associate the .bv files with the result BezierView.exe
     and double click the .bv files.

II) supports : 
   - Tensor product Bezier patches 
   - Tensor product rational patches
   - Triangle Bezier patches
   - Polyhedron
   - PN triangles, PN quadrilateral patches

III) features :
   - Gaussian/Mean/Max/Min curvature plot
   - Plane clipping
   - Position save/load
   - Object groups
       different colors can be assigned to different groups
   - Environment mapping
      (Image file: room.bmp)
