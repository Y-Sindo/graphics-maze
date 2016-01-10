#pragma once
#include<vector>
#include <Windows.h>
using namespace std;

void Draw_Cube(float size);

void Draw_Maze(vector<vector<int>>& matrix, int matrix_size);
    
void Draw_Coins();

void Draw_Map();

float GetTextAspectRatio(HDC hDC, char *str);

void draw_text_3D(HDC hDC, char *str);

void Draw_Text();

void initText();

void myTimer(int value);


