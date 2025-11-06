#if defined _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <math.h>
#include "Settings.h"

#define nScreenWidth 120.0f
#define nScreenHeight 120.0f

BOOL bRunning = TRUE;
sRect screen_rect = { { 0,0 },{ nScreenWidth,nScreenHeight },{ 0.0f,0.0f },L' ' };
wchar_t* screen = NULL;
HANDLE hConsole = NULL;
DWORD dwBytesWritten = 0;
float fElapsedTime = 0.0f;
Container Box;

sImage *Moved;

sImage Figures[10];

sImage XWing = { { { 0.0f,0.0f },{ 16.1f,8.0f },{ 0.0f,0.0f },L' ' },
					{L"|              |",
					 L"█              █",
					 L"█              █",
					 L"█              █",
					 L"█              █",
					 L"█____█▓▓▓▓█____█",
					 L"█____██████____█",
					 L"                "} };

sImage Tfighter = { { { 0.0f,0.0f },{ 16.1f,8.0f },{ 0.0f,0.0f },L' ' },
					{L"                ",
					 L"█_█__██████__█_█",
					 L"█_█__██████__█_█",
					 L"█ █  █▓▓▓▓█  █ █",
					 L"█ █          █ █",
					 L"█_█          █_█",
					 L"|||          |||",
					 L"                "} };

sImage TBomber = { { { 0.0f,0.0f },{ 16.1f,8.0f },{ 0.0f,0.0f },L' ' },
					{L"███          ███",
					 L"███ ████████ ███",
					 L"████████████████",
					 L"███ ██▓▓▓▓██ ███",
					 L"███          ███",
					 L"███          ███",
					 L"███          ███",
					 L"|||          |||"} };

sImage Bullet = { { { 0.0f,0.0f },{ 8.1f,4.0f },{ 0.0f,0.0f },L' ' },
					{L"       ██       ",
					 L"      ████      ",
					 L"     ██████     ",
					 L"    ████████    ",
					 L"    ||||||||    ",
					 L"    █||██||█    ",
					 L"    ▓||▓▓||▓    ",
					 L"    ▓▓▓▓▓▓▓▓    "} };

void CreateBullet(sImage f){
	Figures[5] = Bullet;
	Figures[5].r.p = (sVec2d){ f.r.p.x,f.r.p.y };
	Figures[5].r.v = (sVec2d){ 0.0f,-14.0f };
}

BOOL OverlapRectRect(sRect r1, sRect r2) {
	if (r2.p.x + r2.d.x < r1.p.x) {
		return FALSE;
	}
	if (r2.p.x > r1.p.x + r1.d.x) {
		return FALSE;
	}
	if (r2.p.y + r2.d.y < r1.p.y) {
		return FALSE;
	}
	if (r2.p.y > r1.p.y + r1.d.y) {
		return FALSE;
	}
	return TRUE;
}
BOOL bounce(float x,float y,float lx,float ly){
	if(x>=0&&x<lx&&y>=0&&y<ly) return TRUE;
	return FALSE;
}
wchar_t GetChar(sImage img,float x,float y) {
	float nx = (int)x / img.r.d.x * fImageW;
	float ny = (int)y / img.r.d.y * fImageH;
	if(bounce(nx,ny,fImageW,fImageH)){
		return img.Image[(int)ny][(int)nx];
	}
	return L' ';
}
void RenderImage(sImage img) {
	for (float x = (int)img.r.p.x; x < (int)img.r.p.x + img.r.d.x; x+=1.0f) {
		for (float y = (int)img.r.p.y; y < (int)img.r.p.y + img.r.d.y; y+=1.0f) {
			if(bounce(x,y,nScreenWidth,nScreenHeight)){
				float nx = x - (int)img.r.p.x;
				float ny = y - (int)img.r.p.y;
				wchar_t c = GetChar(img,nx,ny);
				if(c==L' ') continue;
				screen[(int)((int)y * nScreenWidth + x)] = c;
			}
		}
	}
}
void RenderRect(sRect rect) {
	for (int x = rect.p.x; x < rect.p.x + rect.d.x; x++) {
		for (int y = rect.p.y; y < rect.p.y + rect.d.y*0.5f; y++) {
			if(bounce(x,y,(float)nScreenWidth,(float)nScreenHeight)){
				screen[(int)(y * nScreenWidth + x)] = rect.sym;
			}
		}
	}
}
void RenderAll(){
	for(int i = 0;i<Box.size;i++){
		if(Box.vRects[i].d.x!=0.0f&&Box.vRects[i].d.x!=0.0f){
			RenderRect(Box.vRects[i]);
		}
	}
	for(int i = 0;i<10;i++){
		if(Figures[i].r.d.x!=0.0f&&Figures[i].r.d.x!=0.0f){
			RenderImage(Figures[i]);
		}
	}
}

