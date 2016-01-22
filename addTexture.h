#pragma once
#include <windows.h>
#include <GL/glut.h>

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
void texload(int i,char *filename,GLuint *texture);
void getTexture(GLuint* texture);
