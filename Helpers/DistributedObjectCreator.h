#pragma once

#include "Singleton.h"

// what I want to create

class IBaseGraphicsRenderer;
class IBaseGAPIController;
class IBaseOSAPIController;
class IBaseScreenshotTaker;
class IBaseModelRenderer;
class ITexture2D;

class CDistributedObjectCreator final : public CSingleton<CDistributedObjectCreator>
{
	friend class CSingleton<CDistributedObjectCreator>;
public:
	// global objects
	static IBaseGraphicsRenderer	*GetGraphicsRenderer();
	static IBaseGAPIController		*GetGAPIController();
	static IBaseOSAPIController		*GetOSAPIController();
	static IBaseScreenshotTaker		*GetScreenshotTaker();

	// local objects
	IBaseModelRenderer *CreateModelRenderer();

	ITexture2D			*CreateTexture2D();
};