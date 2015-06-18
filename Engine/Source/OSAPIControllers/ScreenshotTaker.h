#pragma once

#include "EnginePCH.h"

class CScreenshotTaker
{
public:
	static void TakeScreenshot(XMINT2 const &vMin = XMINT2(0, 0), XMINT2 const &vMax = XMINT2(0, 0));
};