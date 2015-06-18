#include "EnginePCH.h"

#include "InputManager.h"
#include "DirectInput.h"

#include "OSAPIControllers\\BaseOSAPIController.h"

CHError CInputManager::Init()
{
	{
		std::shared_ptr<IBaseInputController> DirectInput(new CDirectInput);
		CHError er = DirectInput->VInit();
		if (!er.IsOK())
		{
			return er;
		}
		m_InputControllers.emplace_back(DirectInput);
	}

	//m_InputEventToChannels.resize(E_ENGINE_INPUT_EVENTS::NUM_EVENTS);
	//m_Channel_To_InputEvent.resize(E_CHANNELS::COUNT);

	return new CError_OK(AT);
}

void CInputManager::InputUpdate()
{
	for (auto iController : m_InputControllers)
	{
		iController->OnInput(m_InputEventToChannels, m_Channel_To_InputEvent);
	}
}

void CInputManager::LinkEventToKey(INPUTEVENT _uInputEvent, std::vector<std::vector<E_CHANNELS>> &&_Channels)
{
	for (auto iCombination : _Channels)
	{
		for (auto iChannel : iCombination)
		{
			m_Channel_To_InputEvent[iChannel] = _uInputEvent;
		}
	}

	m_InputEventToChannels[_uInputEvent] = std::forward<std::vector<std::vector<E_CHANNELS>>>(_Channels);
}