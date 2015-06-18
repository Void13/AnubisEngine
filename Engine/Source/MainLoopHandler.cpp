#include "EnginePCH.h"

#include "MainLoopHandler.h"
#include "SettingsHandler.h"

#include "InputControllers\\InputManager.h"
#include "GraphicsRenderers\\BaseGraphicsRenderer.h"
#include "GAPIControllers\\BaseGAPIController.h"
#include "OSAPIControllers\\BaseOSAPIController.h"
#include "OSAPIControllers\\WindowsOSController.h"
#include "OSAPIControllers\\ScreenshotTaker.h"

#include "Actors\\BaseActor.h"
#include "Actors\\SimpleActor.h"
#include "Camera.h"

#include "Physics\\PhysXLoader.h"

#ifdef _DEBUG
	#pragma comment( lib, "CEGUI\\CEGUIBase-0_d.lib" )
#else
	#pragma comment( lib, "CEGUI\\CEGUIBase-0.lib" )
#endif

CHRTimer g_LogicTimer;
CHRTimer g_RenderTimer;

CHRTimer g_MainTimer;

std::vector<std::shared_ptr<IActor>>	g_Actors;

void AddActor(IActor *pActor)
{
	g_Actors.emplace_back(pActor);
}

CHError CMainLoopHandler::StartMainLoop()
{
	const double SKIP_MS = 16.0; // in ms

	// чтобы в первый тик сразу апдейтнуть логику
	double dNextGameTick = g_MainTimer.GetAbsoluteTime();

	while (!m_bIsShutdowned)
	{
		m_dTime = g_MainTimer.GetAbsoluteTime();

		CEventManager::GetInstance()->VRunThread();
		
		// если пришло сообщение от оси - обрабатываем его и не даём движку управление
		if (CDistributedObjectCreator::GetOSAPIController()->VProcessMessages())
		{
			continue;
		}

		if (m_bIsPaused)
			continue;

		if (m_bIsRequestedReset)
		{
			CSettingsHandler::GetInstance()->SetWindowWidth(m_nRequestedWidth);
			CSettingsHandler::GetInstance()->SetWindowHeight(m_nRequestedHeight);

			CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::RESET_SYSTEMS);

			m_bIsRequestedReset = false;
		}

		// один раз идёт логика
		if (m_dTime >= dNextGameTick)
		{
			OnFrameUpdate();

			// и надо делать загодя немного
			dNextGameTick += SKIP_MS;

			if (dNextGameTick <= m_dTime)
				dNextGameTick = m_dTime + SKIP_MS;
		}

		// from 0 to 1
		// текущее время ушло не дальше, чем на SKIP_MS
		float fInterpolation = (float)((dNextGameTick - m_dTime) / SKIP_MS);
		fInterpolation = 1.0f - MathHelper::Clamp(fInterpolation, 0.0f, 1.0f);

		OnFrameRender(fInterpolation);
	}

	CResourceManager::GetInstance()->Terminate();

	if (m_pPrimaryCamera)
	{
		SAFE_DELETE(m_pPrimaryCamera);
	}

	m_AFCLibrary.CloseArchive();

	// всем подписавшимся на это событие отправляем его
	CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::SHUTDOWN);

	CPhysicsSystem::DestroyInstance();

	CDistributedObjectCreator::DestroyInstance();

	CInputManager::GetInstance()->DestroyInstance();
	CSettingsHandler::GetInstance()->DestroyInstance();

	CEventManager::GetInstance()->Terminate();

	LOG_INFO("App is shutdowned.\n\n");

	CLogManager::GetInstance()->DestroyInstance();

	return new CError_OK(AT);
};

