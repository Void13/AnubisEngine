#ifndef H__MAINLOOPHANDLER
#define H__MAINLOOPHANDLER

#include "EnginePCH.h"

class IBaseOSAPIController;
class IBaseGraphicsRenderer;
class CActor_Simple;
class ICamera;
class IActor;

class CMainLoopHandler final : public CSingleton<CMainLoopHandler>
{
	friend class CSingleton<CMainLoopHandler>;
public:
	CMainLoopHandler()
	{
	};

	~CMainLoopHandler()
	{
	};

	CHError Init(CGlobalSettings const &Settings);
	CHError StartMainLoop();
	void OnRelease();
	void OnRequestReset(int nWidth, int nHeight);

	void OnFrameUpdate();
	void OnFrameRender(float fInterpolation);

	void OnPause(bool bPause);
	void OnKeysInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown);
	void OnMouseInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDeltaPos, XMINT2 const &_vPos);
	
	float GetLaunchTime() const
	{
		return m_fLaunchTime;
	};

	double GetTime() const
	{
		return m_dTime;
	};

	double GetLogicFPS() const
	{
		return m_dLogicFPS;
	};

	double GetLogicElapsedTime() const
	{
		return m_dLogicElapsedTime;
	};

	double GetRenderFPS() const
	{
		return m_dRenderFPS;
	};

	double GetRenderElapsedTime() const
	{
		return m_dRenderElapsedTime;
	};

	ICamera *GetPrimaryCamera() const
	{
		return m_pPrimaryCamera;
	};

	CAFC *GetAFCLibrary()
	{
		return &m_AFCLibrary;
	};
private:
	bool				m_bIsShutdowned = false;
	bool				m_bIsPaused = false;

	ICamera				*m_pPrimaryCamera = nullptr;

	std::atomic<bool>	m_bIsRequestedReset = false;
	int					m_nRequestedWidth;
	int					m_nRequestedHeight;

	double				m_dTime = 0.0;
	float				m_fLaunchTime = 0.0f;

	int					m_nRenderFrames = 0;
	double				m_dRenderLastTime = 0.0;
	double				m_dRenderFPS = 0.0;
	double				m_dRenderElapsedTime = 0.0;

	int					m_nLogicFrames = 0;
	double				m_dLogicLastTime = 0.0;
	double				m_dLogicFPS = 0.0;
	double				m_dLogicElapsedTime = 0.0;

	CAFC				m_AFCLibrary;
};

void AddActor(IActor *pActor);

#endif