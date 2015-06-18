#include "LogManager.h"
#include "Strings.h"

#include "EnginePCH.h"

char *g_pBuffer = nullptr;

void CLogManager::Init(E_LOGDEST _eWarningDest, E_LOGDEST _eErrorDest, E_LOGDEST _eInfoDest)
{
	m_eWarningDest = _eWarningDest;
	m_eErrorDest = _eErrorDest;
	m_eInfoDest = _eInfoDest;

	OpenFiles();

	LOG_INFO("LogManager: Initialized");
}

CLogManager::CLogManager()
{
	g_pBuffer = new char[1024 * 4];
}

CLogManager::~CLogManager()
{
	SAFE_DELETE_ARRAY(g_pBuffer);
	CloseFiles();
}

void CLogManager::OpenFiles() const
{
	if (m_eWarningDest & E_LOGDEST::INFILE && !m_pLogFile)
	{
		fopen_s(&m_pLogFile, "Logs.txt", "a");
	}
	else if (m_eWarningDest & E_LOGDEST::SEP_FILE)
	{
		fopen_s(&m_pWarnFile, "WarningsLog.txt", "a");
	}

	if (m_eErrorDest & E_LOGDEST::INFILE && !m_pLogFile)
	{
		fopen_s(&m_pLogFile, "Logs.txt", "a");
	}
	else if (m_eErrorDest & E_LOGDEST::SEP_FILE)
	{
		fopen_s(&m_pErrorFile, "ErrorsLog.txt", "a");
	}

	if (m_eInfoDest & E_LOGDEST::INFILE && !m_pLogFile)
	{
		fopen_s(&m_pLogFile, "Logs.txt", "a");
	}
	else if (m_eInfoDest & E_LOGDEST::SEP_FILE)
	{
		fopen_s(&m_pInfoFile, "InfoLog.txt", "a");
	}
}

void CLogManager::CloseFiles() const
{
	if (m_pInfoFile)
	{
		fclose(m_pInfoFile);
		m_pInfoFile = nullptr;
	}

	if (m_pLogFile)
	{
		fclose(m_pLogFile);
		m_pLogFile = nullptr;
	}

	if (m_pErrorFile)
	{
		fclose(m_pErrorFile);
		m_pErrorFile = nullptr;
	}

	if (m_pWarnFile)
	{
		fclose(m_pWarnFile);
		m_pWarnFile = nullptr;
	}
}

void CLogManager::AddMessage(E_LOGTYPE _eType, std::string const &sFormat, char const *pFile, int nLine, ...) const
{
	// vfprintf(stderr, format, argptr);

	std::string sFile = pFile;
	auto Splitted = StringHelper::Split(sFile, "\\");

	auto now = std::time(nullptr);
	std::shared_ptr<tm> pLocalTime(new tm);
	localtime_s(pLocalTime.get(), &now);

	sprintf_s(g_pBuffer, 1024 * 4, "[%02d:%02d:%02d] File: %s\\%s(%d) | ", 
		pLocalTime->tm_hour, pLocalTime->tm_min, pLocalTime->tm_sec,
		Splitted[Splitted.size() - 2].c_str(), 
		Splitted[Splitted.size() - 1].c_str(), 
		nLine);
	std::string sFormattedFormat = std::string(g_pBuffer) + sFormat + std::string("\n");

	va_list argptr;
	va_start(argptr, nLine);
	int n = vsnprintf_s(g_pBuffer, 1024 * 4, 1024 * 4, sFormattedFormat.c_str(), argptr);
	va_end(argptr);

	if (_eType == E_LOGTYPE::LOG_ERROR)
	{
		if (m_eErrorDest & E_LOGDEST::DEBUG_WINDOW)
		{
			OutputDebugStringA(g_pBuffer);
		}
		if (m_eErrorDest & E_LOGDEST::SEP_FILE)
		{
			fputs(g_pBuffer, m_pErrorFile);
		}
		if (m_eErrorDest & E_LOGDEST::INFILE)
		{
			fputs(g_pBuffer, m_pLogFile);
		}
		if (m_eErrorDest & E_LOGDEST::MSGBOX)
		{
			MessageBoxA(0, g_pBuffer, "Error!", MB_OK);
		}
		if (m_eErrorDest & E_LOGDEST::STDERR)
		{
			fputs(g_pBuffer, stderr);
		}
	}
	else if (_eType == E_LOGTYPE::LOG_WARNING)
	{
		if (m_eWarningDest & E_LOGDEST::DEBUG_WINDOW)
		{
			OutputDebugStringA(g_pBuffer);
		}
		if (m_eWarningDest & E_LOGDEST::SEP_FILE)
		{
			fputs(g_pBuffer, m_pWarnFile);
		}
		if (m_eWarningDest & E_LOGDEST::INFILE)
		{
			fputs(g_pBuffer, m_pLogFile);
		}
		if (m_eWarningDest & E_LOGDEST::MSGBOX)
		{
			MessageBoxA(0, g_pBuffer, "Warning!", MB_OK);
		}
		if (m_eWarningDest & E_LOGDEST::STDERR)
		{
			fputs(g_pBuffer, stderr);
		}
	}
	else if (_eType == E_LOGTYPE::LOG_INFO)
	{
		if (m_eInfoDest & E_LOGDEST::DEBUG_WINDOW)
		{
			OutputDebugStringA(g_pBuffer);
		}
		if (m_eInfoDest & E_LOGDEST::SEP_FILE)
		{
			fputs(g_pBuffer, m_pInfoFile);
		}
		if (m_eInfoDest & E_LOGDEST::INFILE)
		{
			fputs(g_pBuffer, m_pLogFile);
		}
		if (m_eInfoDest & E_LOGDEST::MSGBOX)
		{
			MessageBoxA(0, g_pBuffer, "Info!", MB_OK);
		}
		if (m_eInfoDest & E_LOGDEST::STDERR)
		{
			fputs(g_pBuffer, stderr);
		}
	}

	CloseFiles();
	OpenFiles();
}