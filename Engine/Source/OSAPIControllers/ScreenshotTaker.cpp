#include "EnginePCH.h"
#include "ScreenshotTaker.h"
#include "SettingsHandler.h"

#include "DevIL\\il.h"
#include "DevIL\\ilu.h"
#include "DevIL\\ilut.h"

#pragma comment(lib, "DevIL\\devil.lib")
#pragma comment(lib, "DevIL\\ilu.lib")
#pragma comment(lib, "DevIL\\ilut.lib")

#include "BaseScreenshotTaker.h"

void CScreenshotTaker::TakeScreenshot(XMINT2 const &vMin, XMINT2 const &vMax)
{
	XMINT2 vCurrectMin = vMin;
	XMINT2 vCurrectMax = vMax;

	if (vMax.x < 0 && vMax.y < 0 ||
		vMax.x == 0 && vMax.y == 0 ||
		vMax.x > CSettingsHandler::GetInstance()->GetWindowWidth() ||
		vMax.y > CSettingsHandler::GetInstance()->GetWindowHeight())
	{
		vCurrectMax.x = CSettingsHandler::GetInstance()->GetWindowWidth();
		vCurrectMax.y = CSettingsHandler::GetInstance()->GetWindowHeight();
	}

	if (vMin.x < 0 && vMin.y < 0 ||
		vMin.x > CSettingsHandler::GetInstance()->GetWindowWidth() ||
		vMin.y > CSettingsHandler::GetInstance()->GetWindowHeight())
	{
		vCurrectMin = XMINT2(0, 0);
	}
	
	ilInit();
	iluInit();

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	ilEnable(IL_TYPE_SET);
	ilTypeFunc(IL_UNSIGNED_BYTE);

	ilutRenderer(ILUT_OPENGL);

	ilEnable(IL_FILE_OVERWRITE);

	iluSetLanguage(ILU_ENGLISH);

	ILuint ImgID = 0;
	ilGenImages(1, &ImgID);

	ilBindImage(ImgID);
	
	void *pImage = CDistributedObjectCreator::GetScreenshotTaker()->VTakeScreenshot(vCurrectMin, vCurrectMax);

	if (!pImage)
	{
		CHError er = new CError_ScreenshotTakeFailed(AT);
		return;
	}

	if (!ilTexImage(
		vCurrectMax.x - vCurrectMin.x, 
		vCurrectMax.y - vCurrectMin.y, 
		1, 4, IL_BGRA, IL_UNSIGNED_BYTE, pImage))
	{
		auto ErrStr = iluErrorString(ilGetError());
		auto CurrectErrStr = StringHelper::ws2s(ErrStr);

		LOG_ERROR("Screenshoter: %s", CurrectErrStr.c_str());

		CHError er = new CError_ScreenshotTakeFailed(AT);
		return;
	}

	delete[] pImage;

	_wmkdir(L"Screenshots");

	SYSTEMTIME lt;
	GetLocalTime(&lt);
	wchar_t str[100];
	swprintf_s(str, 100, L"Screenshots\\ScreenShot_%d-%02d-%02d %02d-%02d-%02d.png", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);

	if (!ilSave(IL_PNG, str))
	{
		auto ErrStr = iluErrorString(ilGetError());
		auto CurrectErrStr = StringHelper::ws2s(ErrStr);

		LOG_ERROR("Screenshoter: %s", CurrectErrStr.c_str());

		CHError er = new CError_ScreenshotTakeFailed(AT);
		return;
	}

	ilBindImage(0);
	ilDeleteImage(ImgID);
	ilShutDown();

	LOG_INFO("Screenshot was taked: ScreenShot_%d-%02d-%02d %02d-%02d-%02d.png", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
};