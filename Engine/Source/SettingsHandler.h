#ifndef H__SETTINGSHANDLER
#define H__SETTINGSHANDLER

#include "EnginePCH.h"

class CSettingsHandler;
class CDistributedObjectCreator;

struct CGlobalSettings
{
	friend class CSettingsHandler;
	friend class CDistributedObjectCreator;

public:
	CGlobalSettings()
	{
	};

	void Init(std::string const &sFile);

	enum class E_OSAPI
	{
		WINDOWS = 0
	};

	enum class E_GAPI
	{
		D3D11 = 0
	};

public:
	// default values

	int m_nWindowWidth = 1024;
	int m_nWindowHeight = 768;

	int m_nBackBufferWidth = 1024;
	int m_nBackBufferHeight = 768;

	bool m_bIsWindowed = true;

	E_OSAPI m_eOSAPI = E_OSAPI::WINDOWS;
	E_GAPI m_eGAPI = E_GAPI::D3D11;

	// data folder
	std::string m_sDataFolder = "data\\";
};

class CSettingsHandler final : public CSingleton<CSettingsHandler>
{
	friend class CDistributedObjectCreator;
	friend class CSingleton<CSettingsHandler>;
private:
	CSettingsHandler()
	{
		m_pSettings = new CGlobalSettings;
	};

	static CGlobalSettings::E_OSAPI GetOSAPI()
	{
		return GetInstance()->m_pSettings->m_eOSAPI;
	};

	static CGlobalSettings::E_GAPI GetGAPI()
	{
		return GetInstance()->m_pSettings->m_eGAPI;
	};
public:
	static void Init(CGlobalSettings const &Settings)
	{
		*GetInstance()->m_pSettings = Settings;
	};

	static int GetWindowWidth()
	{
		return GetInstance()->m_pSettings->m_nWindowWidth;
	};

	static int GetWindowHeight()
	{
		return GetInstance()->m_pSettings->m_nWindowHeight;
	};

	static void SetWindowWidth(int nWidth)
	{
		GetInstance()->m_pSettings->m_nWindowWidth = nWidth;
	};

	static void SetWindowHeight(int nHeight)
	{
		GetInstance()->m_pSettings->m_nWindowHeight = nHeight;
	};

	static int GetBackBufferWidth()
	{
		return GetInstance()->m_pSettings->m_nBackBufferWidth;
	};

	static int GetBackBufferHeight()
	{
		return GetInstance()->m_pSettings->m_nBackBufferHeight;
	};

	static void GetBackBufferWidth(int nWidth)
	{
		GetInstance()->m_pSettings->m_nBackBufferWidth = nWidth;
	};

	static void GetBackBufferHeight(int nHeight)
	{
		GetInstance()->m_pSettings->m_nBackBufferHeight = nHeight;
	};

	static bool IsWindowed()
	{
		return GetInstance()->m_pSettings->m_bIsWindowed;
	};

	static void SetWindowed(bool bWindowed)
	{
		GetInstance()->m_pSettings->m_bIsWindowed = bWindowed;
	};

	static std::string const &GetDataFolder()
	{
		return GetInstance()->m_pSettings->m_sDataFolder;
	};
private:
	CGlobalSettings *m_pSettings;
};

#endif