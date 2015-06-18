#pragma once

#include "EnginePCH.h"
#include "..\\BaseMeshRenderer.h"
#include "..\\BaseModelRenderer.h"

#include "D3D11GraphicsRenderer.h"

class CD3D11MeshRenderer final : public IBaseMeshRenderer
{
public:
	CD3D11MeshRenderer(IBaseModelRenderer *pModelRenderer) :
		IBaseMeshRenderer(pModelRenderer)
	{
	};

	virtual ~CD3D11MeshRenderer()
	{
		ClearGeometry();
	};

	void ClearGeometry()
	{
		SAFE_RELEASE(m_pVertexBuffer);
		m_dwNumVertices = 0;

		SAFE_RELEASE(m_pVertexBuffer);
		m_dwNumVertices = 0;

		m_pMaterial = nullptr;
	};

	void VCreateMesh(CRawMesh const &RawMesh, CRawMesh const &PhysicsRawMesh) override;

	void VRender(ICamera const *const _pCamera, DWORD dwNumInstances) const override;
private:
	ID3D11Buffer					*m_pIndexBuffer = nullptr;
	DWORD							m_nNumIndices = 0;

	CMaterial						*m_pMaterial = nullptr;

	ID3D11Buffer					*m_pVertexBuffer = nullptr;
	DWORD							m_dwNumVertices = 0;

	D3D11_PRIMITIVE_TOPOLOGY		m_Topology;
};