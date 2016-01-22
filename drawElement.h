#pragma once
#include<vector>
#include <Windows.h>
using namespace std;

void drawCube(float size);

void drawMaze(vector<vector<int>>& matrix, int matrix_size);
    
void drawCoins(GLenum mode);

void drawMap();

float getTextAspectRatio(HDC hDC, char *str);

void drawText3D(HDC hDC, char *str);

void drawText();

void initText();

void myTimer(int value);

void drawTv();

void initCoin();

