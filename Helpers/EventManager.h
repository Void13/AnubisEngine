/*

Event manager
Version: 1.0
Author: Aleksey Golovanov

*/

#ifndef _H__EVENT_MANAGER
#define _H__EVENT_MANAGER

#include "EnginePCH.h"

class CEventManagerThread;
class CEventManager;

#include "EventList.h"

typedef size_t TIMERID;

class CEventManager : public CSingleton<CEventManagerThread>
{
	friend class CSingleton<CEventManagerThread>;
private:
	template<typename ...ARGS>
	using STDFUNCARGS = std::function<void(ARGS...)>;

	template<typename C, typename... ARGS>
	std::function<void(ARGS...)> ObjectBind(void(C::*_Func)(ARGS...) const, C const *_Object)
	{
		return [=](ARGS&&... _Args) { return (_Object->*_Func)(std::forward<ARGS>(_Args)...); };
	};

	template<typename C, typename... ARGS>
	std::function<void(ARGS...)> ObjectBind(void(C::*_Func)(ARGS...), C *const _Object)
	{
		return [=](ARGS&&... _Args) { return (_Object->*_Func)(std::forward<ARGS>(_Args)...); };
	};

protected:
	typedef	std::function<void(void)> STDFUNC;

	struct CEventTimer
	{
		~CEventTimer()
		{
		};

		std::vector<STDFUNC> Functions;
		float fInterval;
		size_t dwRepeatTimes;

		double dLastExecuted = 0.0;
		size_t dwExecutedTimes = 0;
	};

	CEventManager()
	{
	};

	virtual ~CEventManager()
	{
		m_bIsInit = false;

		//if (m_Thread.joinable())
		//	m_Thread.join();

		for (auto &iFunc : m_EventCallbacks)
		{
			SAFE_DELETE(iFunc);
		}

		for (auto &iVector : m_ReservedEventCallbacks)
		{
			for (auto &iFunc : iVector.second)
			{
				SAFE_DELETE(iFunc);
			}
		}
	};
public:
	virtual void VRunThread() = 0;

	void StartThread()
	{
		m_bIsInit = true;

		//m_Thread = std::thread(&CEventManager::RunThread, this);

		LOG_INFO("CEventManager: Thread has been started");
	};

	CEventManager(CEventManager const &) = delete;
	CEventManager& operator=(CEventManager const &) = delete;

	template <typename... ARGS>
	TIMERID RegCallback(void(*_Func)(ARGS...))
	{
		if (!m_bIsInit)
		{
			CHError er = new CError_EventSystemNotInitialized(AT);
			return -1;
		}

		void *pStoredFunc = new STDFUNCARGS<ARGS...>(std::forward<void(*)(ARGS...)>(_Func));

		if (!pStoredFunc)
		{
			CHError er = new CError_EventCreatingFailed(AT);
			return -1;
		}

		m_EventCallbacks.push_back(pStoredFunc);

		return m_EventCallbacks.size() - 1;
	};

	template <typename C, typename... ARGS>
	TIMERID RegCallback(void(C::*_Func)(ARGS...), C *const _Object)
	{
		if (!m_bIsInit)
		{
			CHError er = new CError_EventSystemNotInitialized(AT);
			return -1;
		}

		// ObjectBind MakeCallback
		STDFUNCARGS<ARGS...> Func = ObjectBind(std::forward<void(C::*)(ARGS...)>(_Func), std::forward<C *const>(_Object));

		void *pStoredFunc = new STDFUNCARGS<ARGS...>(std::forward<STDFUNCARGS<ARGS...>>(Func));

		if (!pStoredFunc)
		{
			CHError er = new CError_EventCreatingFailed(AT);
			return -1;
		}

		m_EventCallbacks.push_back(pStoredFunc);

		return m_EventCallbacks.size() - 1;
	};

	template <typename C, typename... ARGS>
	TIMERID RegCallback(void(C::*_Func)(ARGS...) const, C const *const _Object)
	{
		if (!m_bIsInit)
		{
			CHError er = new CError_EventSystemNotInitialized(AT);
			return -1;
		}

		// ObjectBind MakeCallback
		STDFUNCARGS<ARGS...> Func = ObjectBind(std::forward<void(C::*)(ARGS...) const>(_Func), std::forward<C const *const>(_Object));

		void *pStoredFunc = new STDFUNCARGS<ARGS...>(std::forward<STDFUNCARGS<ARGS...>>(Func));

		if (!pStoredFunc)
		{
			CHError er = new CError_EventCreatingFailed(AT);
			return -1;
		}

		// pushed after stored events
		m_EventCallbacks.push_back(pStoredFunc);

		return m_EventCallbacks.size() - 1;
	};

