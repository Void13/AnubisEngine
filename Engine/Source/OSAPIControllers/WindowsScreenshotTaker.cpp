#include "EnginePCH.h"
#include "WindowsScreenshotTaker.h"

#include "WindowsOSController.h"

void *CWindowsScreenshotTaker::VTakeScreenshot(XMINT2 const &vMin, XMINT2 const &vMax)
{
	XMINT2 WindowSize = XMINT2(vMax.x - vMin.x, vMax.y - vMin.y);

	HWND hWnd = CWindowsOSController::GetInstance()->GetHWND();
	HDC hdcScreen = GetDC(hWnd);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, WindowSize.x, WindowSize.y);
	
	HGDIOBJ hOld = SelectObject(hdcMem, hBmp);
	BitBlt(hdcMem, 0, 0, WindowSize.x, WindowSize.y, hdcScreen, vMin.x, vMin.y, SRCCOPY);
	SelectObject(hdcMem, hOld);

	BITMAPINFOHEADER bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER));
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = WindowSize.x;
	bmi.biHeight = WindowSize.y;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;

	void *pImage = malloc(4 * WindowSize.x * WindowSize.y);

	GetDIBits(hdcMem, hBmp, 0, WindowSize.y, pImage, (BITMAPINFO *)&bmi, DIB_RGB_COLORS);

	ReleaseDC(hWnd, hdcScreen);
	DeleteDC(hdcMem);
	DeleteObject(hBmp);
	
	return pImage;
};