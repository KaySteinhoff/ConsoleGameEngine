#include <ConsoleGameEngine.h>

struct CGE cge;

//CGE_Sprite Functions

struct CGE_Sprite* cgeCreateSprite(int width, int height)
{
	struct CGE_Sprite *sprite = malloc(sizeof(struct CGE_Sprite));
	
	if(width < 0 || height < 0)
		return NULL;
	
	sprite->width = width;
	sprite->height = height;
	sprite->glyphs = malloc(sizeof(short)*width*height);
	sprite->colors = malloc(sizeof(short)*width*height);
	
	if(NULL == sprite->glyphs || NULL == sprite->colors)
		return NULL;
	
	if(0 == cge.numSprites)
	{
		cge.numSprites = 1;
		cge.sprites = malloc(sizeof(struct CGE_Sprite*));
	}
	else
	{
		cge.numSprites++;
		cge.sprites = realloc(cge.sprites, sizeof(struct CGE_Sprite*)*cge.numSprites);
	}
	
	cge.sprites[cge.numSprites-1] = sprite;
	
	return sprite;
}

int cgeSetGlyph(struct CGE_Sprite *sprite, int x, int y, short g)
{
	if(x < 0 || x >= sprite->width || y < 0 || y >= sprite->height)
		return 0;
	
	if(NULL == sprite->glyphs)
		return 0;
	
	sprite->glyphs[y*sprite->width+x] = g;
	return 1;
}

int cgeSetColor(struct CGE_Sprite *sprite, int x, int y, short c)
{
	if(x < 0 || x >= sprite->width || y < 0 || y >= sprite->height)
		return 0;
	
	if(NULL == sprite->colors)
		return 0;
	
	sprite->colors[y*sprite->width+x] = c;
	return 1;
}

short cgeGetGlyph(struct CGE_Sprite *sprite, int x, int y)
{
	if(x < 0 || x >= sprite->width || y < 0 || y >= sprite->height)
		return 0;
	
	if(NULL == sprite->glyphs)
		return 0;
	
	return sprite->glyphs[y*sprite->width+x];
}

short cgeGetColor(struct CGE_Sprite *sprite, int x, int y)
{
	if(x < 0 || x >= sprite->width || y < 0 || y >= sprite->height)
		return 0;
	
	if(NULL == sprite->colors)
		return 0;
	
	return sprite->colors[y*sprite->width+x];
}

int cgeSaveSprite(struct CGE_Sprite *sprite, const char* path)
{
	FILE* fptr = fopen(path, "wb");
	
	if(NULL == fptr)
		return 0;
	
	int result = fwrite(&sprite->width, sizeof(int), 1, fptr);
	result += fwrite(&sprite->height, sizeof(int), 1, fptr);
	result += fwrite(sprite->glyphs, sizeof(short), sprite->width*sprite->height, fptr);
	result += fwrite(sprite->colors, sizeof(short), sprite->width*sprite->height, fptr);
	
	if(result <= 0)
		return 0;
	
	fclose(fptr);
	return 1;
}

struct CGE_Sprite* cgeLoadSprite(const char* path)
{
	//Open file and create sprite struct
	FILE* fptr = fopen(path, "rb");
	
	if(NULL == fptr)
		return NULL;
	
	struct CGE_Sprite *sprite = malloc(sizeof(struct CGE_Sprite));
	
	if(NULL == sprite)
		return NULL;
	
	//Read width and height
	int result = fread(&sprite->width, sizeof(int), 1, fptr);
	
	result += fread(&sprite->height, sizeof(int), 1, fptr);

	//Due to adding the result up if result is less than 2 not all values were read(one of the two integers wasn't read)
	if(result < 2)
		return NULL;
	
	//Precompile the buffer size as it will be used multiple times
	int bufferSize = sprite->width*sprite->height;
	
	sprite->glyphs = malloc(sizeof(short)*bufferSize);
	sprite->colors = malloc(sizeof(short)*bufferSize);
	
	if(NULL == sprite->glyphs || NULL == sprite->colors)
		return NULL;
	
