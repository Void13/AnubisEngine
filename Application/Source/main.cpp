#include "PCH.h"

#include "RoadEditing\\RoadEditor2D.h"

// какие могут быть колбэки:

void OnInit();
void OnRelease();
void OnUpdate();
void OnReset();

void OnFPSTimer();

void OnActorSpawn();
void OnActorTriggered();
void OnActorUpdate();

void OnKeysInput(E_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown);
void OnMouseInput(E_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDeltaPos, XMINT2 const &_vPos);

CEGUI::FrameWindow *g_pFPSWindow = nullptr;
CEGUI::FrameWindow	*g_pInfoWindow = nullptr;

CRoadEditor2D g_RoadEditor;

bool g_bIsCEGUICreated = false;

void TravelActorAABB(IActor *pActor, float fRadius, float fTimeSlice)
{
	float fTime = (float)CMainLoopHandler::GetInstance()->GetTime() + fTimeSlice;
	//float fTime = fTimeSlice + 50.0f;

	CAABB *pBox = (CAABB *)pActor;
	XMFLOAT3 vPos = pBox->GetPosition();
	vPos.x = fRadius * cosf(fTime);
	vPos.z = fRadius * sinf(fTime);

	pBox->SetPosition(vPos);
}

IActor *pCube = nullptr;
CVehicleActor *pVehicle = nullptr;

CVehicleActor *GetVehicleActor()
{
	return pVehicle;
}

XMVECTOR g_xvOldVehPos;

void OnUpdate()
{
	auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();
	
	XMVECTOR xvVehPos = XMLoadFloat3(&pVehicle->GetPosition());
	XMVECTOR xvVehDir = xvVehPos - g_xvOldVehPos;
	float fVehDirLength = XMVectorGetX(XMVector3Length(xvVehDir));
	xvVehDir = XMVector3Normalize(xvVehDir);
	g_xvOldVehPos = xvVehPos;

	XMVECTOR xvLookAt = xvVehPos;
	xvLookAt += xvVehDir * 5.0f;
	//pCamera->SetLookAt(xvLookAt);

	XMVECTOR xvCamPos = xvVehPos;
	XMVECTOR xvDelta = xvLookAt - xvCamPos;
	xvDelta = XMVector3Normalize(xvDelta);
	xvDelta = XMVector3Rotate(xvDelta, XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f)));

	//xvCamPos += xvDelta * 5.5f;
	xvCamPos.vector4_f32[1] += 5.0f;
	xvCamPos -= xvVehDir * 10.0f;

	//pCamera->SetPosition(xvCamPos);

	/*
	int i = 0;

	float const fCircleRadius = 100.0f;
	for (auto &iActor : GetActorsDynamic())
	{
		float fCurAABB = (float)i / GetActorsDynamic().size();
		float fRad = fCircleRadius * sqrtf(fCurAABB);
		TravelActorAABB(iActor, fRad, i / 2.0f);

		i++;
	}*/
}

void OnFPSTimer()
{
	if (!g_pFPSWindow)
		return;

	// update FPS imagery
	char fps_textbuff[256];

	sprintf(fps_textbuff, "[colour = 'FFFFFFFF']Render time: %0.2f ms\nLogic time: %0.2f ms", 
		1.0f / CMainLoopHandler::GetInstance()->GetRenderFPS(),
		1.0f / CMainLoopHandler::GetInstance()->GetLogicFPS());

	//sprintf(fps_textbuff, "[colour = 'FFFFFFFF']Render time: %0.2f ms\nLogic time: %0.2f ms",
	//	CMainLoopHandler::GetInstance()->GetRenderElapsedTime(),
	//	CMainLoopHandler::GetInstance()->GetLogicElapsedTime());

	auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();
	sprintf(fps_textbuff, "%s\nCamera: %0.1f %0.1f %0.1f", 
		fps_textbuff,
		pCamera->GetPosition().x, pCamera->GetPosition().y, pCamera->GetPosition().z);

	g_pFPSWindow->setText(fps_textbuff);
	
	sprintf(fps_textbuff, "Gear: %d\nSpeed: %0.1f\nRPM: %0.1f\nAccel: %0.1f\nBrake: %0.1f",
		pVehicle->GetVehicle()->mDriveDynData.getCurrentGear() - 1, 
		pVehicle->GetVehicle()->computeForwardSpeed() * 3.6f,
		ConvertRPSToRPM(pVehicle->GetVehicle()->mDriveDynData.getEngineRotationSpeed()),
		pVehicle->GetVehicle()->mDriveDynData.mControlAnalogVals[PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL],
		pVehicle->GetVehicle()->mDriveDynData.mControlAnalogVals[PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE]);
		
	g_pInfoWindow->setText(fps_textbuff);
}

