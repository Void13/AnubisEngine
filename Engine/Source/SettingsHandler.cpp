#include "EnginePCH.h"
#include "SettingsHandler.h"

void CGlobalSettings::Init(std::string const &sFile)
{
	std::string sTemp;

	INI<> ini(sFile, true);
	
	if ((sTemp = ini.get("Globals", "DataFolder", INI_ERROR)) != INI_ERROR)
		m_sDataFolder = sTemp;

	if ((sTemp = ini.get("Globals", "WindowWidth", INI_ERROR)) != INI_ERROR)
		m_nWindowWidth = StringHelper::StringToNumber<int>(sTemp);

	if ((sTemp = ini.get("Globals", "WindowHeight", INI_ERROR)) != INI_ERROR)
		m_nWindowHeight = StringHelper::StringToNumber<int>(sTemp);

	if ((sTemp = ini.get("Globals", "BackBufferWidth", INI_ERROR)) != INI_ERROR)
		m_nBackBufferWidth = StringHelper::StringToNumber<int>(sTemp);

	if ((sTemp = ini.get("Globals", "BackBufferHeight", INI_ERROR)) != INI_ERROR)
		m_nBackBufferHeight = StringHelper::StringToNumber<int>(sTemp);

	if ((sTemp = ini.get("Globals", "Windowed", INI_ERROR)) != INI_ERROR)
		m_bIsWindowed = (sTemp == "true");
};