#pragma once

#include "EnginePCH.h"
#include "..\\BaseMeshRenderer.h"
#include "..\\BaseModelRenderer.h"

#include "MeshRenderer.h"
#include "D3D11GraphicsRenderer.h"

class CD3D11ModelRenderer final : public IBaseModelRenderer
{
public:
	CD3D11ModelRenderer() :
		IBaseModelRenderer()
	{
	};

	virtual ~CD3D11ModelRenderer()
	{
		ClearGeometry();

		for (auto pMesh : m_Meshes)
		{
			SAFE_DELETE(pMesh);
		}
	};

	void ClearGeometry()
	{
		for (auto pMesh : m_Meshes)
		{
			auto pD3D11Mesh = (CD3D11MeshRenderer *)pMesh;

			pD3D11Mesh->ClearGeometry();
		};

		SAFE_RELEASE(m_pInstanceBuffer);
	};

	void VAddMesh(CRawMesh const *pRawMesh, CRawMesh const *pPhysicsRawMesh) override;

	void VRender(std::vector<IActor *> const &Actors, ICamera const *const _pCamera) override;

	ID3D11Buffer *GetInstanceBuffer() const
	{
		return m_pInstanceBuffer;
	};
private:
	virtual void VCreateInstanceBuffer(DWORD dwNumInstances) override;
private:
	ID3D11Buffer	*m_pInstanceBuffer = nullptr;
};