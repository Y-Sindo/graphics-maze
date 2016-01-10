#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include "maze.h"
#include "generateMaze.h"
#include "screenShot.h"
#include "addTexture.h"
#include "drawElement.h"

using namespace std;                     

vector<vector<int>> maze_matrix;
GLuint texture[1];

GLdouble eye_pos[3] = { 1.5*BLOCK_SIZE, 8, 0, };
GLdouble center[3] = { 1.5*BLOCK_SIZE, 8, -2 * BLOCK_SIZE };//c=e+ 旋转后的d
GLdouble angle = -PI / 2;  //方向向量与x轴的夹角
						  //长度为2*BLOCK_SIZE	
GLfloat view_width, view_height;

bool view_first=true;
void redraw()
{

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);//开启深度测试之前必须调用这个函数。
	glPushMatrix();								
	Draw_Maze(maze_matrix, 21);
	glPopMatrix();
	glPushMatrix();		
	Draw_Text();
	glPopMatrix();
	glutSwapBuffers();
}

void updateView(int width, int height){
	glClear(GL_DEPTH_BUFFER_BIT);//开启深度测试之前必须调用这个函数。
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_MODELVIEW);
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	GLdouble length = 2 * BLOCK_SIZE;
	center[0] = length * cosf(angle) + eye_pos[0];
	center[2] = length * sinf(angle) + eye_pos[2];
	glLoadIdentity();

	if (view_first)
		gluLookAt(eye_pos[0], eye_pos[1], eye_pos[2], center[0], center[1], center[2], 0, 1, 0);//第一人称视角
	else
		gluLookAt(0, 20.1, 0, 0, 0, 0, 0, 0, -1);  //俯视视角
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (view_first)
		gluPerspective(60.f, (GLfloat)width / (GLfloat)height, 2.0f, 210.0f);//第一人称
	else
		glOrtho(0,210,0,210,0,60);//  俯视
	glMatrixMode(GL_MODELVIEW);



}


void reshape(int width, int height)
{
	view_height = height;
	view_width = width;
	
	updateView(width,height);

	//设置光照          
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_pos[4] = { 60, 60, -40.0, 1 };
	GLfloat light_color[4] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_color);
	
}

bool collision_detection(GLdouble new_x, GLdouble new_z){
	GLdouble tmp_x, tmp_z;
	for (int p = -1; p < 2; p++)
		for (int q = -1; q < 2; q++)
		{
			tmp_x = new_x + 2 * p;
			tmp_z = new_z + 2 * q;
			int i = tmp_x / BLOCK_SIZE;
			int j = -tmp_z / BLOCK_SIZE;
			if (i < 0 || j < 0 || i >= MAZE_SIZE || j >= MAZE_SIZE) return true;//超出地图范围
			if (maze_matrix[i][j] == 1) return true;//代表碰撞
		}
	return false;

}

void key(unsigned char k, int x, int y){
	GLdouble length = 0.5;
	switch (k){
	case 'w':
		eye_pos[0] += cos(angle)*length;
		eye_pos[2] += sin(angle)*length;
		if (collision_detection(eye_pos[0], eye_pos[2]))
		{
			eye_pos[0] -= cos(angle)*length;
			eye_pos[2] -= sin(angle)*length;
		}
		break;
	case 's':
		eye_pos[0] -= cos(angle)*length;
		eye_pos[2] -= sin(angle)*length;
		if (collision_detection(eye_pos[0], eye_pos[2]))
		{
			eye_pos[0] += cos(angle)*length;
			eye_pos[2] += sin(angle)*length;
		}
		break;

	case 'd':
		angle += 3.0 / 180 * PI;
		break;//右转，角度增加10度

	case 'a':
		angle -= 3.0 / 180 * PI;
		break;//左转，角度减小10度

	case 'v':
		view_first = !view_first;
		break;

	case '0'://切换第一人称视图和俯视视图
		newScreenShot(700, 700, "ScreenShots/tmp.bmp");
		break;

	}
	updateView(view_width,view_height);

}





void idle()
{
	glutPostRedisplay();
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE |GLUT_DEPTH);
	glutInitWindowSize(700, 700);      


	//生成迷宫
	// maze有一个入口和一个出口，maze[1][0],maze[2*h-1][2*w]
	getNewMaze(10, 10, maze_matrix);

	for (int i = 0; i < 10 * 2 + 1; i++) {
	for (int j = 0; j < 10 * 2 + 1; j++) {
	cout << maze_matrix[i][j];
	}
	cout << endl;
	}


	int windowHandle
		= glutCreateWindow("Maze-Game");
	initText();
	getTexture(texture);
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);		
	glutIdleFunc(idle);							
	glutKeyboardFunc(key);
	glutTimerFunc(20,myTimer,0);
	glutMainLoop();

	return 0;
}


