#include "drawElement.h"
#include "maze.h"
#include <GL/glut.h>
#include <string.h>
#include <windows.h>

extern GLuint *texture;
extern const float BLOCK_SIZE;
extern const float BLOCK_HEIGHT;
GLuint font_list; // Display list for outline font
HDC current_dc;   // Device context
HFONT myfont;     // Font handle
HFONT oldfont;    // Old font handle
float unit_font_descent; // Ratio between font descent and font height
float spin=0.0;
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

// Get text width and height ratio
float GetTextAspectRatio(HDC hDC, char *str)
{
	SIZE s;
	GetTextExtentPoint32(hDC, (wchar_t *)str, strlen(str), &s);
	return (float)s.cx/(float)s.cy;
}

// Draw text string
void draw_text_3D(HDC hDC, char *str)
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

void Draw_Text(){
	float font_height=1.0, font_depth=0.1;
	float aspect;
	static GLfloat mat_color[] = { 69.0/255, 139.0/255, 116.0/255, 1.0 };
	static GLfloat mat_scolor[] = { 69.0/255, 139.0/255, 116.0/255, 1.0 };
	char str[]="start";
	// Draw spinning 3D text
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_color);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_scolor);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

	glPushMatrix();
	aspect=GetTextAspectRatio(current_dc, str);
	glTranslatef(15.0, 8.0, -2.0);
	glRotatef(spin, 0.0, 1, 0.0);
	glScalef(font_height, font_height, font_depth);
	draw_text_3D(current_dc, str);
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
				(wchar_t *)"黑体");			// Font Name

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