void CMainLoopHandler::OnFrameRender(float fInterpolation)
{
	double dAbsTime = g_MainTimer.GetTime();
	m_dRenderElapsedTime = g_RenderTimer.GetElapsedTime();

	m_nRenderFrames++;
	
	if (dAbsTime - m_dRenderLastTime > 1000.0)
	{
		m_dRenderFPS = (double)m_nRenderFrames / (dAbsTime - m_dRenderLastTime);
		m_dRenderLastTime = dAbsTime;
		m_nRenderFrames = 0;
	}

	// creating wireframe queue
	
	IBaseGraphicsRenderer::RenderQueue		WireframeQueue;
	IBaseGraphicsRenderer::RenderQueue		SolidQueue;
	IBaseGraphicsRenderer::RenderQueue		TransparencyQueue;
	IBaseGraphicsRenderer::RenderQueue		DebugQueue;

	for (auto &iActor : g_Actors)
	{
		iActor->VRecalcRenderMatrices(fInterpolation);

		switch (iActor->GetModelRenderer()->GetRenderMethod())
		{
		case E_RENDERMETHOD::SOLID:
			SolidQueue[iActor->GetModelRenderer()].push_back(iActor.get());
			break;
		case E_RENDERMETHOD::WIREFRAMED:
			WireframeQueue[iActor->GetModelRenderer()].push_back(iActor.get());
			break;
		};
	}

	m_pPrimaryCamera->VRecalcRenderMatrices(fInterpolation);

	CDistributedObjectCreator::GetGraphicsRenderer()->SetSolidQueue(SolidQueue);
	CDistributedObjectCreator::GetGraphicsRenderer()->SetDebugQueue(DebugQueue);
	CDistributedObjectCreator::GetGraphicsRenderer()->SetTransparencyQueue(TransparencyQueue);
	CDistributedObjectCreator::GetGraphicsRenderer()->SetWireframeQueue(WireframeQueue);
	
	CDistributedObjectCreator::GetGraphicsRenderer()->VOnFrameRender();
}

void CMainLoopHandler::OnFrameUpdate()
{
	double dAbsTime = g_MainTimer.GetTime();
	m_dLogicElapsedTime = g_LogicTimer.GetElapsedTime();

	m_nLogicFrames++;

	if (dAbsTime - m_dLogicLastTime > 1000.0)
	{
		m_dLogicFPS = (double)m_nLogicFrames / (dAbsTime - m_dLogicLastTime);
		m_dLogicLastTime = dAbsTime;
		m_nLogicFrames = 0;
	}

	CInputManager::GetInstance()->InputUpdate();

	if (auto sysp = CEGUI::System::getSingletonPtr())
		sysp->getDefaultGUIContext().injectTimePulse((float)m_dLogicElapsedTime);
	
	CPhysicsSystem::GetInstance()->Simulate();
	
	CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::ON_UPDATE);
	
	for (auto iActor : g_Actors)
	{
		iActor->VUpdate();
	}
	
	m_pPrimaryCamera->VUpdate();
};

class CMul
{
public:
	void func111(char pp, int dd)
	{
		data = dd;
	};

	void func333(char pp, int dd) const
	{
		int alla = pp + dd;
	};

	int data;
};

void func222(char pp, int dd)
{
	int alla = pp + dd;
};

