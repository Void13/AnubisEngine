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

			// ��������
			if (pEvent->dwExecutedTimes >= pEvent->dwRepeatTimes && pEvent->dwRepeatTimes != 0)
			{
				FastErase(_Timers, i);
			}
		}
	};

	//while (m_bIsInit)
	{
		// ������� ������������ �������
		while (m_EventTimersWrites)
		{
		}

		// ��������, ��� �� ������ ������ ����� ����
		m_EventTimers1Busy = true;

		// ������� ��� ��� �� ������������
		while (m_EventTimersWrites)
		{
		}

		// ������ �������� � ���
		_TimersProcess(m_EventTimers1);

		// ������ ������ ������ ��������
		m_EventTimers1Busy = false;

		// ������� 
		while (m_EventTimersWrites)
		{
		}

		_TimersProcess(m_EventTimers2);
	}
};