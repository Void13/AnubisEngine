#pragma once

#include "EnginePCH.h"

class CScreenshotTaker;

class IBaseScreenshotTaker
{
	friend class CScreenshotTaker;
protected:
	virtual void *VTakeScreenshot(XMINT2 const &vMin = XMINT2(0, 0), XMINT2 const &vMax = XMINT2(0, 0)) = 0;
};