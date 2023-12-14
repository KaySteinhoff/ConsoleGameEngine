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

//Keyboard Inputs
#define BACKSPACE 8
#define TAB 9
#define RETURN 13
#define CAPS_LOCK 20
#define ESCAPE 27
#define PIC_UP 33
#define SPACE 32
#define PIC_DOWN 34
#define END 35
#define POS_1 36
#define ARR_LEFT 37
#define ARR_UP 38
#define ARR_RIGHT 39
#define ARR_DOWN 40
#define INS 45
#define DEL 46
#define KEYBOARD_0 48
#define KEYBOARD_1 49
#define KEYBOARD_2 51
#define KEYBOARD_3 52
#define KEYBOARD_4 53
#define KEYBOARD_5 54
#define KEYBOARD_6 55
#define KEYBOARD_7 56
#define KEYBOARD_8 57
#define KEYBOARD_9 58
#define KEY_A 65
#define KEY_B 66
#define KEY_C 67
#define KEY_D 68
#define KEY_E 69
#define KEY_F 70
#define KEY_G 71
#define KEY_H 72
#define KEY_I 73
#define KEY_J 74
#define KEY_K 75
#define KEY_L 76
#define KEY_M 77
#define KEY_N 78
#define KEY_O 79
#define KEY_P 80
#define KEY_Q 81
#define KEY_R 82
#define KEY_S 83
#define KEY_T 84
#define KEY_U 85
#define KEY_V 86
#define KEY_W 87
#define KEY_X 88
#define KEY_Y 89
#define KEY_Z 90
#define WINDOWS_SYMBOL 91
#define NUMPAD_0 96
#define NUMPAD_1 97
#define NUMPAD_2 98
#define NUMPAD_3 99
#define NUMPAD_4 100
#define NUMPAD_5 101
#define NUMPAD_6 102
#define NUMPAD_7 103
#define NUMPAD_8 104
#define NUMPAD_9 105
#define NUMPAD_MULTIPLY 106
#define NUMPAD_ADD 107
#define NUMPAD_SUBTRACT 109
#define NUMPAD_DIVIDE 111
#define F1 112
#define F2 113
#define F3 114
#define F4 115
#define F5 116
#define F6 117
#define F7 118
#define F8 119
#define F9 120
#define F10 121
#define F11 122
#define F12 123
#define NUMPAD 144
#define LEFT_SHIFT 160
#define RIGHT_SHIFT 161
#define LEFT_CONTROL 162
#define RIGHT_CONTROL 163
#define LEFT_ALT 164
#define RIGHT_ALT 165
#define ISO_D11 186
#define ISO_D12 187
#define ISO_B08 188
#define ISO_B10 189
#define ISO_B09 190
#define ISO_C12 191
#define ISO_C10 192
#define ISO_E00 220
#define ISO_C11 222
#define ISO_B00 226

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