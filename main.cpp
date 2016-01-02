#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include "generateMaze.h"

using namespace std;
#define BLOCK_SIZE (10.0)
#define BLOCK_HEIGHT (20.0)
#define PI (3.1415926)

int**  matrix;
GLdouble eye_pos[3] = { 1.5*BLOCK_SIZE, 8, 0, };
GLdouble center[3] = { 1.5*BLOCK_SIZE, 8, -2 * BLOCK_SIZE };//c=e+ 旋转后的d
GLdouble angle = -PI / 2;  //方向向量与x轴的夹角
						  //长度为2*BLOCK_SIZE	
GLfloat view_width, view_height;

void Draw_Maze( int **  matrix,int matrix_size) 
{
	GLint wall_color[4] = { 1, 1, 1, 1 };
	glMaterialiv(GL_FRONT_AND_BACK, GL_AMBIENT, wall_color);

	glColor3f(1, 0, 0);
	for (int i = 0; i < matrix_size; i++)
		for (int j = 0; j < matrix_size; j++){
			if (matrix[i][j] == 0) continue;
			glPushMatrix();
			glTranslatef(i*10.0, 0, -j*10.0);
			glTranslatef(5.0, 10.0, -5.0);
			glScalef(BLOCK_SIZE, BLOCK_HEIGHT, BLOCK_SIZE);
			glutSolidCube(1.0);
			glPopMatrix();
	}
}

void redraw()
{
	//生成迷宫
	vector<vector<int>> maze;
	getNewMaze(10, 10, maze);
	// maze有一个入口和一个出口，maze[1][0],maze[2*h-1][2*w]
	for (int i = 0; i < 10 * 2 + 1; i++) {
		for (int j = 0; j < 10 * 2 + 1; j++) {
			cout << maze[i][j];
		}
		cout << endl;
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);//开启深度测试之前必须调用这个函数。
	glPushMatrix();								
	Draw_Maze(matrix, 6);
	glPopMatrix();

	glutSwapBuffers();
}

void updateView(int width, int height){
	glClear(GL_DEPTH_BUFFER_BIT);//开启深度测试之前必须调用这个函数。
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
	gluLookAt(eye_pos[0], eye_pos[1], eye_pos[2], center[0], center[1], center[2], 0, 1, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120.f, (GLfloat)width / (GLfloat)height, 4.0f, 40.0f);
	//glOrtho(0,100,0,100,0,40);//???????   俯视
	glMatrixMode(GL_MODELVIEW);



}


void reshape(int width, int height)
{
	view_height = height;
	view_width = width;
	
	updateView(width,height);

	//gluLookAt(0, 60, 0, 0, 0, 0, 0, 0, -1);  //俯视视角



	//设置光照          
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_pos[4] = { 60, 60, -40.0, 1 };
	GLfloat light_color[4] = { 0.5, 0.5, 0.5, 1 };
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
			if (matrix[i][j] == 1) return true;//代表碰撞
		}
	return false;

}


void key(unsigned char k, int x, int y){
	GLdouble length = 1;
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
	case 'd'://右转，角度增加10度
		angle += 10.0 / 180 * PI;
		break;
	case 'a'://左转，角度减小10度
		angle -= 10.0 / 180 * PI;
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(700, 700);      
	matrix= new int*[6];
	matrix[0] = new int[6]{1, 1, 1, 1, 1, 1};
	matrix[1] = new int[6]{0, 0, 1, 0, 0, 1};
	matrix[2] = new int[6]{1, 0, 0, 1, 0, 1};
	matrix[3] = new int[6]{1, 1, 0, 0, 1, 1};
	matrix[4] = new int[6]{1, 1, 1, 0, 0, 1};
	matrix[5] = new int[6]{1, 1, 1, 1, 1, 1};
	int windowHandle
		= glutCreateWindow("Maze-Game");

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);		
	glutIdleFunc(idle);							
	glutKeyboardFunc(key);
	glutMainLoop();

	for (int i = 0; i < 6; i++)
		delete[]matrix[i];
	delete[]matrix;
	return 0;
}