	//result gets reset by assigning a value instead of adding one
	result = fread(sprite->glyphs, sizeof(short), bufferSize, fptr);
	
	result += fread(sprite->colors, sizeof(short), bufferSize, fptr);
	
	//Same logic as the width/height
	if(result < 2)
		return NULL;
	
	fclose(fptr);
	
	if(0 == cge.numSprites)
	{
		cge.numSprites = 1;
		cge.sprites = malloc(sizeof(struct CGE_Sprite*));
	}
	else
	{
		cge.numSprites++;
		cge.sprites = realloc(cge.sprites, sizeof(struct CGE_Sprite*)*cge.numSprites);
	}
	
	cge.sprites[cge.numSprites-1] = sprite;
	
	return sprite;
}

void Clip(struct CGE_Sprite *sprite, int *x, int *y)
{
	if(*x < 0)
		*x = 0;
	else if(*x > sprite->width)
		*x = sprite->width;
	
	if(*y < 0)
		*y = 0;
	else if(*y > sprite->height)
		*y = sprite->height;
}

struct CGE_Sprite* cgeCutSprite(struct CGE_Sprite *sprite, int x, int y, int w, int h)
{	
	if(NULL == sprite)
		return NULL;

	Clip(sprite, &x, &y);
	
	struct CGE_Sprite *newSprite = cgeCreateSprite(w, h);
	
	if(NULL == newSprite)
		return NULL;
	
	for(int oy = y; oy < h; ++oy)
	{
		memcpy(newSprite->glyphs+w*oy, sprite->glyphs+y*sprite->width+x, w);
		memcpy(newSprite->colors+w*oy, sprite->colors+y*sprite->width+x, w);
	}
	
	return newSprite;
}

//CGE Functions

int InitCGE(void)
{
	cge.screenWidth = 80;
	cge.screenHeight = 30;
	
	cge.numSprites = 0;
	
	cge.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	cge.hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
	
	if(INVALID_HANDLE_VALUE == cge.hConsole || INVALID_HANDLE_VALUE == cge.hConsoleIn)
		return 0;
	
	memset(cge.keyNewState, 0, sizeof(short)*256);
	memset(cge.keyOldState, 0, sizeof(short)*256);
	memset(cge.keys, 0, sizeof(struct KeyState)*256);
	cge.mouseX = 0;
	cge.mouseY = 0;
	
	return 1;
}

static BOOL CloseHandler(DWORD evt)
{
	if(evt == CTRL_CLOSE_EVENT)
	{
		for(int i = 0; i < cge.numSprites; ++i)
		{
			free(cge.sprites[i]->colors);
			free(cge.sprites[i]->glyphs);
		}
		free(cge.sprites);
		free(cge.screenBuffer);
	}
	
	return TRUE;
}

