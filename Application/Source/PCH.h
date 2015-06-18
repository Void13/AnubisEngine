#ifndef H_PCH
#define H_PCH

#include "EnginePCH.h"

#ifdef _DEBUG
	#pragma comment( lib, "Debug\\EngineD.lib" )
#else
	#pragma comment( lib, "Release\\Engine.lib" )
#endif

#include "MainLoopHandler.h"
#include "OSAPIControllers\\WindowsOSController.h"
#include "GraphicsRenderers\\D3D11\\D3D11GraphicsRenderer.h"
#include "EventManager.h"
#include "Camera.h"
#include "SettingsHandler.h"
#include "InputEvents.h"
#include "InputControllers\\InputManager.h"

#include "Actors\\Object.h"
#include "Actors\\SimpleActor.h"
#include "Actors\\Vehicle.h"
#include "GraphicsRenderers\\BaseMeshRenderer.h"

#include "BasicObjects.h"

CVehicleActor *GetVehicleActor();

#endif