	// stored events
	template <typename... ARGS>
	void RegCallback(E_RESERVED_EVENTS const _eEv, void(*_Func)(ARGS...))
	{
		if (!m_bIsInit)
		{
			CHError er = new CError_EventSystemNotInitialized(AT);
			return;
		}

		void *pStoredFunc = new STDFUNCARGS<ARGS...>(std::forward<void(*)(ARGS...)>(_Func));

		if (!pStoredFunc)
		{
			CHError er = new CError_EventCreatingFailed(AT);
			return;
		}

		m_ReservedEventCallbacks[_eEv].push_back(pStoredFunc);
	};

	template <typename C, typename... ARGS>
	void RegCallback(E_RESERVED_EVENTS const _eEv, void(C::*_Func)(ARGS...), C *const _Object)
	{
		if (!m_bIsInit)
		{
			CHError er = new CError_EventSystemNotInitialized(AT);
			return;
		}

		// ObjectBind MakeCallback
		STDFUNCARGS<ARGS...> Func = ObjectBind(std::forward<void(C::*)(ARGS...)>(_Func), std::forward<C *const>(_Object));

		void *pStoredFunc = new STDFUNCARGS<ARGS...>(std::forward<STDFUNCARGS<ARGS...>>(Func));

		if (!pStoredFunc)
		{
			CHError er = new CError_EventCreatingFailed(AT);
			return;
		}

		m_ReservedEventCallbacks[_eEv].push_back(pStoredFunc);
	};

	template <typename C, typename... ARGS>
	void RegCallback(E_RESERVED_EVENTS const _eEv, void(C::*_Func)(ARGS...) const, C const *const _Object)
	{
		if (!m_bIsInit)
		{
			CHError er = new CError_EventSystemNotInitialized(AT);
			return;
		}

		// ObjectBind MakeCallback
		STDFUNCARGS<ARGS...> Func = ObjectBind(std::forward<void(C::*)(ARGS...) const>(_Func), std::forward<C const *const>(_Object));

		void *pStoredFunc = new STDFUNCARGS<ARGS...>(std::forward<STDFUNCARGS<ARGS...>>(Func));

		if (!pStoredFunc)
		{
			CHError er = new CError_EventCreatingFailed(AT);
			return;
		}

		m_ReservedEventCallbacks[_eEv].push_back(pStoredFunc);
	};

	template <typename... ARGS>
	CHError FastCallEvent(TIMERID const _dwTimerID, ARGS&&... _Args)
	{
		if (!m_bIsInit)
		{
			return new CError_EventSystemNotInitialized(AT);
		}

		if (_dwTimerID >= m_EventCallbacks.size())
		{
			return new CError_EventCallingFailed(AT);
		}

		auto const Func = m_EventCallbacks[_dwTimerID];

		STDFUNCARGS<ARGS...> &StoredFunc = *static_cast<STDFUNCARGS<ARGS...> *>(Func);

		STDFUNC BindFunc = std::bind(StoredFunc, std::forward<ARGS>(_Args)...);

		BindFunc();

		return new CError_OK(AT);
	};

	template <typename... ARGS>
	CHError FastCallEvent(E_RESERVED_EVENTS const _eEvent, ARGS&&... _Args)
	{
		if (!m_bIsInit)
		{
			return new CError_EventSystemNotInitialized(AT);
		}

		for (auto const &Func : m_ReservedEventCallbacks[_eEvent])
		{
			STDFUNCARGS<ARGS...> &StoredFunc = *static_cast<STDFUNCARGS<ARGS...> *>(Func);

			STDFUNC BindFunc = std::bind(StoredFunc, std::forward<ARGS>(_Args)...);

			BindFunc();
		}

		return new CError_OK(AT);
	};

