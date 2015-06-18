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

	// это нельзя трогать!
	REQUEST_RESET,

	// это нельзя трогать!
	RESET_DEVICE,

	// использовать это для систем!
	RESET_SYSTEMS,

	COUNT
};

#endif