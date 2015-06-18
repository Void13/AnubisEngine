#include "EnginePCH.h"

/*

Event manager
Version: 1.0
Author: Aleksey Golovanov

*/

#include "EventManager.h"

void CEventManager::Terminate()
{
	m_bIsInit = false;

	//if (m_Thread.joinable())
	//	m_Thread.join();

	DestroyInstance();
};

CEventManagerThread::CEventManagerThread() :
CEventManager()
{
	m_Timer.Reset();
};

CEventManagerThread::~CEventManagerThread()
{
};

void CEventManagerThread::VRunThread()
{
	auto _TimersProcess = [this](EVENT_VECTOR &_Timers)
	{
		for (size_t i = 0; i < _Timers.size(); i++)
		{
			CEventTimer *pEvent = &_Timers[i];

			double dCurTime = m_Timer.GetTime();

			if (dCurTime < pEvent->dLastExecuted + pEvent->fInterval)
				continue;

			if (pEvent->dwExecutedTimes < pEvent->dwRepeatTimes || pEvent->dwRepeatTimes == 0)
			{
				for (auto const &iFunc : pEvent->Functions)
				{
					iFunc();
					//printf("Error: %0.1f\n", dCurTime - iEvent.dLastExecuted - iEvent.fInterval);
				}

				pEvent->dLastExecuted = dCurTime;
				pEvent->dwExecutedTimes++;
			}

			// удаление
			if (pEvent->dwExecutedTimes >= pEvent->dwRepeatTimes && pEvent->dwRepeatTimes != 0)
			{
				FastErase(_Timers, i);
			}
		}
	};

	//while (m_bIsInit)
	{
		// ожидаем освобождения вектора
		while (m_EventTimersWrites)
		{
		}

		// сообщаем, что он первый вектор занят стал
		m_EventTimers1Busy = true;

		// ожидаем ещё раз на освобождение
		while (m_EventTimersWrites)
		{
		}

		// теперь работаем с ним
		_TimersProcess(m_EventTimers1);

		// теперь первый вектор свободен
		m_EventTimers1Busy = false;

		// ожидаем 
		while (m_EventTimersWrites)
		{
		}

		_TimersProcess(m_EventTimers2);
	}
};