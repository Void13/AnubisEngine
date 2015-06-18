#pragma once

#include "EnginePCH.h"
#include "ScreenshotTaker.h"
#include "BaseScreenshotTaker.h"

class CWindowsScreenshotTaker final : 
	public IBaseScreenshotTaker,
	public CSingleton<CWindowsScreenshotTaker>
{
	friend class CSingleton<CWindowsScreenshotTaker>;
	friend class CScreenshotTaker;
private:
	void *VTakeScreenshot(XMINT2 const &vMin = XMINT2(0, 0), XMINT2 const &vMax = XMINT2(0, 0)) override;
};