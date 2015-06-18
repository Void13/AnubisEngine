#ifndef H__EVENTLIST
#define H__EVENTLIST

enum class E_RESERVED_EVENTS : int
{
	NONE = 0,

	MANAGE_MODEL_RESOURCES,

	ON_KEYS_INPUT,
	ON_MOUSE_INPUT,

	ON_UPDATE,

	ON_INIT,

	SHUTDOWN,
	PAUSE,

	// ��� ������ �������!
	REQUEST_RESET,

	// ��� ������ �������!
	RESET_DEVICE,

	// ������������ ��� ��� ������!
	RESET_SYSTEMS,

	COUNT
};

#endif