void setup() {
	screen = calloc(nScreenWidth * nScreenHeight,sizeof(wchar_t));
	hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (!hConsole) {
		printf("Couldn't create ScreenBuffer!\n");
		bRunning = FALSE;
		return;
	}
	if (!SetConsoleActiveScreenBuffer(hConsole)) {
		printf("Couldn't set ScreenBuffer!\n");
		bRunning = FALSE;
		return;
	}

	Box = (Container){ { 0 } , 10U};
	Box.vRects[0] = (sRect){ { 0,0 },{ 3,nScreenHeight },{ 0.0f,0.0f },0x2588 };
	Box.vRects[1] = (sRect){ { nScreenWidth-3,0 },{ 3,nScreenHeight },{ 0.0f,0.0f },0x2588 };//0x2588

	Figures[0] = XWing;
	Figures[0].r.p = (sVec2d){ 30.0f,40.0f };
	Figures[1] = Tfighter;
	Figures[1].r.p = (sVec2d){ 10.0f,5.0f };
	Figures[2] = Tfighter;
	Figures[2].r.p = (sVec2d){ 30.0f,5.0f };
	Figures[3] = Tfighter;
	Figures[3].r.p = (sVec2d){ 50.0f,5.0f };
	Figures[4] = TBomber;
	Figures[4].r.p = (sVec2d){ 70.0f,5.0f };

	Moved = &(Figures[0]);
}
void input() {
	if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
		Moved->r.v.y = -30;
	}else if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
		Moved->r.v.y = 30;
	}else{
		Moved->r.v.y = 0;
	}
	if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
		Moved->r.v.x = -60;
	}else if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
		Moved->r.v.x = 60;
	}else{
		Moved->r.v.x = 0;
	}

	if (GetAsyncKeyState((unsigned short)'R') & 0x0001) {
		CreateBullet((*Moved));
	}
	if (GetAsyncKeyState((unsigned short)'E') & 0x8000) {
		printf("You did quit\n");
		bRunning = FALSE;
		return;
	}
}
void update() {
	for(int i = 0;i<10;i++){
		Figures[i].r.p.x += Figures[i].r.v.x * fElapsedTime;
		Figures[i].r.p.y += Figures[i].r.v.y * fElapsedTime;
	}
}
void render() {
	RenderRect(screen_rect);
	RenderAll();
	
	swprintf_s(screen, 50, L"AE=%3.0f, P2=%3.0f, X=%3.2f, Y=%3.2f FPS=%3.2f ", 10.0f, 0.0f, Moved->r.p.x, Moved->r.p.y, 1.0f / fElapsedTime);

	screen[(int)(nScreenWidth * nScreenHeight - 1)] = '\0';
	WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, (COORD){ 0,0 }, &dwBytesWritten);
}

int main()
{
	clock_t tp1 = clock();
	
	setup();

	while (bRunning)
	{
		clock_t tp2 = clock();
		fElapsedTime = (float)(tp2-tp1)/CLOCKS_PER_SEC; 
		tp1 = tp2;

		input();
		update();
		render();

		Sleep(10);
	}

	free(screen);
	//screen = NULL;

	return 0;
}

#else
#error "Plattform must be Windows!"
#endif