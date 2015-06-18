#ifndef H__BASEOSAPICONTR
#define H__BASEOSAPICONTR

#include "EnginePCH.h"

class IBaseOSAPIController
{
public:
	IBaseOSAPIController()
	{
	};

	virtual ~IBaseOSAPIController()
	{
	};

	virtual CHError VCreateWindow() = 0;
	virtual bool VProcessMessages() const = 0;
};

#endif