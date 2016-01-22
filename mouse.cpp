#include "mouse.h"
#include "drawElement.h"
#include <GL/glut.h>
extern bool view_first;
extern GLfloat view_width;
extern GLfloat view_height;
extern GLdouble eye_pos[3];
extern GLdouble center[3];
extern int coinAll[201];
extern int score;
void processHits(GLint hits, GLuint buffer[])
{
	printf("一共有%d条点击记录\n",hits);
	unsigned int i;
	GLuint minZ, names, *ptr, *ptrNames=nullptr;
	ptr = (GLuint *)buffer;
	minZ = 0xffffffff;
	for (i = 0; i<hits; i++)    //处理每一个点击记录
	{
		names = *ptr;      //获得名字堆栈中的名称数量
		ptr ++;
		printf("min z:%g\n",(float)*ptr/0x7FFFFFFF);
		printf("name :%d\n",*(ptr+2));
		if (*ptr < minZ) {
			minZ = *ptr;
			ptrNames = ptr+2;
			
		}
		ptr += names+2;
	}
	coinAll[*ptrNames] = 0;
	score+=50;
	printf("\n");	
}

void mouse(int button, int state, int x, int y)
{
	GLuint selectBuf[BUFSIZE];				//存储点击记录
	GLint hits;                             //点击记录的个数
	GLint viewport[4];						//视口

	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		return;
	
	glSelectBuffer(BUFSIZE, selectBuf);		//指定存储点击记录的数组
	glRenderMode(GL_SELECT);				//进入选择模式
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glGetIntegerv(GL_VIEWPORT, viewport);  //获得视口
	GLfloat aspect = (GLfloat)viewport[2]/(GLfloat)viewport[3];
	//设置挑选矩阵，挑选区域的中心坐标是(x,viewport[3]-y)，大小是(1,1)
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 1, 1, viewport);
	//设置投影矩阵
	//if (view_first)
	//	gluLookAt(eye_pos[0], eye_pos[1], eye_pos[2], center[0], center[1], center[2], 0, 1, 0);//第一人称视角
	//else
	//	gluLookAt(0, 20.1, 0, 0, 0, 0, 0, 0, -1);  //俯视视角
	if (view_first){
		gluPerspective(60.f, (GLfloat)view_width / (GLfloat)view_height, 2.0f, 210.0f);
	    //glFrustum(-3, 3, -3, 3, 3,100);
		} else {
			glOrtho(0,210,0,210,0,60);
	}

	glMatrixMode(GL_MODELVIEW);
	glInitNames();           //初始化名字堆栈并压入初始元素
	glPushName(0);
	//在选择模式下绘制场景
	drawCoins(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);		//
	glFlush();        //绘制结束

	//处理点击记录
	hits = glRenderMode(GL_RENDER); //获取记录下的点击的个数
	if(hits>0)
		processHits(hits, selectBuf);           //处理点击记录selectBuf
	glutPostRedisplay();
}