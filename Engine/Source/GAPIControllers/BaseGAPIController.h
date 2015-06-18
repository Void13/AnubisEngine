#ifndef H__BASEGAPICONTR
#define H__BASEGAPICONTR

#include "EnginePCH.h"

class IBaseGAPIController
{
public:
	virtual ~IBaseGAPIController()
	{
	};

	virtual CHError VInit() = 0;
	virtual void VStartFrame() = 0;
	virtual void VEndFrame() = 0;
	virtual void VResetDevice() = 0;
};

#endif