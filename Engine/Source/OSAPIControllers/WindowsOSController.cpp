#include "EnginePCH.h"

#include "WindowsOSController.h"
#include "SettingsHandler.h"

#define WND_CLASS L"AnibusWindowClass"
#define WND_TITLE L"Lugansk"

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

CWindowsOSController::~CWindowsOSController()
{
	m_pWndClass = nullptr;
	m_hWnd = 0;
	m_hInstance = 0;

	m_bIsWindowCreated = false;
	m_bIsActive = false;
}

// hinstance = HMODULE from GetModuleHandle(nullptr);
CHError CWindowsOSController::VCreateWindow()
{
	HMODULE hMod = GetModuleHandle(nullptr);
	m_hInstance = hMod;

	m_pWndClass.reset(new WNDCLASS);
	ZeroMemory(m_pWndClass.get(), sizeof(WNDCLASS));
	m_pWndClass->hbrBackground = CreateSolidBrush(RGB(32, 32, 255));
	m_pWndClass->hCursor = LoadCursor(0, IDC_ARROW);
	m_pWndClass->hIcon = LoadIcon(0, IDI_APPLICATION);
	m_pWndClass->hInstance = m_hInstance;
	m_pWndClass->lpfnWndProc = &WndProc;
	m_pWndClass->lpszClassName = WND_CLASS;
	m_pWndClass->style = CS_OWNDC;
	m_pWndClass->cbClsExtra = 0;
	m_pWndClass->cbWndExtra = 0;
	m_pWndClass->lpszMenuName = L"";
	if (!RegisterClass(m_pWndClass.get()))
	{
		return new CError_CreateWindowFailed(AT);
	}

	m_hWnd = CreateWindow(WND_CLASS, WND_TITLE, 
		//WS_POPUP,
		WS_OVERLAPPEDWINDOW,
		//WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		10, 10,
		CSettingsHandler::GetInstance()->GetWindowWidth(), 
		CSettingsHandler::GetInstance()->GetWindowHeight(), 0, 0, m_hInstance, 0);
	if (!m_hWnd)
	{
		return new CError_CreateWindowFailed(AT);
	}

	ShowWindow(m_hWnd, 1);
	UpdateWindow(m_hWnd);

	LOG_INFO("CWindowsOSController: window has been created");

	return new CError_OK(AT);
}

bool CWindowsOSController::VProcessMessages() const
{
	bool bIsGetMessage = false;

	MSG Msg = { 0 };

	if (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);

		bIsGetMessage = true;
	}

	if (Msg.message == WM_QUIT)
	{
		LOG_INFO("CWindowsOSController: exit initizlizated");

		CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::SHUTDOWN);
	}

	return bIsGetMessage;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool bCurActive = true;
	static int bIsMouseInWindow = -1;

	if (bIsMouseInWindow == -1)
	{
		POINT MousePos;
		GetCursorPos(&MousePos);
		ScreenToClient(hWnd, &MousePos);

		RECT Rect;
		GetWindowRect(hWnd, &Rect);

		if (Rect.bottom != 0)
		{
			Rect.bottom -= Rect.top;
			Rect.right -= Rect.left;

			if (MousePos.x >= 0 && MousePos.x <= Rect.right &&
				MousePos.y >= 0 && MousePos.y <= Rect.bottom)
			{
				bIsMouseInWindow = true;
				while (ShowCursor(false) >= 0);
				CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::PAUSE, false);
			}
			else
			{
				bIsMouseInWindow = false;
				while (ShowCursor(true) < 0);
				CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::PAUSE, true);
			}
		}
	}

	switch (uMsg)
	{
	case WM_CHAR:
		CEGUI::System::getSingletonPtr()->getDefaultGUIContext().
			injectChar((CEGUI::utf32)wParam);
		break;
	// leave, visible
	case WM_MOUSELEAVE:
	case WM_NCMOUSEMOVE:
		if (bIsMouseInWindow)
		{
			bIsMouseInWindow = false;
			while (ShowCursor(true) < 0);
			CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::PAUSE, true);
		}
		break;
	// enter, invisible
	case WM_MOUSEMOVE:
		if (!bIsMouseInWindow)
		{
			bIsMouseInWindow = true;
			while (ShowCursor(false) >= 0);
			CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::PAUSE, false);
		}
		break;
	case WM_ACTIVATEAPP:
		if (wParam == 1 && !bCurActive)
		{
			CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::PAUSE, true);
		}
		else if (wParam == 0 && bCurActive)
		{
			CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::PAUSE, false);
		}
		break;
	case WM_EXITSIZEMOVE:
		RECT rect;
		GetClientRect(hWnd, &rect);

		CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::REQUEST_RESET, rect.right, rect.bottom);

		LOG_INFO("CWindowsOSController: window has been resized");

		break;

	case WM_SIZE:
		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_MINIMIZED || wParam == SIZE_RESTORED)
		{
			RECT rect;
			GetClientRect(hWnd, &rect);

			CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::REQUEST_RESET, rect.right, rect.bottom);

			LOG_INFO("CWindowsOSController: window has been resized");
		}

		break;
	case WM_CLOSE:
	case WM_DESTROY:
		LOG_INFO("CWindowsOSController: exit initizlizated");

		PostQuitMessage(0);
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}