#pragma once

#include "Resource.h"
#include "GraphicsRenderers\\BaseTexture2D.h"

class CTextureAdditionalData : public IAdditionalData
{
public:
	CTextureAdditionalData(XMFLOAT2 const &vSize) :
		m_vSize(vSize)
	{
	};

	virtual ~CTextureAdditionalData()
	{
	};

	XMFLOAT2 m_vSize;
};

// пускай пока мешкой будет
class CTextureResource : public IResource
{
public:
	CTextureResource(RESOURCE_ID nResourceID, bool bIsUnique);
	virtual ~CTextureResource();

	virtual void VLoadResource(CResourceLoadingData const &LoadingData) override;

	ITexture2D *GetTexture2D() const
	{
		return m_pTexture2D;
	};
private:
	ITexture2D *m_pTexture2D = nullptr;
};