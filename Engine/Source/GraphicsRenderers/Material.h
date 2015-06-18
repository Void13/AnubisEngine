#pragma once

#include "EnginePCH.h"

struct CBMaterial
{
	XMFLOAT3		vDiffuse;
	float			fOpacity;

	XMFLOAT3		vSpecular;
	float			fShininess;

	XMFLOAT3		vAmbient;
	float			fShininessStrength;

	XMFLOAT3		vEmissive;
	float			fRefraction;

	XMFLOAT3		vTransparent;
	int				bIsWireframe;
};

class CMaterial final
{
public:
	~CMaterial()
	{
	};

	CMaterial()
	{
	};

	std::string const &GetMaterialName() const
	{
		return m_sName;
	};

	void OnModelLoaded(IResource *pResource)
	{
		m_DiffuseTexture.m_dwUniqueResourceID = pResource->GetResourceID();
	};
public:
	std::string		m_sName;

	CBMaterial		m_CBMaterial;

	CResourceLoadingData	m_DiffuseTexture;
};

typedef std::vector<CMaterial *> MaterialsVector;