
CC = g++
CFLAGS = -c 

LDLIBS = -lglut  -lGL -lGLU -L/usr/X11R6/lib -lXi -lXmu \
	-lXext -lX11 -lm -lpthread

# ---- Files --- 
output	: SGOL

ALLSGOL	=  TriBezier.o Polygon.o util.o curvature.o QuadBezier.o highlight.o

SGOL:    $(ALLSGOL)
	ld -r -o libSGOL.a $(ALLSGOL) 

.cpp.o:
	$(CC) $(CFLAGS) $*.cpp

dep:
	$(CC) $(CFLAGS) -M *.cpp > depend.mak

clean:
	rm -f *.o
all:
	rm -f *.o; make

