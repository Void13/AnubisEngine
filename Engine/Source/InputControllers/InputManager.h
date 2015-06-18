#ifndef H__INPUTMANAGER
#define H__INPUTMANAGER

#include "EnginePCH.h"
#include "EngineInputEvents.h"

class IBaseOSAPIController;
class IBaseInputController;

enum class E_CHANNELS : unsigned int
{
	KEY_NONE = 0,
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, 
	KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L,
	KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
	KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
	KEY_Y, KEY_Z,

	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, 
	KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

	KEY_F01, KEY_F02, KEY_F03, KEY_F04, KEY_F05, KEY_F06,
	KEY_F07, KEY_F08, KEY_F09, KEY_F10,	KEY_F11, KEY_F12,

	KEY_MINUS,
	KEY_EQUALS,
	KEY_SLASH,
	KEY_BACKSLASH,

	KEY_ESCAPE,
	KEY_ENTER,
	KEY_PAUSE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	
	KEY_RSHIFT,
	KEY_LSHIFT,
	KEY_RCTRL,
	KEY_LCTRL,
	KEY_RALT,
	KEY_LALT,
	KEY_RWINDOWS,
	KEY_LWINDOWS,
	
	KEY_DELETE,
	KEY_INSERT,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_TILDE,
	KEY_SPACE,
	KEY_COMMA,
	KEY_DOT,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,

	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,

	NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4,
	NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9,

	NUMPAD_PLUS,
	NUMPAD_MINUS,
	NUMPAD_STAR,
	NUMPAD_SLASH,
	NUMPAD_DELETE,
	NUMPAD_NUMLOCK,
	NUMPAD_ENTER,

	MOUSE_MOVE,
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,

	GAMEPAD_A,
	GAMEPAD_B,
	GAMEPAD_X,
	GAMEPAD_Y,
	// ...

	COUNT
};

class CInputManager : public CSingleton<CInputManager>
{
	friend class CSingleton<CInputManager>;
public:
	//							    	REPEATS	   COMBINATION
	typedef std::unordered_map<INPUTEVENT, std::vector<std::vector<E_CHANNELS>>> EVENT_TO_CHANNELS;
	typedef std::unordered_map<E_CHANNELS, INPUTEVENT> CHANNEL_TO_EVENT;
	//typedef std::vector<std::vector<std::vector<E_CHANNELS>>> EVENT_TO_CHANNELS;
	//typedef std::vector<INPUTEVENT> CHANNEL_TO_EVENT;

	CHError Init();
	void InputUpdate();

	void LinkEventToKey(INPUTEVENT _uInputEvent, std::vector<std::vector<E_CHANNELS>> &&_Channels);
private:
	std::vector<std::shared_ptr<IBaseInputController>>
		m_InputControllers;
	 
	EVENT_TO_CHANNELS
		m_InputEventToChannels;

	CHANNEL_TO_EVENT
		m_Channel_To_InputEvent;
};

#endif