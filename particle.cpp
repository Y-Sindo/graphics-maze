#include "particle.h"
#include <cmath>
#include <GL/glut.h>
#include <cstdlib>
struct particle
{
    float t;       // 粒子的寿命
    float v;       // 粒子的运动速度
    float d;       // 粒子的运动方向
    float x, y, z; // 粒子的位置坐标
    float xd, yd, zd;  // X Y Z 方向上的增量
    char type;     // 粒子的状态（移动或淡化）
    float a;       // 粒子淡化的 alpha 值
    struct particle *next, *prev;
};
struct particle *fn = nullptr;   //water 链表结构
int timep1 = 17;//微秒
int timep = 17;
void AddParticles()
{
    struct particle *tempp;
    int i;
    
    for (i = 0; i<6; i++)
    {
        tempp = (struct particle *)malloc(sizeof(struct particle));
        if (fn) fn->prev = tempp;
        tempp->next = fn;
        fn = tempp;
        
        tempp->t = 0;
        tempp->v = (float)(rand() % 200) / 100 + 1;     //速度
        tempp->d = (float)(rand() % 628) / 100 - 3.14;     // 方向角度
        // 开始运动的坐标
        tempp->x = 0; 
        tempp->y = 0;
        tempp->z = 0; 
        if (i == 2 || i == 4 || i == 0)
            tempp->xd = -cos(tempp->d)*tempp->v / 3;//与下雨方向相关
        else
            tempp->xd = cos(tempp->d)*tempp->v / 1.5;
        
        tempp->zd = sin((tempp->d*3.14159) / 180)*tempp->v * 20;
        
        tempp->type = 0;
        tempp->a = 1;
    }
}

void MoveParticles()
{
    struct particle *tempp;
    {
        tempp = fn;
        while (tempp)
        {
            if (tempp->type == 0) // 如果粒子是运动的
            {
                tempp->x += tempp->xd;
                
                tempp->z += tempp->zd;
                tempp->t += 0.1; // 寿命递增
                tempp->y = -(10 * (tempp->t*tempp->t / 4)) / 2 + 125; // 计算高度
                if (tempp->y < -5) tempp->type = 1; // 如果粒子落到地上，则粒子淡化
                // 房子避雨
                if (tempp->x > -15 && tempp->x < 15 && tempp->y < 25) {
                    tempp->type = 1;
                }
            }
            else // 粒子落到地上
            {
                tempp->a -= 0.5;
            }
            tempp = tempp->next; //进行下一个粒子的计算
        }
    }
}

void DeleteParticles()
{
    struct particle *tempp, *temp1;
    
    {
        tempp = fn;
        while (tempp)
        {
            if ((tempp->type == 1) && (tempp->a <= 0)) // 粒子死亡
            {
                // 删除粒子
                temp1 = tempp->prev;
                tempp->prev->next = tempp->next;
                if (tempp->next) tempp->next->prev = temp1;
                free(tempp);
                tempp = temp1;
            }
            tempp = tempp->next;
        }
    }
}


void timf(int value)//定时器
{
    glutPostRedisplay();//指定窗口重新绘制
    glutTimerFunc(timep, timf, 0);//注册按一定时间间隔触发的定时器回调函数
}

void material(GLfloat ambR, GLfloat ambG, GLfloat ambB, GLfloat difR, GLfloat difG, GLfloat difB,
              GLfloat speR, GLfloat speG, GLfloat speB)//指定用于光照计算的当前材质属性
{
    GLfloat mat_ambient[] = { ambR, ambG, ambB, 1.0 };
    GLfloat mat_diffuse[] = { difR, difG, difB, 1.0 };
    GLfloat mat_specular[] = { speR, speG, speB, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);// 材质的环境颜色
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);//材质的散射颜色
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);//材质的镜面反射颜色
}

void drawParticles()
{
    AddParticles();//粒子位置生成
    MoveParticles();//粒子移动
    DeleteParticles();//粒子删除

	glPushMatrix();
    {
        struct particle *tempp;
		float mat_ambient[] = { 0.5, 0.2, 0.0, 1.0 };
		float mat_diffuse[] = { 0, 0.8, 0.8, 1.0 };
		float mat_specular[] = { 0, 0, 0, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);// 材质的环境颜色
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);//材质的散射颜色
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);//材质的镜面反射颜色

		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 50);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//特殊的像素算法 把渲染的图像融合到目标区域 源的每一个像素的alpha都等于自己的alpha，目标的每一个像素的alpha等于1减去该位置源像素的alpha。 因此不论叠加多少次，亮度是不变的。
        
        glEnable(GL_BLEND);
        glBegin(GL_QUADS);
        tempp = fn;
        while (tempp)
        {
			glColor3f(0, 1, 1);
            glVertex3f((tempp->x-0.2), (tempp->y-0.5), tempp->z-0.2);
            glVertex3f((tempp->x+0.2), (tempp->y-0.5), (tempp->z-0.2));
            glVertex3f((tempp->x+0.2), (tempp->y+0.5), (tempp->z-0.2));
            glVertex3f((tempp->x-0.2), (tempp->y+0.5), (tempp->z+0.2));
			glColor3f(1,1,1);
            tempp = tempp->next; // 下一个粒子
        }
        glEnd();
        glPopMatrix();
    }
    glDisable(GL_BLEND);
}
