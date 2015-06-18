#include "EnginePCH.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "..\\BaseMeshRenderer.h"

#include "Physics\\ShapeCreator.h"

void CD3D11MeshRenderer::VRender(ICamera const *const _pCamera, DWORD dwNumInstances) const
{
	static ID3D11ShaderResourceView *pSRVNull[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	auto cbidPerMesh = CD3D11GraphicsRenderer::GetInstance()->GetCBIDPerMesh();
	CBPerMesh *const pCBMesh = (CBPerMesh *)CShaders::GetCB(cbidPerMesh)->pData;

	pImmediateContext->IASetPrimitiveTopology(m_Topology);

	if (m_pVertexBuffer)
	{
		ID3D11Buffer *pVB[2] = { m_pVertexBuffer, ((CD3D11ModelRenderer *)m_pModelRenderer)->GetInstanceBuffer() };
		UINT strides[2] = { m_dwVertexSize, sizeof(XMMATRIX) };
		UINT offsets[2] = { 0, 0 };

		pImmediateContext->IASetVertexBuffers(0, 2, pVB, strides, offsets);
	}

	pCBMesh->bIsUseTexture = 0;

	if (m_pMaterial)
	{
		ITexture2D *pTex2D = nullptr;
		{
			IResource *pResource = CResourceManager::GetInstance()->GetResource(m_pMaterial->m_DiffuseTexture);

			if (pResource)
			{
				pTex2D = pResource->ToTextureResource()->GetTexture2D();
				if (pTex2D)
				{
					pTex2D->VBindTexture(0);
					pCBMesh->bIsUseTexture = 1;
				}
			}
		}

		pCBMesh->Material = m_pMaterial->m_CBMaterial;
	}

	CShaders::UpdateCB(cbidPerMesh);

	if (m_pIndexBuffer)
	{
		pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		pImmediateContext->DrawIndexedInstanced(m_nNumIndices, dwNumInstances, 0, 0, 0);
	}
	else if (m_dwNumVertices > 0)
	{
		// there is no index buffer

		pImmediateContext->DrawInstanced(m_dwNumVertices, dwNumInstances, 0, 0);
	}

	pImmediateContext->PSSetShaderResources(0, 1, pSRVNull);
}

void CD3D11MeshRenderer::VCreateMesh(CRawMesh const &RawMesh, CRawMesh const &PhysicsRawMesh)
{
	ID3D11Device *pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();

	if (!RawMesh.m_Vertices.empty())
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(CVertex) * RawMesh.m_Vertices.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));

		InitData.pSysMem = &RawMesh.m_Vertices.at(0);
		if (pD3DDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer) != S_OK)
		{
			CHError er = new CError_ObjectModelCreatingFailed(AT);

			return;
		}

		m_dwNumVertices = RawMesh.m_Vertices.size();
	}

	if (!RawMesh.m_Indices.empty())
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(CIndex) * RawMesh.m_Indices.size();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &RawMesh.m_Indices.at(0);
		if (pD3DDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer) != S_OK)
		{
			CHError er = new CError_ObjectModelCreatingFailed(AT);

			return;
		}

		m_nNumIndices = RawMesh.m_Indices.size();
		m_pMaterial = RawMesh.m_pMaterial;
	}

	switch (RawMesh.GetTopology())
	{
	case E_MESH_TOPOLOGY::POINTLIST:
		m_Topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case E_MESH_TOPOLOGY::TRIANGLELIST:
		m_Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case E_MESH_TOPOLOGY::TRIANGLESTRIP:
		m_Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	};

	m_sMeshName = RawMesh.m_sName;
	m_dwVertexSize = sizeof(CVertex);
	m_mLocalWorld = RawMesh.m_mLocalWorld;
	m_InitAABB = RawMesh.m_AABB;

	m_PhysXShape = ShapeCreator::CreateShape(PhysicsRawMesh);
};