int CALLBACK wWinMain(
	__in  HINSTANCE hInstance,
	__in  HINSTANCE hPrevInstance,
	__in  LPWSTR lpCmdLine,
	__in  int nCmdShow
	)
{
	/*
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	*/

	_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CLogManager::GetInstance()->Init(
		E_LOGDEST::SEP_FILE | E_LOGDEST::DEBUG_WINDOW,
		E_LOGDEST::SEP_FILE | E_LOGDEST::DEBUG_WINDOW | E_LOGDEST::MSGBOX,
		E_LOGDEST::SEP_FILE);

	CEventManager::GetInstance()->StartThread();

	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::SHUTDOWN, &OnRelease);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_INIT, &OnInit);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_KEYS_INPUT, &OnKeysInput);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_MOUSE_INPUT, &OnMouseInput);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_UPDATE, &OnUpdate);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::RESET_SYSTEMS, &OnReset);

	//CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_INIT, &CRoadEditor2D::OnInit, &g_RoadEditor);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_UPDATE, &CRoadEditor2D::OnUpdate, &g_RoadEditor);
	
	int nFPSTimer = CEventManager::GetInstance()->RegCallback(&OnFPSTimer);

	CGlobalSettings settings;
	settings.Init("data\\GlobalSettings.ini");

	CMainLoopHandler::GetInstance()->Init(settings);

	CEventManager::GetInstance()->CallEvent(nFPSTimer, 100.0f, 0);

	CMainLoopHandler::GetInstance()->StartMainLoop();

	CMainLoopHandler::GetInstance()->DestroyInstance();

	//_CrtDumpMemoryLeaks();

	return 0;
}

void OnInit()
{
	CInputManager::GetInstance()->LinkEventToKey(E_INPUT_EVENTS::CREATE_OBJECT, { { E_CHANNELS::MOUSE_LEFT, E_CHANNELS::KEY_LCTRL } });

	CCircleBy3Points circ(XMFLOAT3(-2.0f, 2.0f, 0.0f), XMFLOAT3(3.0f, 4.0f, 0.0f), XMFLOAT3(0.0f, 7.0f, 0.0f));

	g_bIsCEGUICreated = true;

	// INITIALIZATION OF CEGUI
	auto pCEGUIRender = CEGUI::System::getSingletonPtr()->getRenderer();
	OnReset();

	CEGUI::DefaultResourceProvider* rp =
		static_cast<CEGUI::DefaultResourceProvider*>
		(CEGUI::System::getSingleton().getResourceProvider());

	rp->setResourceGroupDirectory("imagesets", "data/GUI/imagesets/");
	rp->setResourceGroupDirectory("fonts", "data/GUI/fonts/");
	rp->setResourceGroupDirectory("schemes", "data/GUI/schemes/");
	rp->setResourceGroupDirectory("looknfeels", "data/GUI/looknfeel/");
	rp->setResourceGroupDirectory("layouts", "data/GUI/layouts/");
	rp->setResourceGroupDirectory("lua_scripts", "data/GUI/lua_scripts/");
	rp->setResourceGroupDirectory("animations", "data/GUI/animations/");

	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	CEGUI::AnimationManager::setDefaultResourceGroup("animations");
	// setup default group for validation schemas
	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");

	CEGUI::SchemeManager::getSingletonPtr()->createFromFile("AlfiskoSkin.scheme");
	CEGUI::Font& defaultFont = CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-14.font");
	CEGUI::System::getSingletonPtr()->getDefaultGUIContext().setDefaultFont(&defaultFont);

	auto pGuiContext = &CEGUI::System::getSingletonPtr()->getDefaultGUIContext();
	pGuiContext->getMouseCursor().setDefaultImage("AlfiskoSkin/MouseArrow");

	// Add FPS window text

	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	auto root = (CEGUI::DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");
	pGuiContext->setRootWindow(root);

	CEGUI::FrameWindow* wnd = (CEGUI::FrameWindow*)winMgr.createWindow("AlfiskoSkin/Label", "FrameTime");
	root->addChild(wnd);

	wnd->setProperty("HorzFormatting", "LeftAligned");
	//wnd->setProperty("HorzLabelFormatting", "true");
	//wnd->setProperty("BackgroundEnabled", "true");
	//wnd->setProperty("FrameEnabled", "true");
	//wnd->setProperty("WordWrap", "True");

	wnd->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim(0.01f)));
	wnd->setSize(CEGUI::USize(cegui_reldim(0.3f), cegui_reldim(0.12f)));

	wnd->setMaxSize(CEGUI::USize(cegui_reldim(1.0f), cegui_reldim(1.0f)));
	wnd->setMinSize(CEGUI::USize(cegui_reldim(0.01f), cegui_reldim(0.01f)));

	wnd->setText("[colour = 'FFFFFFFF']Frame time: 1.0 ms");

	g_pFPSWindow = wnd;

	g_pInfoWindow = (CEGUI::FrameWindow*)winMgr.createWindow("AlfiskoSkin/Label", "InfoWindow");
	root->addChild(g_pInfoWindow);
	g_pInfoWindow->setAlwaysOnTop(true);
	g_pInfoWindow->setAlpha(0.8f);
	//g_pInfoWindow->setProperty("BackgroundEnabled", "true");
	g_pInfoWindow->setPosition(CEGUI::UVector2(cegui_reldim(0.75f), cegui_reldim(0.80f)));
	g_pInfoWindow->setSize(CEGUI::USize(cegui_reldim(0.25f), cegui_reldim(0.20f)));

	const CEGUI::Rectf scrn(CEGUI::Vector2f(0, 0), pCEGUIRender->getDisplaySize());

	auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();
	pCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));
	pCamera->SetLookAt(XMFLOAT3(0.0f, 0.0f, 500.0f));
	pCamera->SetMoveSpeed(5.0f);

	//pCamera->SetOrthogonalMatrix(1000.0f, 1000.0f, 1.0f, 100000.0f);
	pCamera->SetPosition(XMFLOAT3(113.0f, 112.0f, -181.0f));
	pCamera->SetLookAt(XMFLOAT3(175.0f, 80.0f, -181.0f));

	{
		//auto pActor = AddDynamicObject(XMFLOAT3(0.0f, 10000.0f, 0.0f), 0xFF0000FF, E_ACTOR_TYPE::CUBE, E_RIGID_BODY_FLAG::STATIC);

		//auto pActor = AddStaticObject(XMVectorSet(0.0f, 1.0f, 0.0f, -100.0f), 0xAAAAAAFF, E_ACTOR_TYPE::PLANE, E_RIGID_BODY_FLAG::STATIC);
	}
	
	// wall.obj
	// ferrari//ferrari.3ds
	// spider.irrmesh

	{
		//pCube = new CActor_Simple("data\\actors\\world.ini");
		//pCube->VInit();
		//AddActor(pCube);
		
		// CVehicleActor CActor_Simple
		IActor *pTempActor = new CVehicleActor("data\\actors\\testcar.ini");
		pTempActor->VInit();
		//pTempActor->SetRotation(XM_PI / 2, 0.90f, 0.0f);
		pTempActor->SetPosition(XMFLOAT3(75.0f, 11.5f, -131.0f));
		AddActor(pTempActor);
		
		pVehicle = (CVehicleActor *)pTempActor;
	}

	g_RoadEditor.OnInit();
}

