#include "screenshot.h"


using namespace std;
using namespace std::chrono;


int SX = 1600, SY = 900;
int ScreenX = SX;
int ScreenY = SY;
// BYTE* ScreenDataTmp = NULL;
BYTE* ScreenDataTmp = (BYTE*)malloc(3 * ScreenX * ScreenY);
// BYTE* ScreenData = 0;

BYTE* screenshot::getBitmap() {
	
    // copy screen to bitmap
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, SX, SY);
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, SX, SY, hScreen, 0, 0, SRCCOPY);
	// get hbitmap pixel data
	ScreenX = GetDeviceCaps(hScreen, HORZRES);
	ScreenY = GetDeviceCaps(hScreen, VERTRES);
	
	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	// bmi.biBitCount = 32; // rgba
	bmi.biBitCount = 24; // rgb
	bmi.biWidth = ScreenX;
	bmi.biHeight = -ScreenY;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;
	bmi.biXPelsPerMeter = 0;
	bmi.biYPelsPerMeter = 0;
	bmi.biClrUsed = 0;
	bmi.biClrImportant = 0;
	
	
	// BYTE* ScreenDataTmp = (BYTE*)malloc(4 * ScreenX * ScreenY);
	// data include rgba stored in ScreenData
	GetDIBits(hDC, hBitmap, 0, ScreenY, ScreenDataTmp, (BITMAPINFO*)& bmi, DIB_RGB_COLORS);

	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);

	return ScreenDataTmp;
}
