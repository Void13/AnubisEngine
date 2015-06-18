#ifndef H__INPUTEVENTS
#define H__INPUTEVENTS

typedef unsigned int INPUTEVENT;

struct E_INPUT_EVENTS
{
	enum E_INPUT_EVENTS22 : INPUTEVENT
	{
		CREATE_OBJECT = 0,
		CHANGE_PROJECTION,
		DRAW_ROAD
	} event;
};

#endif