#pragma once

#include "ResourceManager.h"

class IAdditionalData
{
public:
	virtual ~IAdditionalData()
	{
	};
};

/*

	Класс базового ресурса.

*/

const DWORD g_dwAddTicks = 2;

class IResource
{
public:
	IResource(RESOURCE_ID nResourceID, bool bIsUnique) :
		m_nResourceID(nResourceID), m_bIsUnique(bIsUnique)
	{
	};

	virtual ~IResource()
	{
	};

	virtual void VLoadResource(CResourceLoadingData const &LoadingData) = 0;

	void AddTicks()
	{
		m_dwTicksToUnloading += g_dwAddTicks;
	};

	E_RESOURCE_TYPE GetResourceType() const
	{
		return m_eResourceType;
	};

	CModelResource *ToModelResource()
	{
		if (m_eResourceType == E_RESOURCE_TYPE::MODEL)
		{
			return (CModelResource *)this;
		}

		return nullptr;
	};

	CTextureResource *ToTextureResource()
	{
		if (m_eResourceType == E_RESOURCE_TYPE::TEXTURE)
		{
			return (CTextureResource *)this;
		}

		return nullptr;
	};

	RESOURCE_ID GetResourceID() const
	{
		return m_nResourceID;
	};
protected:
	bool			m_bIsInfinite = false;
	bool			m_bIsUnique = false;

	DWORD			m_dwTicksToUnloading = 0;

	E_RESOURCE_TYPE	m_eResourceType;

	RESOURCE_ID		m_nResourceID = -1;
};