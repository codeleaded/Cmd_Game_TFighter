#define fImageW 16.0f
#define fImageH 8.0f

typedef unsigned int u32;

typedef struct sVec2d {
	float x;
	float y;
} sVec2d;

typedef struct sRect {
	sVec2d p;
	sVec2d d;
	sVec2d v;
	wchar_t sym; // 0x2588 0x2593 0x2592 0x2591 ' '
} sRect;

typedef struct sImage {
	sRect r;
	wchar_t Image[(int)fImageH][(int)fImageW];
} sImage;

typedef struct Container {
	sRect vRects[10];
	u32 size;
} Container;