#include "EnginePCH.h"
#include "BaseActor.h"
#include "ModelRenderer.h"

void CD3D11ModelRenderer::VAddMesh(CRawMesh const *pRawMesh, CRawMesh const *pPhysicsRawMesh)
{
	CD3D11MeshRenderer *pMesh = new CD3D11MeshRenderer(this);

	pMesh->VCreateMesh(*pRawMesh, *pPhysicsRawMesh);

	m_Meshes.push_back(pMesh);
	m_MeshNameToMesh.emplace(pMesh->GetName(), pMesh);
}

void CD3D11ModelRenderer::VRender(std::vector<IActor *> const &Actors, ICamera const *const _pCamera)
{
	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	for (size_t idMesh = 0; idMesh < m_Meshes.size(); idMesh++)
	{
		IBaseMeshRenderer const *pMesh = m_Meshes[idMesh];
		
		VCreateInstanceBuffer(Actors.size());

		D3D11_MAPPED_SUBRESOURCE MapRes;
		if (pImmediateContext->Map(m_pInstanceBuffer, D3D11CalcSubresource(0, 0, 1),
			D3D11_MAP_WRITE_DISCARD, 0, &MapRes) != S_OK)
		{
			CHError er = new CError_ResourceMapFailed(AT);
			return;
		}

		DWORD dwNumInstances = 0;

		for (IActor *pActor : Actors)
		{
			CObject *pObject = pActor->GetObjectByName(pMesh->GetName());
			
			if (pObject)
			{
				*((XMMATRIX *)MapRes.pData + dwNumInstances) = XMMatrixTranspose(pObject->GetGlobalRenderTransform());
			}
			else
			{
				// нужно пересобрать матрицу для отдельной мешки
				XMMATRIX mResult = pMesh->GetLocalWorld() * pActor->GetRenderTransform();

				// очень важная вещь. если нету объекта, значит мешка не связана с объектом, но рендерить её НАДО!!
				*((XMMATRIX *)MapRes.pData + dwNumInstances) = XMMatrixTranspose(mResult);
			}

			dwNumInstances++;
		}

		pImmediateContext->Unmap(m_pInstanceBuffer, D3D11CalcSubresource(0, 0, 1));

		if (dwNumInstances > 0)
		{
			pMesh->VRender(_pCamera, dwNumInstances);
		}
	}
}

void CD3D11ModelRenderer::VCreateInstanceBuffer(DWORD dwNumInstances)
{
	if (!dwNumInstances)
	{
		CHError er = new CError_Model_CreatingInstanceBufferFailed(AT);
		return;
	}

	m_dwNumInstances = dwNumInstances;

	if (dwNumInstances <= m_dwAllocatedInstances)
		return;

	if (m_dwAllocatedInstances == 0)
		m_dwAllocatedInstances = 1;

	while (m_dwAllocatedInstances < dwNumInstances)
		m_dwAllocatedInstances <<= 1;

	// recreating
	SAFE_RELEASE(m_pInstanceBuffer);

	auto pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(BufferDesc));

	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.ByteWidth = sizeof(XMMATRIX) * m_dwAllocatedInstances;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (pD3DDevice->CreateBuffer(&BufferDesc, nullptr, &m_pInstanceBuffer) != S_OK)
	{
		CHError er = new CError_Model_CreatingInstanceBufferFailed(AT);
		return;
	}
};