#ifndef H__BASEINPUTCONTROLLER
#define H__BASEINPUTCONTROLLER

#include "EnginePCH.h"
#include "InputManager.h"

class IBaseOSAPIController;

class IBaseInputController
{
public:
	virtual ~IBaseInputController()
	{
	};

	virtual CHError VInit() = 0;
	virtual void OnInput(CInputManager::EVENT_TO_CHANNELS const &EventToChannels, CInputManager::CHANNEL_TO_EVENT const &_ChannelToEvent) = 0;
};

#endif