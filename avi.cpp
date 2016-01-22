
/**************************************
*                                     *
*     Playing AVI Files In OpenGL     *
*      Jeff Molofee's Lesson 36       *
*       http://nehe.gamedev.net       *
*                2001                 *
*                                     *
**************************************/

#include <windows.h>											// Header File For Windows
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glut.h>												// Header File For The GLu32 Library
#include <vfw.h>												// Header File For Video For Windows
#include "avi.h"
#include <iostream>
#include <atlbase.h>
#include <iostream>
using namespace std;
#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
#pragma comment( lib, "vfw32.lib" )								// Search For VFW32.lib While Linking

#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors


// User Defined Variables
int			nextframe;												// Used For Animation
int			frame = 0;											// Frame Counter
int			effect;												// Current Effect
bool		sp;													// Space Bar Pressed?
bool		env = TRUE;											// Environment Mapping (Default On)
bool		ep;													// 'E' Pressed?
bool		bg = TRUE;											// Background (Default On)
bool		bp;													// 'B' Pressed?

AVISTREAMINFO		psi;										// Pointer To A Structure Containing Stream Info
PAVISTREAM			pavi;										// Handle To An Open Stream
PGETFRAME			pgf;										// Pointer To A GetFrame Object
BITMAPINFOHEADER	bmih;										// Header Information For DrawDibDraw Decoding
long				lastframe;									// Last Frame Of The Stream
int					width;										// Video Width
int					height;										// Video Height
char				*pdata;										// Pointer To Texture Data
int					mpf;										// Will Hold Rough Milliseconds Per Frame

GLUquadricObj *quadratic;										// Storage For Our Quadratic Objects

HBITMAP hBitmap;												// Handle To A Device Dependant Bitmap
HDC hdc = CreateCompatibleDC(0);								// Creates A Compatible Device Context
DWORD				tickCount, lastTickCount = GetTickCount();;										// Used For The Tick Counter

HDRAWDIB hdd;													// Handle For Our Dib
unsigned char* data = 0;										// Pointer To Our Resized Image

void flipIt(void* buffer)										// Flips The Red And Blue Bytes (256x256)
{
	void* b = buffer;											// Pointer To The Buffer
	__asm														// Assembler Code To Follow
	{
		mov ecx, 256 * 256										// Counter Set To Dimensions Of Our Memory Block
			mov ebx, b												// Points ebx To Our Data (b)
		label :													// Label Used For Looping
		mov al, [ebx + 0]										// Loads Value At ebx Into al
			mov ah, [ebx + 2]										// Loads Value At ebx+2 Into ah
			mov[ebx + 2], al										// Stores Value In al At ebx+2
			mov[ebx + 0], ah										// Stores Value In ah At ebx

			add ebx, 3											// Moves Through The Data By 3 Bytes
			dec ecx												// Decreases Our Loop Counter
			jnz label											// If Not Zero Jump Back To Label
	}
}

