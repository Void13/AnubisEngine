#pragma once

#include "EnginePCH.h"

enum class EPixelFormat
{
	BC1,
	BC2,
	BC3
};

struct CTextureInitInfo
{
	// init data
	void *pData;
	size_t dwDataSize;

	std::string sName;
	
	size_t nWidth;
	size_t nHeight;

	EPixelFormat ePixelFormat;
};

class ITexture2D
{
public:
	virtual ~ITexture2D()
	{
	};

	virtual void VInitTexture(CTextureInitInfo const &InitInfo) = 0;
	virtual void VBindTexture(size_t dwSlot) = 0;

	virtual void VMapTexture(void *&pData) = 0;
	virtual void VUnmapTexture() = 0;
protected:
	size_t			m_nWidth;
	size_t			m_nHeight;

	EPixelFormat	m_ePixelFormat;

	std::string		m_sName;
};

class CMapTexture
{
	CMapTexture(ITexture2D *pTexture, void *&pData) :
		m_pTexture(pTexture)
	{
		m_pTexture->VMapTexture(pData);
	};

	~CMapTexture()
	{
		m_pTexture->VUnmapTexture();
	};
private:
	ITexture2D *m_pTexture;
};