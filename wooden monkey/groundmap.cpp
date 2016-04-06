#if defined(__APPLE_CC__)
#include<OpenGL/gl.h>
#include<OpenGL/glu.h>
#include<GLUT/glut.h>
#elif defined(WIN32)
#include<windows.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
#else
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include<stdint.h>
#endif

#include <fstream>
#include "groundmap.h"
#define     STEP_SIZE   1

//loadTexture is from stackoverflow and modified some part of it
//http://stackoverflow.com/questions/12518111/how-to-load-a-bmp-on-glut-to-use-it-as-a-texture
GLuint GroundMap::LoadTexture(char * source)
{
    
    GLuint texture;
    FILE * file = fopen(source, "r" );
    
    int width = 2048;
    int height = 2048;
    unsigned char *picture = (unsigned char *)malloc(width * height * 3);
    unsigned char *picture_copy = (unsigned char *)malloc(width * height * 3);
    fread(picture, width * height * 3, 1, file );
    
    int i = 0;
    for(; i < width * height * 3; i ++ ) {
        picture_copy[i] = picture[i];
    }
    
    i = 0;
    for(; i < width * height ; i ++ ) {
        picture[i*3] = picture_copy[i*3 + 2];
        picture[i*3 + 2] = picture_copy[i*3];
        
    }
    
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, picture );
    free(picture);
    free(picture_copy);
    fclose(file);
    return texture;
}

//Render is from github and modified some part of it
void GroundMap::Render(char* source){
    int X = 0;
    int Z;
    TexID = GroundMap::LoadTexture(source);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TexID);
    glBegin( GL_QUADS );

    for (; X < (MAP_SIZE-STEP_SIZE); X += STEP_SIZE )
        for ( Z = 0; Z < (MAP_SIZE-STEP_SIZE); Z += STEP_SIZE )
        {
            glTexCoord2f((float)X / MAP_SIZE, (float)Z / MAP_SIZE);
            glVertex3i(X, 0, Z);
            glTexCoord2f((float)X / MAP_SIZE, (float)(Z + STEP_SIZE) / MAP_SIZE) ;
            glVertex3i(X, 0, Z + STEP_SIZE);
            glTexCoord2f((float)(X + STEP_SIZE) / MAP_SIZE, (float)(Z+ STEP_SIZE) / MAP_SIZE);
            glVertex3i(X+STEP_SIZE, 0, Z+STEP_SIZE);
            glTexCoord2f((float)(X + STEP_SIZE) / MAP_SIZE, (float)Z / MAP_SIZE);
            glVertex3i(X+STEP_SIZE, 0, Z);
        }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
