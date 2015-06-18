#include "EnginePCH.h"

#include <dxgi.h>
#include <dinput.h>

#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "dinput8.lib" )

#include "BaseInputController.h"
#include "DirectInput.h"
#include "InputManager.h"
#include "SettingsHandler.h"

#include "OSAPIControllers\\WindowsOSController.h"

CDirectInput::~CDirectInput()
{
	SAFE_RELEASE(m_pKeyBoardDevice);
	SAFE_RELEASE(m_pMouseDevice);
	SAFE_RELEASE(m_pDirectInput);
}

CHError CDirectInput::VInit()
{
	//m_vGlobalMousePos.x = CSettingsHandler::GetInstance()->GetWindowWidth() / 2;
	//m_vGlobalMousePos.y = CSettingsHandler::GetInstance()->GetWindowHeight() / 2;

	//m_vGlobalMousePos.z = 0;

	CWindowsOSController *pWinOS = (CWindowsOSController *)CDistributedObjectCreator::GetOSAPIController();

	HRESULT hr;

	hr = DirectInput8Create(
		pWinOS->GetHInstance(),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_pDirectInput, nullptr);
	if (hr != S_OK)
	{
		return new CError_InputDeviceInitFailed(AT);
	}

	// KEYBOARD
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyBoardDevice, nullptr);
	if (hr != S_OK)
	{
		return new CError_InputDeviceInitFailed(AT);
	}

	m_pKeyBoardDevice->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyBoardDevice->SetCooperativeLevel(pWinOS->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyBoardDevice->Acquire();

	// MOUSE
	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr);
	if (hr != S_OK)
	{
		return new CError_InputDeviceInitFailed(AT);
	}

	m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
	m_pMouseDevice->SetCooperativeLevel(pWinOS->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pMouseDevice->Acquire();

	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_A, DIK_A);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_B, DIK_B);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_C, DIK_C);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_D, DIK_D);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_E, DIK_E);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F, DIK_F);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_G, DIK_G);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_H, DIK_H);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_I, DIK_I);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_J, DIK_J);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_K, DIK_K);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_L, DIK_L);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_M, DIK_M);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_N, DIK_N);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_O, DIK_O);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_P, DIK_P);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_Q, DIK_Q);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_R, DIK_R);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_S, DIK_S);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_T, DIK_T);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_U, DIK_U);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_V, DIK_V);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_W, DIK_W);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_X, DIK_X);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_Y, DIK_Y);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_Z, DIK_Z);

	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_0, DIK_0);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_1, DIK_1);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_2, DIK_2);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_3, DIK_3);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_4, DIK_4);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_5, DIK_5);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_6, DIK_6);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_7, DIK_7);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_8, DIK_8);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_9, DIK_9);

	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F01, DIK_F1);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F02, DIK_F2);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F03, DIK_F3);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F04, DIK_F4);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F05, DIK_F5);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F06, DIK_F6);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F07, DIK_F7);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F08, DIK_F8);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F09, DIK_F9);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F10, DIK_F10);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F11, DIK_F11);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_F12, DIK_F12);

	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_MINUS, DIK_MINUS);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_EQUALS, DIK_EQUALS);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_SLASH, DIK_SLASH);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_BACKSLASH, DIK_BACKSLASH);

	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_ESCAPE, DIK_ESCAPE);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_ENTER, DIK_RETURN);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_PAUSE, DIK_PAUSE);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_HOME, DIK_HOME);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_END, DIK_END);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_PAGEUP, DIK_PRIOR);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_PAGEDOWN, DIK_NEXT);
	
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_RSHIFT, DIK_RSHIFT);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_LSHIFT, DIK_LSHIFT);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_RCTRL, DIK_RCONTROL);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_LCTRL, DIK_LCONTROL);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_RALT, DIK_RALT);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_LALT, DIK_LALT);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_RWINDOWS, DIK_RWIN);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_LWINDOWS, DIK_LWIN);

	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_DELETE, DIK_DELETE);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_INSERT, DIK_INSERT);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_BACKSPACE, DIK_BACK);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_TAB, DIK_TAB);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_TILDE, DIK_GRAVE);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_SPACE, DIK_SPACE);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_COMMA, DIK_COMMA);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_DOT, DIK_PERIOD);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_SEMICOLON, DIK_SEMICOLON);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_APOSTROPHE, DIK_APOSTROPHE);
	
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_UP, DIK_UP);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_DOWN, DIK_DOWN);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_LEFT, DIK_LEFT);
	m_Channel_To_DInput.emplace(E_CHANNELS::KEY_RIGHT, DIK_RIGHT);

	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_0, DIK_NUMPAD0);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_1, DIK_NUMPAD1);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_2, DIK_NUMPAD2);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_3, DIK_NUMPAD3);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_4, DIK_NUMPAD4);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_5, DIK_NUMPAD5);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_6, DIK_NUMPAD6);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_7, DIK_NUMPAD7);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_8, DIK_NUMPAD8);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_9, DIK_NUMPAD9);

	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_PLUS, DIK_ADD);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_MINUS, DIK_SUBTRACT);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_STAR, DIK_MULTIPLY);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_SLASH, DIK_DIVIDE);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_DELETE, DIK_DECIMAL);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_NUMLOCK, DIK_NUMLOCK);
	m_Channel_To_DInput.emplace(E_CHANNELS::NUMPAD_ENTER, DIK_NUMPADENTER);

	m_Channel_To_DInput.emplace(E_CHANNELS::MOUSE_LEFT, 0);
	m_Channel_To_DInput.emplace(E_CHANNELS::MOUSE_MIDDLE, 2);
	m_Channel_To_DInput.emplace(E_CHANNELS::MOUSE_RIGHT, 1);

	LOG_INFO("CDirectInput: input has been created");

	return new CError_OK(AT);
}

