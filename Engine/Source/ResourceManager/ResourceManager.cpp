#include "EnginePCH.h"

#include "ResourceManager.h"

#include "Resource.h"
#include "Resource_Model.h"
#include "Resource_Texture.h"

void CResourceManager::ReadResource(CAFC const *pAFCLibrary, std::string const &sFileName, char *&pData, size_t &dwDataSize)
{
	if (pAFCLibrary)
	{
		size_t nFileSize;
		void *pFileData = nullptr;

		EAFCErrors Error;
		if ((Error = pAFCLibrary->ReadFile(sFileName.c_str(), pFileData, nFileSize)) != EAFCErrors::OK)
		{
			LOG_ERROR("Resource Manager %s", CAFC::GetErrorString(Error).c_str());
			CHError er = new CError_ResourceManager(AT);

			return;
		}

		pData = (char *)pFileData;
		dwDataSize = nFileSize;
	}
	// если нет - грузим с обычного файла
	else
	{
		size_t nFileSize;
		void *pFileData = nullptr;

		GetFileData(sFileName, pFileData, nFileSize);

		if (!pData)
		{
			CHError er = new CError_ResourceManager(AT);

			return;
		}

		pData = (char *)pFileData;
		dwDataSize = nFileSize;
	}
};

void CResourceManager::PreloadResource(CResourceLoadingData const &ResourceLoadingData)
{
	if (ResourceLoadingData.m_sFileName.empty())
		return;

	if (IsResourceLoaded(ResourceLoadingData))
	{
		return;
	}

	m_LoadingResourcesWrites = true;
	if (m_LoadingResources1Busy)
	{
		m_LoadingResources2.push_back(ResourceLoadingData);
	}
	else
	{
		m_LoadingResources1.push_back(ResourceLoadingData);
	}
	m_LoadingResourcesWrites = false;
}

IResource *CResourceManager::GetResource(CResourceLoadingData const &ResourceLoadingData)
{
	if (ResourceLoadingData.m_sFileName.empty())
		return nullptr;

	IResource *pResource = GetLoadedResourcePtr(ResourceLoadingData);

	// если есть - возвращаем его
	if (pResource)
		return pResource;

	// нету - загружаем немедленно!
	pResource = CreateResource(ResourceLoadingData);

	return pResource;
}

std::string CResourceManager::GetResourceTypeFolder(E_RESOURCE_TYPE eResourceType) const
{
	std::string sDataFolder = CSettingsHandler::GetDataFolder();

	switch (eResourceType)
	{
	case E_RESOURCE_TYPE::MODEL:
		sDataFolder = sDataFolder + "modelsdef\\";

		break;
	case E_RESOURCE_TYPE::TEXTURE:
		sDataFolder = sDataFolder + "models\\";
		//sDataFolder = sDataFolder + "textures\\";

		break;
	};

	return sDataFolder;
}

IResource *CResourceManager::CreateResource(CResourceLoadingData const &ResourceLoadingData)
{
	IResource *pResource;

	switch (ResourceLoadingData.m_eResourceType)
	{
	case E_RESOURCE_TYPE::MODEL:
		pResource = new CModelResource(m_LoadedResources.size(), ResourceLoadingData.m_bIsUnique);
		break;
	case E_RESOURCE_TYPE::TEXTURE:
		pResource = new CTextureResource(m_LoadedResources.size(), ResourceLoadingData.m_bIsUnique);
		break;
	};

	pResource->VLoadResource(ResourceLoadingData);

	if (ResourceLoadingData.m_OnResourceLoaded != -1)
	{
		CEventManager::GetInstance()->FastCallEvent(ResourceLoadingData.m_OnResourceLoaded, pResource);
	}

	m_LoadedResources.push_back(pResource);
	m_LoadedNameToResource.emplace(ResourceLoadingData.m_sFileName, pResource);

	return pResource;
};

IResource *CResourceManager::GetLoadedResourcePtr(CResourceLoadingData const &ResourceLoadingData)
{
	// не нашли в списке идов
	if (ResourceLoadingData.m_dwUniqueResourceID == -1 || 
		ResourceLoadingData.m_dwUniqueResourceID >= m_LoadedResources.size())
	{
		// если не уникальный - ищем в списке с именами
		if (!ResourceLoadingData.m_bIsUnique)
		{
			// нашли ресурс
			auto it = m_LoadedNameToResource.find(ResourceLoadingData.m_sFileName);
			if (it != m_LoadedNameToResource.end())
			{
				return it->second;
			}
		}

		// не нашли(или уникальный нужен)
		return nullptr;
	}
	
	return m_LoadedResources[ResourceLoadingData.m_dwUniqueResourceID];
}

bool CResourceManager::IsResourceLoaded(CResourceLoadingData const &ResourceLoadingData)
{
	return GetLoadedResourcePtr(ResourceLoadingData) != nullptr;
}

void CResourceManager::StartThread()
{
	m_bIsInit = true;

	m_Thread = std::thread(&CResourceManager::VRunThread, this);

	LOG_INFO("CResourceManager: Thread has been started");
}

void CResourceManager::Terminate()
{
	m_bIsInit = false;

	if (m_Thread.joinable())
		m_Thread.join();

	LOG_INFO("CResourceManager: Thread has been started");

	for (auto pResource : m_LoadedResources)
	{
		SAFE_DELETE(pResource);
	}

	DestroyInstance();
}

void CResourceManagerWorkedThread::VRunThread()
{
	while (m_bIsInit)
	{
		// work with resources
	}
}