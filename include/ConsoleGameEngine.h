#ifdef _WIN64
#error Please change to a Windows environment to use this Engine
#endif

#ifndef UNICODE
#error Please enable UNICODE for your compiler!
#endif

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define FG_BLACK		0x0000
#define FG_DARK_BLUE    0x0001
#define FG_DARK_GREEN   0x0002
#define FG_DARK_CYAN    0x0003
#define FG_DARK_RED     0x0004
#define FG_DARK_MAGENTA 0x0005
#define FG_DARK_YELLOW  0x0006
#define FG_GREY			0x0007
#define FG_DARK_GREY    0x0008
#define FG_BLUE			0x0009
#define FG_GREEN		0x000A
#define FG_CYAN			0x000B
#define FG_RED			0x000C
#define FG_MAGENTA		0x000D
#define FG_YELLOW		0x000E
#define FG_WHITE		0x000F
#define BG_BLACK		0x0000
#define BG_DARK_BLUE	0x0010
#define BG_DARK_GREEN	0x0020
#define BG_DARK_CYAN	0x0030
#define BG_DARK_RED		0x0040
#define BG_DARK_MAGENTA 0x0050
#define BG_DARK_YELLOW	0x0060
#define BG_GREY			0x0070
#define BG_DARK_GREY	0x0080
#define BG_BLUE			0x0090
#define BG_GREEN		0x00A0
#define BG_CYAN			0x00B0
#define BG_RED			0x00C0
#define BG_MAGENTA		0x00D0
#define BG_YELLOW		0x00E0
#define BG_WHITE		0x00F0

#define PIXEL_SOLID 		0x2588
#define PIXEL_THREEQUARTERS 0x2593
#define PIXEL_HALF 			0x2592
#define PIXEL_QUARTER 		0x2591

typedef struct _CONSOLE_FONT_INFOEX
{
    ULONG cbSize;
    DWORD nFont;
    COORD dwFontSize;
    UINT  FontFamily;
    UINT  FontWeight;
    WCHAR FaceName[LF_FACESIZE];
}CONSOLE_FONT_INFOEX, *PCONSOLE_FONT_INFOEX;

BOOL WINAPI SetCurrentConsoleFontEx(HANDLE hConsoleOutput, BOOL bMaximumWindow, PCONSOLE_FONT_INFOEX
lpConsoleCurrentFontEx);

struct CGE_Sprite
{
	int width;
	int height;
	
	short *glyphs;
	short *colors;
};

struct CGE
{
	int screenWidth;
	int screenHeight;
	
	CHAR_INFO *screenBuffer;
	HANDLE hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO originalConsoleInfo;
	
	HANDLE hConsole;
	HANDLE hConsoleIn;
	SMALL_RECT rectWindow;
	
	short keyOldState[256];
	short keyNewState[256];
	struct KeyState
	{
		unsigned char bPressed;
		unsigned char bReleased;
		unsigned char bHeld;
	} keys[256], mouse[5];
	unsigned char mouseOldState[5];
	unsigned char mouseNewState[5];
	
	int mouseX;
	int mouseY;
	
	double elapsed;
	clock_t oldTime;
	
	int numSprites;
	struct CGE_Sprite **sprites;
};

// Functions used by CGE_Sprite

struct CGE_Sprite* cgeCreateSprite(int width, int height);

int cgeSetGlyph(struct CGE_Sprite *sprite, int x, int y, short g);

int cgeSetColor(struct CGE_Sprite *sprite, int x, int y, short c);

short cgeGetGlyph(struct CGE_Sprite *sprite, int x, int y);

short cgeGetColor(struct CGE_Sprite *sprite, int x, int y);

int cgeSaveSprite(struct CGE_Sprite *sprite, const char* path);

struct CGE_Sprite* cgeLoadSprite(const char* path);

struct CGE_Sprite* cgeCutSprite(struct CGE_Sprite *sprite, int x, int y, int w, int h);

//Functions used by CGE

int InitCGE(void);

int cgeCreateConsole(int width, int height, int fontW, int fontH);

void cgeDrawString(int x, int y, char *str, short color);

void cgeDraw(int x, int y, short color, short pixelStatus);

void cgeDrawLine(int x1, int y1, int x2, int y2, short color, short pixelState);

void cgeDrawSprite(int x, int y, struct CGE_Sprite *sprite);

void cgeClearBuffer(void);

void cgePollEvents(void);

double cgeGetElapsedTime(void);

void cgeSwapBuffer(void);

unsigned char GetKeyDown(int i);

unsigned char GetKey(int i);

unsigned char GetKeyUp(int i);

unsigned char GetMouseDown(int i);

unsigned char GetMouse(int i);

unsigned char GetMouseUp(int i);

int cgeMouseX(void);

int cgeMouseY(void);

//TODO: Implement all functionality of CGE by the standard of the olcConsoleGameEngine.h 
// on https://github.com/OneLoneCoder/Javidx9/blob/master/ConsoleGameEngine/olcConsoleGameEngine.h