void OpenAVI(char szFile[])										// Opens An AVI File (szFile)
{
	AVIFileInit();												// Opens The AVIFile Library
	USES_CONVERSION;
	LPCSTR a = szFile;
	LPCWSTR w = A2W(a);
	std::wcout << w << std::endl;
	// Opens The AVI Stream
	if (AVIStreamOpenFromFile(&pavi, w, streamtypeVIDEO, 0, OF_READ, NULL) != 0)
	{
		// An Error Occurred Opening The Stream
		cout << "Failed To Open The AVI Stream" << endl;
		//MessageBox(HWND_DESKTOP, "Failed To Open The AVI Stream", "Error", MB_OK | MB_ICONEXCLAMATION);
	}

	AVIStreamInfo(pavi, &psi, sizeof(psi));						// Reads Information About The Stream Into psi
	width = psi.rcFrame.right - psi.rcFrame.left;					// Width Is Right Side Of Frame Minus Left
	height = psi.rcFrame.bottom - psi.rcFrame.top;					// Height Is Bottom Of Frame Minus Top

	lastframe = AVIStreamLength(pavi);							// The Last Frame Of The Stream

	mpf = AVIStreamSampleToTime(pavi, lastframe) / lastframe;		// Calculate Rough Milliseconds Per Frame
	cout << mpf << endl;
	bmih.biSize = sizeof(BITMAPINFOHEADER);					// Size Of The BitmapInfoHeader
	bmih.biPlanes = 1;											// Bitplanes	
	bmih.biBitCount = 24;										// Bits Format We Want (24 Bit, 3 Bytes)
	bmih.biWidth = 256;											// Width We Want (256 Pixels)
	bmih.biHeight = 256;										// Height We Want (256 Pixels)
	bmih.biCompression = BI_RGB;								// Requested Mode = RGB

	hBitmap = CreateDIBSection(hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&(::data)), NULL, NULL);
	SelectObject(hdc, hBitmap);								// Select hBitmap Into Our Device Context (hdc)

	pgf = AVIStreamGetFrameOpen(pavi, NULL);						// Create The PGETFRAME	Using Our Request Mode
	if (pgf == NULL)
	{
		// An Error Occurred Opening The Frame
		cout << "Failed To Open The AVI Stream" << endl;
		//MessageBox(HWND_DESKTOP, "Failed To Open The AVI Frame", "Error", MB_OK | MB_ICONEXCLAMATION);
	}
}

void GrabAVIFrame(int frame)									// Grabs A Frame From The Stream
{
	LPBITMAPINFOHEADER lpbi;									// Holds The Bitmap Header Information
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);	// Grab Data From The AVI Stream
	pdata = (char *)lpbi + lpbi->biSize + lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

	// Convert Data To Requested Bitmap Format
	DrawDibDraw(hdd, hdc, 0, 0, 256, 256, lpbi, pdata, 0, 0, width, height, 0);
	flipIt((::data));												// Swap The Red And Blue Bytes (GL Compatability)
	// Update The Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, (::data));
}

void CloseAVI(void)												// Properly Closes The Avi File
{
	DeleteObject(hBitmap);										// Delete The Device Dependant Bitmap Object
	DrawDibClose(hdd);											// Closes The DrawDib Device Context
	AVIStreamGetFrameClose(pgf);								// Deallocates The GetFrame Resources
	AVIStreamRelease(pavi);										// Release The Stream
	AVIFileExit();												// Release The File
}

void Deinitialize(void)										// Any User DeInitialization Goes Here
{
	CloseAVI();													// Close The AVI File
}

void Update(DWORD milliseconds)								// Perform Motion Updates Here
{
	nextframe += milliseconds;											// Increase next Based On The Timer
	frame = nextframe / mpf;												// Calculate The Current Frame
	if (frame >= lastframe)										// Are We At Or Past The Last Frame?
	{
		frame = 0;												// Reset The Frame Back To Zero (Start Of Video)
		nextframe = 0;													// Reset The Animation Timer (next)
	}
}

void drawAVI(void)												// Draw Our Scene
{
	tickCount = GetTickCount();				// Get The Tick Count
	Update(tickCount - lastTickCount);	// Update The Counter
	lastTickCount = tickCount;
	GrabAVIFrame(frame);
	glEnable(GL_TEXTURE_2D);
	// Grab A Frame From The AVI
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, (::data));
	glTranslatef(0.0f, 0.0f, -2.0f);							// Translate 10 Units Into The Screen
	glBegin(GL_QUADS);											// Begin Drawing A Cube
	// Front Face
	glNormal3f(0.0f, 0.0f, 0.5f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glNormal3f(0.0f, 0.0f, -0.5f);
	// Top Face
	glNormal3f(0.0f, 0.5f, 0.0f);
	// Bottom Face
	glNormal3f(0.0f, -0.5f, 0.0f);
	// Right Face
	glNormal3f(0.5f, 0.0f, 0.0f);
	// Left Face
	glNormal3f(-0.5f, 0.0f, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	// Done Drawing Our Cube
}

void initAVI()					// Any GL Init Code & User Initialiazation Goes Here
{
	hdd = DrawDibOpen();										// Grab A Device Context For Our Dib
	OpenAVI("Video/CoinFalling.avi");									// Open The AVI File
	OpenAVI("Video/Face3.avi");
}
