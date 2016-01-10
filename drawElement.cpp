#include "drawElement.h"
#include <GL/glut.h>
#include "maze.h"
extern GLuint *texture;
extern const float BLOCK_SIZE;
extern const float BLOCK_HEIGHT;


void Draw_Cube(float size)
{
	float r = size / 2;
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(-r, -r, -r);
	glTexCoord2i(0, 1); glVertex3f(-r, -r, r);
	glTexCoord2i(1, 1); glVertex3f(r, -r, r);
	glTexCoord2i(1, 0); glVertex3f(r, -r, -r);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(r, -r, r);
	glTexCoord2i(0, 1); glVertex3f(r, r, r);
	glTexCoord2i(1, 1); glVertex3f(r, r, -r);
	glTexCoord2i(1, 0); glVertex3f(r, -r, -r);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(-r, r, -r);
	glTexCoord2i(0, 1); glVertex3f(-r, r, r);
	glTexCoord2i(1, 1); glVertex3f(r, r, r);
	glTexCoord2i(1, 0); glVertex3f(r, r, -r);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(-r, -r, r);
	glTexCoord2i(0, 1); glVertex3f(-r, r, r);
	glTexCoord2i(1, 1); glVertex3f(-r, r, -r);
	glTexCoord2i(1, 0); glVertex3f(-r, -r, -r);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(-r, -r, r);
	glTexCoord2i(0, 1); glVertex3f(-r, r, r);
	glTexCoord2i(1, 1); glVertex3f(r, r, r);
	glTexCoord2i(1, 0); glVertex3f(r, -r, r);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(-r, -r, -r);
	glTexCoord2i(0, 1); glVertex3f(-r, r, -r);
	glTexCoord2i(1, 1); glVertex3f(r, r, -r);
	glTexCoord2i(1, 0); glVertex3f(r, -r, -r);
	glEnd();
}

void Draw_Maze(vector<vector<int>>& matrix, int matrix_size)
{
	GLint wall_color[4] = { 1, 1, 1, 1 };
	glMaterialiv(GL_FRONT_AND_BACK, GL_AMBIENT, wall_color);

	//glColor3f(1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture[0]);
	for (int i = 0; i < matrix_size; i++)
		for (int j = 0; j < matrix_size; j++) {
			if (matrix[i][j] == 0) continue;
			glPushMatrix();
			glTranslatef(i*10.0, 0, -j*10.0);
			glTranslatef(5.0, 10.0, -5.0);
			glScalef(BLOCK_SIZE, BLOCK_HEIGHT, BLOCK_SIZE);
			Draw_Cube(1.0);
			glPopMatrix();
		}
	glDisable(GL_TEXTURE_2D);

	//画天花板
	for (int i = -1; i < matrix_size; i++)
		for (int j = -1; j < matrix_size; j++) {
			glPushMatrix();
			glTranslatef(i*10.0, 0, -j*10.0);
			glTranslatef(5.0, BLOCK_HEIGHT+0.1, -5.0);
			glScalef(BLOCK_SIZE,1.0, BLOCK_SIZE);
			Draw_Cube(1.0);
			glPopMatrix();
		}

	//画地板
	for (int i = -1; i < matrix_size; i++)
		for (int j = -1; j < matrix_size; j++) {
			glPushMatrix();
			glTranslatef(i*10.0, 0, -j*10.0);
			glTranslatef(5.0, -1.1, -5.0);
			glScalef(BLOCK_SIZE, 1.0, BLOCK_SIZE);
			Draw_Cube(1.0);
			glPopMatrix();
		}

}

void Draw_Coins() {

}

void Draw_Map() {

}
