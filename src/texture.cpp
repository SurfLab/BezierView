/* ------------------------------------------------------------
 *  BezierView
 *    SurfLab, Univ Of Florida
 *
 *  File:    texture.cpp
 *  Purpose: load the texture from a bitmap file
 *      
 * ------------------------------------------------------------
 */
#include "stdheaders.h"

int texture_id=-1;
extern char programDir[500];

// ------------------------------------------------------
//
// LoadBitmap(char *)
//
// load a bitmap 
//
// ------------------------------------------------------
int LoadBitmap(char *filename)
{
    FILE * file;
    char temp;
    long i;
	GLubyte byte;
	char* data;

	int width, height;
	short int planes;
	unsigned short int bitcount;

    texture_id++; // The counter of the current texture is increased

    if( (file = fopen(filename, "rb"))==NULL) {
		char systemDataFile[1024];
		strcpy(systemDataFile, programDir);
		strcat(systemDataFile, filename);
	    if ((file = fopen(systemDataFile,"rb")) == NULL) {
			printf("the texture file: %s is not found!\n", filename);
			return (-1); 
		}
    }

    fseek(file, 18, SEEK_CUR);  /* start reading width & height */

    fread(&byte,1,1,file);
    width = byte;
    fread(&byte,1,1,file);
    width+=(byte*256);
    fread(&byte,1,1,file);
    width+=(byte*256*256);
    fread(&byte,1,1,file);
    width+=(byte*256*256*256);

    fread(&byte,1,1,file);
    height=byte;
    fread(&byte,1,1,file);
    height+=(byte*256);
    fread(&byte,1,1,file);
    height+=(byte*256*256);
    fread(&byte,1,1,file);
    height+=(byte*256*256*256);

	//printf("width: %d, size of int: %d\n", width, sizeof(int));

	//printf("height: %d, size of int: %d\n", height, sizeof(int));

    fread(&byte,1,1,file);
    planes=byte;
    fread(&byte,1,1,file);
    planes+=(byte*256);


	//printf("planes: %d, size of short int: %d\n", planes, sizeof( short int));
    if (planes != 1) {
	    printf("Planes from %s is not 1: %u\n", filename, planes);
	    return 0;
    }

    // read the bpp
    fread(&byte,1,1,file);
    bitcount=byte;
    fread(&byte,1,1,file);
    bitcount+=(byte*256);

    if (bitcount!= 24) {
      printf("%s 24 bits color bmp!: %d\n", filename, bitcount);
      return 0;
    }

    fseek(file, 24, SEEK_CUR);

    // read the color pixels
    data = (char *) malloc(width * height * 3);
    if (data == NULL) {
	    printf("Error allocating memory for texture mapping!\n");
	    return 0;
    }

    if ((i = fread(data, width*height*3, 1, file)) != 1) {
	    printf("Error reading pixel data from %s.\n", filename);
	    return 0;
    }

    for (i=0; i<(width*height *3); i+=3) { // switch bgr to rgb
	    temp = data[i];
	    data[i] = data[i+2];
	    data[i+2] = temp;
    }


    fclose(file); // Closes the file stream


    glBindTexture(GL_TEXTURE_2D, texture_id); // Bind the ID texture specified by the 2nd parameter

    // The next commands sets the texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //The minifying function

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // We don't combine the color with the original surface color, use only the texture map.

    // Finally we define the 2d texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // And create 2d mipmaps for the minifying function
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    
	printf("Texture file %s loaded successfully. \n", filename);

    free(data); // Free the memory we used to load the texture

    return (texture_id); // Returns the current texture OpenGL ID
}
