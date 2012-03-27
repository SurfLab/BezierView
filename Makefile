# -- makefile for code using openGL
#    if you  encounter the message: "Missing separator..."
#    make sure that all indented lines are indented by using a TAB
#    rather than blanks

SGOL = SGOL/

CC = g++
CFLAGS = -c -I$(SGOL)

LDLIBS = -lglut  -lGL -lGLU -L/usr/X11R6/lib -lXi -lXmu \
	-lXext -lX11 -lm -lpthread -L$(SGOL) -lSGOL

# ---- Files --- 
ALL	=  Patch.o bview.o glob.o rotate.o draw.o pick.o light.o menu.o texture.o

all:
	cd $(SGOL); make;
	make bview;

bview:    $(ALL) $(SGOL)/libSGOL.a
	$(CC) -o bview -g $(ALL) $(LDLIBS)

.cpp.o:
	$(CC) $(CFLAGS) $*.cpp

dep:
	$(CC) $(CFLAGS) -M *.cpp > depend.mak

clean:
	rm -f *.o bview SGOL/*.o
