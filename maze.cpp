#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <math.h>
#include <string>
#include <iostream>
#include <time.h>
#include "maze.h"
#include "generateMaze.h"
#include "screenShot.h"
#include "addTexture.h"
#include "drawElement.h"
#include "obj.h"
#include "glm.h"
#include "avi.h"
#include "particle.h"
#include "mouse.h"

using namespace std;

vector<vector<int>> maze_matrix;
GLuint texture[3];
GLuint tv, lamp, coin;
GLdouble eye_pos[3] = { 1.5*BLOCK_SIZE, 8, 0, };
GLdouble center[3] = { 1.5*BLOCK_SIZE, 8, -2 * BLOCK_SIZE };//c=e+ 旋转后的d
GLdouble angle = -PI / 2;  //方向向量与x轴的夹角
//长度为2*BLOCK_SIZE	
GLfloat view_width, view_height;
int score = 0;
int view_chance = 3;
bool view_first = true;
bool raining = true;
bool avi = false;
bool timeOut = false;
bool ToTimeOut = false;

string screenShotPath = "ScreenShot/";
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", &tstruct);
	return buf;
}

void updateView(int width, int height);
void print_tip(){
	static char buffer[256];
	int totalTime = 600;//total time is 2s.
	int time = glutGet(GLUT_ELAPSED_TIME);
	float timeRes = totalTime - time / 1000.0;
	if (timeRes <= 120)
	{
		ToTimeOut = true;
		updateView(view_width, view_height);
	}
	if (timeRes <= 0){
		timeOut = true;
		timeRes = 0;
	}
	sprintf_s(buffer, "<Amazing Maze> Time left:%.2f  Score:%d   Chances to look at map:%d", timeRes, score, view_chance);
	glutSetWindowTitle(buffer);
	char *c;
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵
	glOrtho(0, 480, 0, 480, -1, 1);// 位置正投影
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵*/
	glRasterPos2f(10, 10);
	for (c = buffer; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPopMatrix();// 重置为原保存矩阵
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPopMatrix();// 重置为原保存矩阵
	glEnable(GL_DEPTH_TEST);


}
void redraw()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);//开启深度测试之前必须调用这个函数。
	
	if (!timeOut){

		glPushMatrix();
		drawMaze(maze_matrix, 21);
		glPopMatrix();

		glPushMatrix();
		drawText();
		glPopMatrix();

		glPushMatrix();
		drawCoins(GL_RENDER);
		glPopMatrix();

		if (raining) {
			drawParticles();
		}

	}
	if (avi || timeOut){
		glPushMatrix();
		drawTv();
		glPopMatrix();
	}
	print_tip();
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
		glOrtho(0, 210, 0, 210, 0, 60);//  俯视
	glMatrixMode(GL_MODELVIEW);

	//设置光照          
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_pos[4] = { 60, 60, -40.0, 1 };
	GLfloat light_color[4] = { 1, 1, 1, 1 };
	if (ToTimeOut) {
		light_color[0] = 0.02;
		light_color[1] = 0.02;
		light_color[2] = 0.02;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);

}


void reshape(int width, int height)
{
	view_height = height;
	view_width = width;
	updateView(width, height);

}

bool collision_detection(GLdouble new_x, GLdouble new_z){
	GLdouble tmp_x, tmp_z;
	for (int p = -1; p < 2; p++)
		for (int q = -1; q < 2; q++)
		{
			tmp_x = new_x + 3 * p;
			tmp_z = new_z + 3 * q;
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

	case 'v'://切换第一人称视图和俯视视图
		if (view_first)
		{
			if (view_chance > 0){
				view_first = false;
				view_chance--;
				break;
			}
		}
		else{
			view_first = !view_first;
		}
		break;

	case '0':
		screenShotPath = screenShotPath + currentDateTime();
		screenShotPath += ".bmp";
		newScreenShot(700, 700, screenShotPath.c_str());
		break;

	case 'r':
		raining = !raining;
		break;

	case 'p':avi=!avi;
	}

	updateView(view_width, view_height);
}


void idle()
{
	glutPostRedisplay();
}

void init()
{
	getNewMaze(10, 10, maze_matrix);

	initText();
	loadOBJ();
	getTexture(texture);
	initCoin();
	initAVI();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(900, 700);
	int windowHandle = glutCreateWindow("Maze-Game");
	init();
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(key);
	glutTimerFunc(20, myTimer, 0);
	glutMouseFunc(mouse);
	glutMainLoop();

	return 0;
}


