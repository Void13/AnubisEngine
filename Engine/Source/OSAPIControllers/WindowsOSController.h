#ifndef H__WINDOWSOSAPICONTR
#define H__WINDOWSOSAPICONTR

#include "EnginePCH.h"

#include "BaseOSAPIController.h"

class CWindowsOSController final : 
	public IBaseOSAPIController,
	public CSingleton<CWindowsOSController>
{
	friend class CSingleton<CWindowsOSController>;
private:
	CWindowsOSController()
	{
	};
public:
	virtual ~CWindowsOSController();

	virtual CHError VCreateWindow() override;
	virtual bool VProcessMessages() const override;

	HWND GetHWND() const
	{
		return m_hWnd;
	};

	HINSTANCE GetHInstance() const
	{
		return m_hInstance;
	};
private:
	std::unique_ptr<WNDCLASS>	m_pWndClass = nullptr;
	HWND						m_hWnd = 0;
	HINSTANCE					m_hInstance = 0;

	bool						m_bIsWindowCreated = false;
	bool						m_bIsActive = false;
};

#endif