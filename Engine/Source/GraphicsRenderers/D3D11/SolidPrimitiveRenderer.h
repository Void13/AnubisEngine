#pragma once

#include "EnginePCH.h"

class CSolidRenderer
{
public:
	static void OpenRenderer();
	static void CloseRenderer();

	static void InitRenderer();
	static void DestroyRenderer();
};