#include <GL/glut.h>
#include <string.h>
#include <windows.h>
#include "drawElement.h"
#include "maze.h"
#include "avi.h"

extern GLuint texture[3];
extern GLuint tv, lamp, coin;
extern const float BLOCK_SIZE;
extern const float BLOCK_HEIGHT;

extern int coin_select;
extern vector<vector<int>> maze_matrix;
GLuint font_list; // Display list for outline font
HDC current_dc;   // Device context
HFONT myfont;     // Font handle
HFONT oldfont;    // Old font handle
GLfloat spin=1.0;

int coinAll[201];
int coinX[201];
int coinY[201];

float unit_font_descent; // Ratio between font descent and font height

void drawCube(float size)
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

float blue[] = { 0, 0, 1, 1 };
void drawMaze(vector<vector<int>>& matrix, int matrix_size)
{
	GLint wall_color[4] = { 1, 1, 1, 1 };
	glMaterialiv(GL_FRONT_AND_BACK, GL_AMBIENT, wall_color);

	//绘制墙壁
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	for (int i = 0; i < matrix_size; i++)
		for (int j = 0; j < matrix_size; j++) {
			if (matrix[i][j] == 0) continue;
			glPushMatrix();
			glTranslatef(i*10.0, 0, -j*10.0);
			glTranslatef(BLOCK_SIZE / 2, BLOCK_HEIGHT / 2, -BLOCK_SIZE/2);
			glScalef(BLOCK_SIZE, BLOCK_HEIGHT, BLOCK_SIZE);
			drawCube(1.0);
			glPopMatrix();
		}
	

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	//画天花板
	for (int i = -1; i < matrix_size; i++)
		for (int j = -1; j < matrix_size; j++) {
			glPushMatrix();
			glTranslatef(i*10.0, 0, -j*10.0);
			glTranslatef(5.0, BLOCK_HEIGHT+0.1, -5.0);
			glScalef(BLOCK_SIZE,1.0, BLOCK_SIZE);
			drawCube(1.0);
			glPopMatrix();
		}

	//画地板
	for (int i = -1; i < matrix_size; i++)
		for (int j = -1; j < matrix_size; j++) {
			glPushMatrix();
			glTranslatef(i*10.0, 0, -j*10.0);
			glTranslatef(5.0, -1.1, -5.0);
			glScalef(BLOCK_SIZE, 1.0, BLOCK_SIZE);
			drawCube(1.0);
			glPopMatrix();
		}

}

void initCoin(){
	for (int i = 0; i<201; i++)
	{
		coinAll[i] = 1;	//name for coin
		coinX[i] = 2 + rand() % 6;
		coinY[i] = 2 + rand() % 6;
	}
}

void drawCoins(GLenum mode) {
	/*绘制钱币*/
	int count = 0;
	for (int i = 0; i < MAZE_SIZE; i++)
		for (int j = 0; j < MAZE_SIZE; j++, count++) {
			if (maze_matrix[i][j] == 1) continue;
			if (coinAll[count] == 0) continue;
			if ((count % 4) != 0) continue;
			glLoadName(count);
			glPushMatrix();
			glTranslatef(i*10.0, 0, -j*10.0);
			glTranslatef(coinX[count], 0, -coinX[count]);
			glScalef(0.2, 0.2, 0.2);
			glCallList(coin);
			glPopMatrix();
		}
}
// Get text width and height ratio
float getTextAspectRatio(HDC hDC, char *str)
{
	SIZE s;
	//GetTextExtentPoint32(hDC, (wchar_t *)str, strlen(str), &s);
	GetTextExtentPoint32(hDC, (LPCWSTR) str, strlen(str), &s);
	return (float)s.cx/(float)s.cy;
}

// Draw text string
void drawText3D(HDC hDC, char *str)
{
	int i=0;
	unsigned int ichar;
	while(str[i]!='\0')
	{
		if (IsDBCSLeadByte(str[i])) // Check if the byte is a lead byte
		{
			ichar=(str[i]<<8)+256+str[i+1]; // Chinese character
			i+=2;
		}
		else
		{
			ichar=str[i]; // ASCII character
			++i;
		}

		// Generate outline character
		wglUseFontOutlines (hDC, ichar, 1, font_list,
			0.0, 1.0, WGL_FONT_POLYGONS, NULL);
		glCallList(font_list);
	}
}

void drawText(){
	float font_height = 1.0, font_depth = 0.1;
	float aspect;
	static GLfloat mat_color[] = { 30.0 / 255, 255.0 / 255, 255.0 / 255, 1.0 };
	static GLfloat mat_scolor[] = { 30.0 / 255, 255.0 / 255, 255.0 / 255, 1.0 };
	char str[] = "start";
	// Draw spinning 3D text
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_color);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_scolor);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

	glPushMatrix();
	aspect = getTextAspectRatio(current_dc, str);
	glTranslatef(15.0, 8.0, -2.0);
	glRotatef(spin, 0.0, 1, 0.0);
	glScalef(font_height, font_height, font_depth);
	drawText3D(current_dc, str);
	glPopMatrix();

	// Outline font display list may change front face definition
	// So change it back to counter-clockwise
	glFrontFace(GL_CCW);
}



void initText()
{
	int font_height=100;

	myfont = CreateFont(	-font_height,		// Height Of Font
				0,				// Width Of Font
				0,				// Angle Of Escapement
				0,				// Orientation Angle
				FW_BOLD,			// Font Weight
				FALSE,				// Italic
				FALSE,				// Underline
				FALSE,				// Strikeout
				GB2312_CHARSET,		// Character Set Identifier
				OUT_TT_PRECIS,			// Output Precision
				CLIP_DEFAULT_PRECIS,		// Clipping Precision
				ANTIALIASED_QUALITY,		// Output Quality
				FF_DONTCARE|DEFAULT_PITCH,	// Family And Pitch
				//(wchar_t *)"黑体");			// Font Name
				(LPCWSTR)"黑体");			// Font Name

	// Select font into current device context
	current_dc=GetDC(NULL);
	oldfont=(HFONT)SelectObject(current_dc, myfont);

	// Obtain text metrics
	TEXTMETRIC tm;
	GetTextMetrics(current_dc, &tm);
	unit_font_descent=(float)tm.tmDescent/(float)font_height;

	// Generate font display list
	font_list=glGenLists(1);
}

void myTimer(int value)
{
	spin+=1.0;
	if (spin > 360.0) spin-=360.0;
	glutTimerFunc(20, myTimer, 0);
	glutPostRedisplay();
}

void drawTv()
{
	glPushMatrix();   
	glRotatef(180, 0, 1.0f, 0);
	glTranslatef(0.0f, 0.1f, -4.0f);
	glPushMatrix();	//电视
	glTranslatef(0.2f, 0.4f, 0.0f);
	glScalef(1.0f, 1.0f, 1.0f);
	glCallList(tv);
	glPopMatrix();
	glPushMatrix();								//电视画面
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Set Texture Min Filter
	gluQuadricNormals(gluNewQuadric(), GLU_SMOOTH);						// Create Smooth Normals 
	gluQuadricTexture(gluNewQuadric(), GL_TRUE);						// Create Texture Coords 
	glTranslatef(10.0f, 10.0f, -5.0f);
	glScalef(0.65f, 0.35f, 0.0f);
	drawAVI();
	glPopMatrix();
	glPopMatrix();

}