CHError CMainLoopHandler::Init(CGlobalSettings const &Settings)
{
	CSettingsHandler::Init(Settings);

	g_MainTimer.Reset();
	g_LogicTimer.Reset();
	g_RenderTimer.Reset();

	SYSTEMTIME SysTime;
	GetSystemTime(&SysTime);
	
	CMul *const th = new CMul;

	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::COUNT, &CMul::func333, th);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::COUNT, &CMul::func111, th);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::COUNT, &func222);

	int timer1 = CEventManager::GetInstance()->RegCallback(&CMul::func333, th);
	int timer2 = CEventManager::GetInstance()->RegCallback(&CMul::func111, th);
	int timer3 = CEventManager::GetInstance()->RegCallback(&func222);

	CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::COUNT, 'g', 3);
	CEventManager::GetInstance()->FastCallEvent(timer1, 'g', 3);

	CEventManager::GetInstance()->CallEvent(E_RESERVED_EVENTS::COUNT, 100.0f, 1, 'g', 3);
	CEventManager::GetInstance()->CallEvent(timer1, 100.0f, 1, 'g', 3);

	CEventManager::GetInstance()->KillEvent(E_RESERVED_EVENTS::COUNT);
	CEventManager::GetInstance()->KillEvent(timer1);
	
	m_fLaunchTime = (float)SysTime.wHour + (SysTime.wMinute + (SysTime.wSecond + SysTime.wMilliseconds / 1000.0f) / 60.0f) / 60.0f;

	{
		CHError er = CDistributedObjectCreator::GetOSAPIController()->VCreateWindow();
		if (!er.IsOK())
			return er;
	}
	
	{
		CHError er = CDistributedObjectCreator::GetGraphicsRenderer()->VInit();
		if (!er.IsOK())
			return er;
	}

	{
		CHError er = CInputManager::GetInstance()->Init();
		if (!er.IsOK())
			return er;
	}

	m_AFCLibrary.OpenArchive("data\\game.afc");

	//CResourceManager::GetInstance()->StartThread();

	CPhysicsSystem::GetInstance()->Init();

	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::PAUSE, &CMainLoopHandler::OnPause, this);

	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::SHUTDOWN, &CMainLoopHandler::OnRelease, this);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::REQUEST_RESET, &CMainLoopHandler::OnRequestReset, this);

	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_MOUSE_INPUT, &CMainLoopHandler::OnMouseInput, this);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_KEYS_INPUT, &CMainLoopHandler::OnKeysInput, this);

	LOG_INFO("CMainLoopHandler: Initialized");

	m_pPrimaryCamera = new CCameraFirstPerson();
	m_pPrimaryCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, 20.0f));
	m_pPrimaryCamera->SetLookAt(XMFLOAT3(-5.0f, -5.0f, 0.0f));
	m_pPrimaryCamera->SetMoveSpeed(20.0f);
	m_pPrimaryCamera->SetRotSpeed(0.5f);

	float fAspectRatio = (float)CSettingsHandler::GetInstance()->GetBackBufferWidth() / (float)CSettingsHandler::GetInstance()->GetBackBufferHeight();
	m_pPrimaryCamera->SetPerspectiveMatrix(XM_PI / 4.0f, fAspectRatio, 0.1f, 100000.0f);

	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_MOVE_FRONT, { { E_CHANNELS::KEY_W } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_MOVE_BACK, { { E_CHANNELS::KEY_S } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_MOVE_LEFT, { { E_CHANNELS::KEY_A } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_MOVE_RIGHT, { { E_CHANNELS::KEY_D } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_MOVE_UP, { { E_CHANNELS::KEY_SPACE } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_MOVE_DOWN, { { E_CHANNELS::KEY_C } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_INC_MOVE_SPEED, { { E_CHANNELS::NUMPAD_PLUS } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_DEC_MOVE_SPEED, { { E_CHANNELS::NUMPAD_MINUS } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_INC_ROLL, { { E_CHANNELS::KEY_Q } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_DEC_ROLL, { { E_CHANNELS::KEY_E } });

	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::KEY_ACCELERATE_MOVE, { { E_CHANNELS::KEY_RSHIFT }, { E_CHANNELS::KEY_LSHIFT } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::EVENT_FREESE, { { E_CHANNELS::MOUSE_LEFT } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::CAMERA_ROTATE, { { E_CHANNELS::MOUSE_MOVE } });

	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::CLOSE, { { E_CHANNELS::KEY_ESCAPE } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::TAKE_SCREENSHOT, { { E_CHANNELS::KEY_LCTRL, E_CHANNELS::KEY_H } });

	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::VEHICLE_ACCELERATE,		{ { E_CHANNELS::KEY_UP } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::VEHICLE_BRAKE,			{ { E_CHANNELS::KEY_DOWN } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::VEHICLE_STEER_LEFT,		{ { E_CHANNELS::KEY_LEFT } });
	CInputManager::GetInstance()->LinkEventToKey(E_ENGINE_INPUT_EVENTS::VEHICLE_STEER_RIGHT,	{ { E_CHANNELS::KEY_RIGHT } });

	CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::ON_INIT);

	return new CError_OK(AT);
};

void CMainLoopHandler::OnRequestReset(int nWidth, int nHeight)
{
	m_bIsRequestedReset = true;
	m_nRequestedWidth = nWidth;
	m_nRequestedHeight = nHeight;
}

void CMainLoopHandler::OnPause(bool bPause)
{
	if (bPause)
	{
		g_LogicTimer.Stop();
		g_MainTimer.Stop();
		g_RenderTimer.Stop();
	}
	else
	{
		g_LogicTimer.Start();
		g_MainTimer.Start();
		g_RenderTimer.Start();
	}

	m_bIsPaused = bPause;
}

void CMainLoopHandler::OnRelease()
{
	m_bIsShutdowned = true;
}

void CMainLoopHandler::OnKeysInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown)
{
	switch (_eInputEvent.e_event)
	{
	case E_ENGINE_INPUT_EVENTS::CLOSE:
		OnRelease();
		break;
	case E_ENGINE_INPUT_EVENTS::EVENT_FREESE:
		m_pPrimaryCamera->SetFreeze(!_bKeyDown);
		break;
	case E_ENGINE_INPUT_EVENTS::TAKE_SCREENSHOT:
		if (_bKeyDown)
		{
			CScreenshotTaker::TakeScreenshot();
		}
		break;
	};
}

void CMainLoopHandler::OnMouseInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDeltaPos, XMINT2 const &_vPos)
{
	if (_vDeltaPos.z != 0)
	{
		float fAspectRatio = (float)CSettingsHandler::GetInstance()->GetWindowWidth() / (float)CSettingsHandler::GetInstance()->GetWindowHeight();
		m_pPrimaryCamera->SetPerspectiveMatrix(m_pPrimaryCamera->GetFOV() + _vDeltaPos.z * 0.001f, fAspectRatio, 0.1f, 100000.0f);
	}
}