	// in seconds
	template <typename... ARGS>
	CHError CallEvent(TIMERID const _dwTimerID, float const _fInterval, size_t const _dwRepeatTimes, ARGS&&... _Args)
	{
		if (!m_bIsInit)
		{
			return new CError_EventSystemNotInitialized(AT);
		}

		if (_fInterval < 0.01f && (_dwRepeatTimes > 1000 || _dwRepeatTimes == 0))
		{
			return new CError_EventCallingFailed(AT);
		}

		if (_dwRepeatTimes > 1000)
		{
			return new CError_EventCallingFailed(AT);
		}

		if (_dwTimerID > m_EventCallbacks.size())
		{
			return new CError_EventCallingFailed(AT);
		}

		auto const Func = m_EventCallbacks[_dwTimerID];

		CEventTimer EventTimer;

		STDFUNCARGS<ARGS...> &StoredFunc = *static_cast<STDFUNCARGS<ARGS...> *>(Func);
		EventTimer.Functions.push_back(std::bind(StoredFunc, std::forward<ARGS>(_Args)...));
		EventTimer.dwRepeatTimes = _dwRepeatTimes;
		EventTimer.fInterval = _fInterval;

		m_EventTimersWrites = true;
		// если первый читает - пишем во второй и наоборот
		if (m_EventTimers1Busy)
		{
			m_EventTimers2.push_back(EventTimer);
		}
		else
		{
			m_EventTimers1.push_back(EventTimer);
		}
		m_EventTimersWrites = false;

		return new CError_OK(AT);
	};

	// in seconds
	template <typename... ARGS>
	CHError CallEvent(E_RESERVED_EVENTS const _eEvent, float const _fInterval, size_t const _dwRepeatTimes, ARGS&&... _Args)
	{
		if (!m_bIsInit)
		{
			return new CError_EventSystemNotInitialized(AT);
		}

		if (_fInterval < 0.01f && (_dwRepeatTimes > 1000 || _dwRepeatTimes == 0))
		{
			return new CError_EventCallingFailed(AT);
		}

		if (_dwRepeatTimes > 1000)
		{
			return new CError_EventCallingFailed(AT);
		}

		CEventTimer EventTimer;

		for (auto const &iFunc : m_ReservedEventCallbacks[_eEvent])
		{
			STDFUNCARGS<ARGS...> &StoredFunc = *static_cast<STDFUNCARGS<ARGS...> *>(iFunc);

			EventTimer.Functions.push_back(std::bind(StoredFunc, std::forward<ARGS>(_Args)...));
		}

		EventTimer.dwRepeatTimes = _dwRepeatTimes;
		EventTimer.fInterval = _fInterval;

		m_EventTimersWrites = true;
		// если первый читает - пишем во второй и наоборот
		if (m_EventTimers1Busy)
		{
			m_EventTimers2.push_back(EventTimer);
		}
		else
		{
			m_EventTimers1.push_back(EventTimer);
		}
		m_EventTimersWrites = false;

		return new CError_OK(AT);
	};

	CHError KillEvent(TIMERID const _dwTimerID)
	{
		if (!m_bIsInit)
		{
			return new CError_EventSystemNotInitialized(AT);
		}

		if (_dwTimerID > m_EventCallbacks.size())
		{
			return new CError_EventKillingFailed(AT);
		}

		auto Func = m_EventCallbacks[_dwTimerID];

		if (Func)
			delete Func;
		Func = nullptr;

		m_EventCallbacks.erase(m_EventCallbacks.begin() + _dwTimerID);

		return new CError_OK(AT);
	};

	CHError KillEvent(E_RESERVED_EVENTS const _eEvent)
	{
		if (!m_bIsInit)
		{
			return new CError_EventSystemNotInitialized(AT);
		}

		for (auto &iFunc : m_ReservedEventCallbacks[_eEvent])
		{
			if (iFunc)
				delete iFunc;
			iFunc = nullptr;
		}

		m_ReservedEventCallbacks[_eEvent].clear();

		return new CError_OK(AT);
	};

	void Terminate();
protected:
	typedef std::vector<CEventTimer> EVENT_VECTOR;

	std::atomic<bool>			m_bIsInit = false;

	std::vector<void *>			m_EventCallbacks;
	std::unordered_map<E_RESERVED_EVENTS, std::vector<void *>> m_ReservedEventCallbacks;
	
	EVENT_VECTOR				m_EventTimers1;
	EVENT_VECTOR				m_EventTimers2;
	std::atomic<bool>			m_EventTimersWrites = false; // нужна для того, чтобы пока кто-то пишет в вектор, никто не залез в вектор(читать, например) (для воркер фрида)
	std::atomic<bool>			m_EventTimers1Busy = false; // а эта нужна для того, чтобы смотреть, какой вектор занят(для мэйн фрида)
	
	//std::thread					m_Thread;
};

class CEventManagerThread final : public CEventManager
{
public:
	CEventManagerThread();
	virtual ~CEventManagerThread();
	void VRunThread() override;
private:

	CHRTimer m_Timer;
};

#endif