void CDirectInput::OnInput(CInputManager::EVENT_TO_CHANNELS const &EventToChannels, CInputManager::CHANNEL_TO_EVENT const &_ChannelToEvent)
{
#define KEY_PRESSED(x, y)	((x)[(y)] & 0x80)

	HRESULT hr;

	hr = m_pKeyBoardDevice->GetDeviceState(256, &m_KeysState.at(0));
	if (hr != S_OK)
	{
		m_pKeyBoardDevice->Acquire();
	}

	hr = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
	if (hr != S_OK)
	{
		m_pMouseDevice->Acquire();
	}

	/*
	for (DINPUT dik = 0; dik < 256; dik++)
	{
		// find channel of dik
		auto itChannel = m_DInput_To_Channel.find(dik);
		if (itChannel == m_DInput_To_Channel.end())
			continue;
		auto iChannel = itChannel->second;

		// next, find event of channel. BUT. we need many events to one channel. in the future, do this.
		auto itEvent = _ChannelToEvent.find(iChannel);
		if (itEvent == _ChannelToEvent.end())
			continue;
		auto iEvent = itEvent->second;

		// next, find channels of event
		auto itChannels = EventToChannels.find(iEvent);
		if (itChannels == EventToChannels.end())
			continue;
		auto iChannels = itChannels->second;

		// check on pressed combination
		for (auto iCombination : iChannels)
		{
			// check combination
			// if true, resolve it.
		}
	}
	*/

	// KEYBOARD INPUT
	for (auto const &iEventAndChannels : EventToChannels)
	{
		// throw all reapeated combinations
		for (auto const iCombination : iEventAndChannels.second)
		{
			int nKeysDown = 0;
			int nOldKeysDown = 0;

			int nOverallKeys = iCombination.size();

			for (auto const iChannel : iCombination)
			{
				auto itDInput = m_Channel_To_DInput.find(iChannel);
				if (itDInput == m_Channel_To_DInput.end())
					continue;

				auto iDInput = itDInput->second;

				bool bIsNewPressed = KEY_PRESSED(m_KeysState, iDInput) != 0;
				bool bIsOldPressed = KEY_PRESSED(m_OldKeysState, iDInput) != 0;

				int nIsMouseKey = 0;

				if (iChannel == E_CHANNELS::MOUSE_LEFT ||
					iChannel == E_CHANNELS::MOUSE_RIGHT ||
					iChannel == E_CHANNELS::MOUSE_MIDDLE)
				{
					nIsMouseKey = (int)iChannel - (int)E_CHANNELS::MOUSE_LEFT;

					bIsNewPressed |= KEY_PRESSED(m_MouseState.rgbButtons, iDInput) != 0;
					bIsOldPressed |= KEY_PRESSED(m_OldMouseState.rgbButtons, iDInput) != 0;
				}

				if (bIsNewPressed)
				{
					nKeysDown++;

					if (!bIsOldPressed)
					{
						if (!nIsMouseKey)
						{
							if (auto sysp = CEGUI::System::getSingletonPtr())
								sysp->getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)iDInput);
						}
					}
				}

				if (bIsOldPressed)
				{
					nOldKeysDown++;

					if (!bIsNewPressed)
					{
						if (!nIsMouseKey)
						{
							if (auto sysp = CEGUI::System::getSingletonPtr())
								sysp->getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)iDInput);
						}
					}
				}
			}

			// if all is down, but not all old is down
			if (nKeysDown == nOverallKeys && nOldKeysDown != nOverallKeys)
			{
				// comb is down
				CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::ON_KEYS_INPUT, iEventAndChannels.first, true);

				break;
			}
			// if not all is down, but all old is down
			else if (nKeysDown != nOverallKeys && nOldKeysDown == nOverallKeys)
			{
				// comb is up
				CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::ON_KEYS_INPUT, iEventAndChannels.first, false);

				break;
			}
		}
	}

	m_OldKeysState.assign(m_KeysState.begin(), m_KeysState.end());

	for (int i = 0; i < 3; i++)
	{
		bool bNewPressed = KEY_PRESSED(m_MouseState.rgbButtons, i) != 0;
		bool bOldPressed = KEY_PRESSED(m_OldMouseState.rgbButtons, i) != 0;

		if (bNewPressed && !bOldPressed)
		{
			if (auto sysp = CEGUI::System::getSingletonPtr())
				sysp->getDefaultGUIContext().injectMouseButtonDown((CEGUI::MouseButton)i);
		}

		if (!bNewPressed && bOldPressed)
		{
			if (auto sysp = CEGUI::System::getSingletonPtr())
				sysp->getDefaultGUIContext().injectMouseButtonUp((CEGUI::MouseButton)i);
		}
	}

	if (m_MouseState.lX != m_OldMouseState.lX ||
		m_MouseState.lY != m_OldMouseState.lY ||
		m_MouseState.lZ != m_OldMouseState.lZ)
	{
		// get local pos of cursor in window

		CWindowsOSController *pWinOS = (CWindowsOSController *)CDistributedObjectCreator::GetOSAPIController();

		POINT MousePos;
		GetCursorPos(&MousePos);
		ScreenToClient(pWinOS->GetHWND(), &MousePos);

		XMINT2 vGlobalMousePos;
		vGlobalMousePos.x = MousePos.x;
		vGlobalMousePos.y = MousePos.y;

		XMINT3 vMouseDelta = XMINT3(m_MouseState.lX, m_MouseState.lY, m_MouseState.lZ);

		//if (auto sysp = CEGUI::System::getSingletonPtr())
		//	sysp->getDefaultGUIContext().injectMouseMove((float)vMouseDelta.x, (float)vMouseDelta.y);

		if (auto sysp = CEGUI::System::getSingletonPtr())
			sysp->getDefaultGUIContext().injectMousePosition((float)vGlobalMousePos.x, (float)vGlobalMousePos.y);

		if (auto sysp = CEGUI::System::getSingletonPtr())
			sysp->getDefaultGUIContext().injectMouseWheelChange((float)vMouseDelta.z);

		auto itEvent = _ChannelToEvent.find(E_CHANNELS::MOUSE_MOVE);
		if (itEvent != _ChannelToEvent.end())
		{
			auto iEvent = itEvent->second;

			CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::ON_MOUSE_INPUT, iEvent, vMouseDelta, vGlobalMousePos);
		}
	}

	m_OldMouseState = m_MouseState;

#undef KEY_PRESSED
}