int cgeCreateConsole(int width, int height, int fontW, int fontH)
{
	cge.screenWidth = width;
	cge.screenHeight = height;
	
	//Reduce console window size to a minimum to resize screen buffer
	cge.rectWindow = (SMALL_RECT){ 0, 0, 1, 1 };
	SetConsoleWindowInfo(cge.hConsole, TRUE, &cge.rectWindow);
		
	//Resize console screen buffer
	COORD coord = { (short)cge.screenWidth, (short)cge.screenHeight };
	if(!SetConsoleScreenBufferSize(cge.hConsole, coord))
	{
		printf("Failed to set the console buffer size\n");
		return 0;
	}
		
	//Set active console buffer
	if(!SetConsoleActiveScreenBuffer(cge.hConsole))
	{
		printf("Failed to set active screen buffer of the console\n");
		return 0;
	}
		
	//Set console font
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontW;
	cfi.dwFontSize.Y = fontH;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	
	wcscpy(cfi.FaceName, L"Consolas");
	if(!SetCurrentConsoleFontEx(cge.hConsole, FALSE, &cfi))
	{
		printf("Failed to set console font\n");
		return 0;
	}
		
	//Check console buffer info
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(!GetConsoleScreenBufferInfo(cge.hConsole, &csbi))
	{
		printf("Failed to read console buffer info");
		return 0;
	}	
	if(cge.screenWidth > csbi.dwMaximumWindowSize.X)
	{
		printf("Passed screen width / font width too large\n");
		return 0;
	}
	if(cge.screenHeight > csbi.dwMaximumWindowSize.Y)
	{
		printf("Passed screen height / font height too large\n");
		return 0;
	}
	
	//Get physical window console Size
	cge.rectWindow = (SMALL_RECT){ 0, 0, (short)cge.screenWidth-1, (short)cge.screenHeight-1 };
	if(!SetConsoleWindowInfo(cge.hConsole, TRUE, &cge.rectWindow))
	{
		printf("Failed to set window size to passed width/height");
		return 0;
	}
		
	//Set console flags to allow mouse input
	if(!SetConsoleMode(cge.hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
	{
		printf("Failed to set up mouse input");
		return 0;
	}

	//Allocate screen buffer
	cge.screenBuffer = malloc(sizeof(CHAR_INFO)*cge.screenWidth*cge.screenHeight);
	memset(cge.screenBuffer, 0, sizeof(CHAR_INFO)*cge.screenWidth*cge.screenHeight);
		
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
	return 1;
}

void cgeDrawString(int x, int y, char *str, short color)
{
	int i = 0;
	for(char *c = str; *c != '\0'; c++)
	{
		cge.screenBuffer[y * cge.screenWidth + x + i].Char.UnicodeChar = *c;
		cge.screenBuffer[y * cge.screenWidth + x + i].Attributes = color;
		i++;
	}
}

void Draw(int x, int y, short color, short pixelStatus)
{
	if(x < 0 || x > cge.screenWidth || y < 0 || y > cge.screenHeight)
		return;
	
	cge.screenBuffer[y * cge.screenWidth + x].Char.UnicodeChar = pixelStatus;
	cge.screenBuffer[y * cge.screenWidth + x].Attributes = color;
}

void DrawLineH(short pixelStatus, short color, int x, int y, int dx, int dy, int dx1, int dy1, int x1, int y1, int x2, int y2, int xe, int px)
{
	if (dx >= 0)
		{ x = x1; y = y1; xe = x2; }
	else
		{ x = x2; y = y2; xe = x1;}

	Draw(x, y, pixelStatus, color);
	
	for (int i = 0; x<xe; i++)
	{
		x = x + 1;
		if (px<0)
			px = px + 2 * dy1;
		else
		{
			if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
			px = px + 2 * (dy1 - dx1);
		}
		Draw(x, y, pixelStatus, color);
	}
}

void DrawLineV(short pixelStatus, short color, int x, int y, int dx, int dy, int dx1, int dy1, int x1, int y1, int x2, int y2, int ye, int py)
{
	if (dy >= 0)
		{ x = x1; y = y1; ye = y2; }
	else
		{ x = x2; y = y2; ye = y1; }

	Draw(x, y, pixelStatus, color);

	for (int i = 0; y<ye; i++)
	{
		y = y + 1;
		if (py <= 0)
			py = py + 2 * dx1;
		else
		{
			if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
			py = py + 2 * (dx1 - dy1);
		}
		Draw(x, y, pixelStatus, color);
	}
}

void cgeDrawLine(int x1, int y1, int x2, int y2, short color, short pixelStatus)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	
	dx = x2 - x1;
	dy = y2 - y1;
	
	dx1 = abs(dx);
	dy1 = abs(dy);
	
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;
	
	if (dy1 <= dx1)
		DrawLineH(pixelStatus, color, x, y, dx, dy, dx1, dy1, x1, y1, x2, y2, xe, px);
	else
		DrawLineV(pixelStatus, color, x, y, dx, dy, dx1, dy1, x1, y1, x2, y2, ye, py);
}

void cgeDrawSprite(int x, int y, struct CGE_Sprite *sprite)
{
	if(sprite == NULL)
		return;
	
	for(int i = 0; i < sprite->height*sprite->width; ++i)
	{
		int dx = i/sprite->width;
		int dy = i%sprite->height;
		
		if(sprite->glyphs[i] == L'@')
			continue;
		Draw(x+dx, y+dy, sprite->colors[i], sprite->glyphs[i]);
	}
}

void cgeClearBuffer(void)
{
	memset(cge.screenBuffer, 0, sizeof(CHAR_INFO)*cge.screenWidth*cge.screenHeight);
}

void PollKeys(void)
{
	for(int i = 0; i < 256; ++i)
	{
		cge.keyNewState[i] = GetAsyncKeyState(i);
		
		cge.keys[i].bPressed = 0;
		cge.keys[i].bReleased = 0;
		
		if(cge.keyNewState[i] == cge.keyOldState[i])
			continue;
		
		if(cge.keyNewState[i] & 0x8000)
		{
			cge.keys[i].bPressed = !cge.keys[i].bHeld;
			cge.keys[i].bHeld = 1;
		}
		else
		{
			cge.keys[i].bReleased = 1;
			cge.keys[i].bHeld = 0;
		}
		
		cge.keyOldState[i] = cge.keyNewState[i];
	}
}

void HandleMouseEvent(MOUSE_EVENT_RECORD record)
{
	switch(record.dwEventFlags)
	{
		case MOUSE_MOVED:
			cge.mouseX = record.dwMousePosition.X;
			cge.mouseY = record.dwMousePosition.Y;
			break;
		case 0:
			for(int m = 0; m < 5; ++m)
				cge.mouseNewState[m] = (record.dwButtonState & (1 << m)) > 0;
			break;
		
		default:
			break;
	}
}

void PollMouse(void)
{
	INPUT_RECORD inBuff[32];
	DWORD events = 0;
	GetNumberOfConsoleInputEvents(cge.hConsoleIn, &events);
	if(events <= 0)
		return;
	
	ReadConsoleInput(cge.hConsoleIn, inBuff, events, &events);
	
	for(DWORD i = 0; i < events; ++i)
	{	
		switch(inBuff[i].EventType)
		{
			case MOUSE_EVENT:
				HandleMouseEvent(inBuff[i].Event.MouseEvent);
				break;
		}
	}
	
	for(int m = 0; m < 5; ++m)
	{
		cge.mouse[m].bReleased = 0;
		cge.mouse[m].bPressed = 0;
		
		if(cge.mouseNewState[m] == cge.mouseOldState[m])
			continue;
		
		if(cge.mouseNewState[m])
		{
			cge.mouse[m].bPressed = 1;
			cge.mouse[m].bHeld = 1;
		}
		else
		{
			cge.mouse[m].bReleased = 1;
			cge.mouse[m].bHeld = 0;
		}
		
		cge.mouseOldState[m] = cge.mouseNewState[m];
	}
}

void cgePollEvents(void)
{
	clock_t now = clock();
	cge.elapsed = ((double)now - cge.oldTime) / CLOCKS_PER_SEC;
	cge.oldTime = now;
	
	PollKeys();
	
	PollMouse();
}

double cgeGetElapsedTime(void)
{
	return cge.elapsed;
}

void cgeSwapBuffer(void)
{
	WriteConsoleOutput(cge.hConsole, cge.screenBuffer, (COORD){ (short)cge.screenWidth, (short)cge.screenHeight }, (COORD){ 0, 0 }, &cge.rectWindow);
}

unsigned char GetKeyDown(int i)
{
	return cge.keys[i].bPressed;
}

unsigned char GetKey(int i)
{
	return cge.keys[i].bHeld;
}

unsigned char GetKeyUp(int i)
{
	return cge.keys[i].bReleased;
}

unsigned char GetMouseDown(int i)
{
	return cge.mouse[i].bPressed;
}

unsigned char GetMouse(int i)
{
	return cge.mouse[i].bHeld;
}

unsigned char GetMouseUp(int i)
{
	return cge.mouse[i].bReleased;
}

int cgeMouseX(void)
{
	return cge.mouseX;
}

int cgeMouseY(void)
{
	return cge.mouseY;
}