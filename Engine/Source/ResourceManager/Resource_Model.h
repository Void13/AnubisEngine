#pragma once

#include "Resource.h"
#include "GraphicsRenderers\\BaseModelRenderer.h"

class CModelAdditionalData : public IAdditionalData
{
public:
	CModelAdditionalData(XMFLOAT3 const &vScale) :
		m_vScale(vScale)
	{
	};

	CModelAdditionalData(float fScale) :
		m_vScale(fScale, fScale, fScale)
	{
	};

	virtual ~CModelAdditionalData()
	{
	};

	XMFLOAT3 m_vScale;
};

// пускай пока мешкой будет
class CModelResource : public IResource
{
public:
	CModelResource(RESOURCE_ID nResourceID, bool bIsUnique);
	virtual ~CModelResource();

	virtual void VLoadResource(CResourceLoadingData const &LoadingData) override;

	IBaseModelRenderer *GetModelRenderer() const
	{
		return pModelRenderer;
	};
private:
	IBaseModelRenderer *pModelRenderer = nullptr;
};