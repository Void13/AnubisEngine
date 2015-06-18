#include "EnginePCH.h"
#include "DistributedObjectCreator.h"
#include "SettingsHandler.h"

#ifdef USE_D3D11
	#include "GraphicsRenderers\\D3D11\\D3D11GraphicsRenderer.h"
	#include "GAPIControllers\\D3D11Controller.h"
	#include "GraphicsRenderers\\D3D11\\ModelRenderer.h"
	#include "GraphicsRenderers\\D3D11\\D3D11Texture2D.h"
#endif

#ifdef USE_WINDOWS
	#include "OSAPIControllers\\WindowsOSController.h"
	#include "OSAPIControllers\\WindowsScreenshotTaker.h"
#endif

IBaseModelRenderer *CDistributedObjectCreator::CreateModelRenderer()
{
	switch (CSettingsHandler::GetGAPI())
	{
	case CGlobalSettings::E_GAPI::D3D11:
		return new CD3D11ModelRenderer();
		break;
	};

	return nullptr;
}

ITexture2D *CDistributedObjectCreator::CreateTexture2D()
{
	switch (CSettingsHandler::GetGAPI())
	{
	case CGlobalSettings::E_GAPI::D3D11:
		return new CD3D11Texture2D();
		break;
	};

	return nullptr;
}

// gapi

IBaseGraphicsRenderer *CDistributedObjectCreator::GetGraphicsRenderer()
{
	switch (CSettingsHandler::GetGAPI())
	{
	case CGlobalSettings::E_GAPI::D3D11:
		return CD3D11GraphicsRenderer::GetInstance();
		break;
	};

	return nullptr;
}

IBaseGAPIController *CDistributedObjectCreator::GetGAPIController()
{
	switch (CSettingsHandler::GetGAPI())
	{
	case CGlobalSettings::E_GAPI::D3D11:
		return CD3D11Controller::GetInstance();
		break;
	};

	return nullptr;
}


// os
IBaseOSAPIController *CDistributedObjectCreator::GetOSAPIController()
{
	switch (CSettingsHandler::GetOSAPI())
	{
	case CGlobalSettings::E_OSAPI::WINDOWS:
		return CWindowsOSController::GetInstance();
		break;
	};

	return nullptr;
}

IBaseScreenshotTaker *CDistributedObjectCreator::GetScreenshotTaker()
{
	switch (CSettingsHandler::GetOSAPI())
	{
	case CGlobalSettings::E_OSAPI::WINDOWS:
		return CWindowsScreenshotTaker::GetInstance();
		break;
	};

	return nullptr;
}