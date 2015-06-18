#pragma once

#include "EnginePCH.h"

class IResource;
class IAdditionalData;
class CResourceManagerWorkedThread;
class CAFC;
class CResourceManager;

class CModelResource;
class CTextureResource;

typedef size_t RESOURCE_ID;

enum class E_RESOURCE_TYPE
{
	TEXTURE = 0,
	MODEL
};

enum class E_RESOURCE_PRIORITY : DWORD
{
	NONE = 0,
	LOW,
	MEDIUM,
	IMMEDIATELY
};

struct CResourceLoadingData
{
	CResourceLoadingData()
	{
	};

	// когда ресурс будет загружен - вызовется эта хрень.
	TIMERID					m_OnResourceLoaded = -1;
	// ид ресурса. если ида нету - значит нужно его вернуть
	size_t					m_dwUniqueResourceID = -1;

	std::string				m_sFileName;
	E_RESOURCE_TYPE			m_eResourceType;
	E_RESOURCE_PRIORITY		m_ePriority;
	bool					m_bIsInfinite = false;
	bool					m_bIsUnique = false;
	
	CAFC 					const *m_pAFCLibrary = nullptr;
	IAdditionalData			const *m_pAddData = nullptr;
};

class CResourceManager : public CSingleton<CResourceManagerWorkedThread>
{
	friend class CSingleton<CResourceManagerWorkedThread>;
protected:
	CResourceManager()
	{
	};

	virtual ~CResourceManager()
	{
	};

	CResourceManager(CResourceManager const &) = delete;
	CResourceManager& operator=(CResourceManager const &) = delete;

	virtual void VRunThread() = 0;
public:
	void StartThread();
	void Terminate();

	void ReadResource(CAFC const *pAFCLibrary, std::string const &sFileName, char *&pData, size_t &dwDataSize);

	void PreloadResource(CResourceLoadingData const &ResourceLoadingData);
	IResource *GetResource(CResourceLoadingData const &ResourceLoadingData);

	IResource *GetLoadedResourcePtr(CResourceLoadingData const &ResourceLoadingData);
	bool IsResourceLoaded(CResourceLoadingData const &ResourceLoadingData);
protected:
	IResource *CreateResource(CResourceLoadingData const &ResourceLoadingData);

	std::string GetResourceTypeFolder(E_RESOURCE_TYPE eResourceType) const;

	typedef std::vector<IResource *> RESOURCE_VECTOR;
	typedef std::vector<CResourceLoadingData> PRELOAD_RESOURCE_VECTOR;

	std::unordered_map<std::string, IResource *> m_LoadedNameToResource;
	RESOURCE_VECTOR			m_LoadedResources;

	// sector of preloading resources
	PRELOAD_RESOURCE_VECTOR	m_LoadingResources1;
	PRELOAD_RESOURCE_VECTOR	m_LoadingResources2;
	std::atomic<bool>		m_LoadingResourcesWrites = false;
	std::atomic<bool>		m_LoadingResources1Busy = false;

	std::atomic<bool>		m_bIsInit = false;
	std::thread				m_Thread;
};

class CResourceManagerWorkedThread final : public CResourceManager
{
public:
	virtual ~CResourceManagerWorkedThread()
	{
	};
private:
	void VRunThread() override;
};

/*

	3. 
		чтобы для тех ресурсов, которые можно выгружать, он просчитывал максимально эффективно в плане памяти тот момент, 
		что какой-то ресурс надо выгрузить, а какой-то лучше не выгружать, т.к. он скоро будет востребован ещё раз.

		пока что, при GetResource() будет добавляться к переменной
		если переменная == 0, то выгружаем ресурс
		в каждом тике ресурс менеджера будут у всех отниматься по единичке от переменой и браться все нулевые ресурсы, затем их отправляем на выгрузку

	8. 
		отправка на выгрузку: UnloadResource(вообще для всех ресурсов). нужно учитывать, что выгрузка может происходить через принудительную выгрузку и через выгрузку по истечению срока "лежания" в памяти.

*/