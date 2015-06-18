#ifndef H__LOGMANAGER
#define H__LOGMANAGER

#include <stdio.h>
#include <string>

#include <Windows.h>

#include "Singleton.h"

enum class E_LOGTYPE
{
	LOG_WARNING = 0,
	LOG_ERROR,
	LOG_INFO
};

enum class E_LOGDEST
{
	NONE = 0,
	STDERR = 1 << 1,
	DEBUG_WINDOW = 1 << 2,
	MSGBOX = 1 << 3,
	INFILE = 1 << 4,
	SEP_FILE = 1 << 5
	//SCREEN =		1 << 6
};

inline E_LOGDEST operator|(E_LOGDEST a, E_LOGDEST b)
{
	return static_cast<E_LOGDEST>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(E_LOGDEST a, E_LOGDEST b)
{
	return (static_cast<int>(a)& static_cast<int>(b)) != 0;
}

class CLogManager;

class CLogManager final : public CSingleton<CLogManager>
{
	friend class CSingleton<CLogManager>;
private:
	CLogManager();
	~CLogManager();

	void OpenFiles() const;
	void CloseFiles() const;
public:
	void Init(E_LOGDEST _eWarningDest, E_LOGDEST _eErrorDest, E_LOGDEST _eInfoDest);

	void AddMessage(E_LOGTYPE _eType, std::string const &sFormat, char const *pFile, int nLine, ...) const;
private:
	E_LOGDEST		m_eWarningDest = E_LOGDEST::NONE;
	E_LOGDEST		m_eErrorDest = E_LOGDEST::NONE;
	E_LOGDEST		m_eInfoDest = E_LOGDEST::NONE;

	mutable FILE		*m_pWarnFile = nullptr;
	mutable FILE		*m_pErrorFile = nullptr;
	mutable FILE		*m_pInfoFile = nullptr;

	mutable FILE		*m_pLogFile = nullptr;
};

#define LOG_MESSAGE(logtype, format, ...) { CLogManager::GetInstance()->AddMessage(logtype, format, __FILE__, __LINE__, __VA_ARGS__); }

#define LOG_WARN(format, ...) { CLogManager::GetInstance()->AddMessage(E_LOGTYPE::LOG_WARNING, format, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_INFO(format, ...) { CLogManager::GetInstance()->AddMessage(E_LOGTYPE::LOG_INFO, format, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_ERROR(format, ...) { CLogManager::GetInstance()->AddMessage(E_LOGTYPE::LOG_ERROR, format, __FILE__, __LINE__, __VA_ARGS__); }

#endif