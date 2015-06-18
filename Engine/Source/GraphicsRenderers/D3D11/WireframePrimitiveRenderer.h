#ifndef H__WIREFRAMERENDERER
#define H__WIREFRAMERENDERER

#include "EnginePCH.h"

class CWireframeRenderer
{
public:
	static void OpenWireframeRenderer();
	static void CloseWireframeRenderer();

	static void InitWireframeRenderer();
	static void DestroyWireframeRenderer();
};

#endif