XMINT2	g_vMousePos;

void OnMouseInput(E_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDeltaPos, XMINT2 const &_vPos)
{
	g_vMousePos = _vPos;
}

void OnKeysInput(E_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown)
{
	static const XMCOLOR ColorTable[] =
	{
		0xFFFFFFFF,
		0xFF0000FF,
		0x000000FF,
		0x00CDCDFF,
		0x7FFF00FF,
		0xCDAD00FF,
		0x8B658BFF,
		0x8B4726FF,
		0xFF4040FF,
		0xFF00FFFF,
		0x9B30FFFF,
		0x8B008BFF
	};

	static const DWORD dwNumColorTableItems = SizeOfArray(ColorTable);

	switch (_eInputEvent.event)
	{
	case E_INPUT_EVENTS::CREATE_OBJECT:
		if (_bKeyDown)
		{
			//XMFLOAT3 vOldPos = pCube->GetPosition();
			//vOldPos.y += 1.0f;
			//pCube->SetPosition(vOldPos);
			//return;

			auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();

			float fDistance = 20.0f;

			XMFLOAT3 vRayDir;
			XMFLOAT3 vRayOrigin;
			ScreenToWorld(pCamera, g_vMousePos, vRayDir, vRayOrigin);
			vRayOrigin.x += vRayDir.x * fDistance;
			vRayOrigin.y += vRayDir.y * fDistance;
			vRayOrigin.z += vRayDir.z * fDistance;

			float fPitch = XM_PI / 2 - atan2(vRayDir.y, sqrt((vRayDir.x * vRayDir.x) + (vRayDir.z * vRayDir.z)));
			float fYaw = XM_PI / 2 - atan2(vRayDir.z, vRayDir.x);

			auto pActor = AddDynamicObject(vRayOrigin, ColorTable[rand() % dwNumColorTableItems], E_ACTOR_TYPE::CUBE, E_RIGID_BODY_FLAG::DYNAMIC);
			pActor->SetRotation(fPitch, fYaw, 0.0f);

			float fKoef = 50000.0f;
			pActor->ApplyForce(XMFLOAT3(vRayDir.x * fKoef, vRayDir.y * fKoef, vRayDir.z * fKoef));
		}

		break;
	}
}

void OnReset()
{
	if (g_bIsCEGUICreated)
	{
		auto pCEGUIRender = CEGUI::System::getSingletonPtr()->getRenderer();
		if (pCEGUIRender)
			pCEGUIRender->setDisplaySize(CEGUI::Sizef((float)CSettingsHandler::GetInstance()->GetWindowWidth(), (float)CSettingsHandler::GetInstance()->GetWindowHeight()));
	}
}

void OnRelease()
{
	//auto pCEGUIRender = CEGUI::System::getSingletonPtr()->getRenderer(); 
}