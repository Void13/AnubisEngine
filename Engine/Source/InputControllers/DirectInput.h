#ifndef H__DIRECTINPUT
#define H__DIRECTINPUT

#include "EnginePCH.h"

#include <dinput.h>

#include "BaseInputController.h"

class IBaseOSAPIController;
class CWindowsOSController;

class CDirectInput final : public IBaseInputController
{
public:
	CDirectInput()
	{
	}

	virtual ~CDirectInput();

	virtual CHError VInit() override;
	void OnInput(CInputManager::EVENT_TO_CHANNELS const &EventToChannels, CInputManager::CHANNEL_TO_EVENT const &_ChannelToEvent) override;
private:
	typedef unsigned int DINPUT;
	typedef std::unordered_map<E_CHANNELS, DINPUT> CHANNEL_TO_DINPUT;

	IDirectInput8			*m_pDirectInput = nullptr;
	IDirectInputDevice8		*m_pMouseDevice = nullptr;
	IDirectInputDevice8		*m_pKeyBoardDevice = nullptr;

	std::vector<char>		m_KeysState = std::vector<char>(256, 0);
	DIMOUSESTATE			m_MouseState;

	std::vector<char>		m_OldKeysState = std::vector<char>(256, 0);
	DIMOUSESTATE			m_OldMouseState;

	CHANNEL_TO_DINPUT		m_Channel_To_DInput;
};

#endif