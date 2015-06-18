#ifndef H__ERRORHANDLER
#define H__ERRORHANDLER

#include <string>

#include "EnginePCH.h"

#include "Singleton.h"
#include "LogManager.h"

#define AT __FILE__, __LINE__

#define ADD_ERROR(error) \
	class CError_##error final : public IBaseError \
	{ \
	public: \
		virtual ~CError_##error() \
		{ \
		}; \
		CError_##error(CError_##error const &) = delete; \
		CError_##error & operator=(CError_##error const &) = delete; \
		CError_##error(std::string const &sFile, int nLine) \
		{ \
			m_sFile = sFile; \
			m_nLine = nLine; \
			m_sErrorMsg = #error; \
		}; \
		IBaseError *VClone() const override \
		{ \
			return new CError_##error(m_sFile, m_nLine); \
		}; \
		std::string const &VGetMsg() const override \
		{ \
			return m_sErrorMsg; \
		}; \
		std::string const &VGetFile() const override \
		{ \
			return m_sFile; \
		}; \
		int VGetLine() const override \
		{ \
			return m_nLine; \
		}; \
	private: \
		std::string		m_sErrorMsg; \
		std::string		m_sFile; \
		int				m_nLine; \
	}; \

/*
class CGlobalIDKeeper : public CSingleton<CGlobalIDKeeper>
{
	friend class CSingleton<CGlobalIDKeeper>;
public:
	CGlobalIDKeeper()
	{
		m_nLastErrorID = 0;
	};

	int GetAndIncErrorID()
	{
		return m_nLastErrorID++;
	};
private:
	int m_nLastErrorID = 0;
};
*/

class IBaseError
{
public:
	virtual ~IBaseError()
	{
	};

	virtual IBaseError *VClone() const = 0;
	virtual std::string const &VGetMsg() const = 0;
	virtual std::string const &VGetFile() const = 0;
	virtual int VGetLine() const = 0;
};

#include "ErrorList.h"

class CError_OK final : public IBaseError
{
public:
	CError_OK(CError_OK const &) = delete;
	CError_OK& operator=(CError_OK const &) = delete;

	CError_OK(std::string const &sFile, int nLine)
	{
		m_sFile = sFile;
		m_nLine = nLine;
		m_sErrorMsg = "";
	};
	
	virtual ~CError_OK()
	{
	}

	IBaseError *VClone() const override
	{
		return new CError_OK(m_sFile, m_nLine);
	};

	std::string const &VGetMsg() const override
	{
		return m_sErrorMsg;
	};

	std::string const &VGetFile() const override
	{
		return m_sFile;
	};

	int VGetLine() const override
	{
		return m_nLine;
	};
private:
	std::string		m_sErrorMsg;
	std::string		m_sFile;
	int				m_nLine;
};

class CHError
{
public:
	CHError(IBaseError *pError)
	{
		if (!pError)
			return;

		SAFE_DELETE(m_pError);
		m_pError = pError;

		if (!IsOK())
		{
			CLogManager::GetInstance()->AddMessage(E_LOGTYPE::LOG_ERROR, pError->VGetMsg(), pError->VGetFile().c_str(), pError->VGetLine());

			DEBUGBRAKE();
		}
	};

	CHError(CHError &hError)
	{
		if (this != &hError)
		{
			SAFE_DELETE(m_pError);
			m_pError = hError.m_pError->VClone();
		}
	};

	CHError(CHError &&hError)
	{
		if (this != &hError)
		{
			m_pError = hError.m_pError;
			hError.m_pError = nullptr;
		}
	};

	~CHError()
	{
		SAFE_DELETE(m_pError);
	};

	bool IsOK() const
	{
		return m_pError->VGetMsg().empty();
	};
private:
	IBaseError *m_pError = nullptr;
};

#endif