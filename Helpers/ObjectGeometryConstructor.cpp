#include "EnginePCH.h"

#include "ObjectGeometryConstructor.h"
#include "GraphicsRenderers//BaseMeshRenderer.h"

void ContructBoxVertexBuffer(CRawModel *pRawModel)
{
	CRawMesh *pRawMesh = new CRawMesh(pRawModel, E_MESH_TYPE::BOX, E_MESH_TOPOLOGY::TRIANGLESTRIP, pRawModel->m_eRidigBodyFlag);

	const DWORD	dwAABBNumVertices = 19;

	pRawMesh->m_Vertices.resize(dwAABBNumVertices);

	// to triangle strips
	pRawMesh->m_Vertices[0].vPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	pRawMesh->m_Vertices[1].vPosition = XMFLOAT3(-1.0f, 1.0f, -1.0f);
	pRawMesh->m_Vertices[2].vPosition = XMFLOAT3(1.0f, -1.0f, -1.0f);
	pRawMesh->m_Vertices[3].vPosition = XMFLOAT3(1.0f, 1.0f, -1.0f);
	pRawMesh->m_Vertices[4].vPosition = XMFLOAT3(1.0f, -1.0f, 1.0f);
	pRawMesh->m_Vertices[5].vPosition = XMFLOAT3(1.0f, 1.0f, 1.0f);
	pRawMesh->m_Vertices[6].vPosition = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	pRawMesh->m_Vertices[7].vPosition = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	pRawMesh->m_Vertices[8].vPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	pRawMesh->m_Vertices[9].vPosition = XMFLOAT3(-1.0f, 1.0f, -1.0f);

	pRawMesh->m_Vertices[10].vPosition = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	pRawMesh->m_Vertices[11].vPosition = XMFLOAT3(1.0f, 1.0f, -1.0f);
	pRawMesh->m_Vertices[12].vPosition = XMFLOAT3(1.0f, 1.0f, 1.0f);

	pRawMesh->m_Vertices[13].vPosition = XMFLOAT3(1.0f, 1.0f, 1.0f);
	pRawMesh->m_Vertices[14].vPosition = XMFLOAT3(-1.0f, -1.0f, 1.0f);

	pRawMesh->m_Vertices[15].vPosition = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	pRawMesh->m_Vertices[16].vPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	pRawMesh->m_Vertices[17].vPosition = XMFLOAT3(1.0f, -1.0f, 1.0f);
	pRawMesh->m_Vertices[18].vPosition = XMFLOAT3(1.0f, -1.0f, -1.0f);

	for (size_t i = 0; i < dwAABBNumVertices; i++)
	{
		pRawMesh->m_Vertices[i].vPosition.x *= pRawModel->m_vScale.x;
		pRawMesh->m_Vertices[i].vPosition.y *= pRawModel->m_vScale.y;
		pRawMesh->m_Vertices[i].vPosition.z *= pRawModel->m_vScale.z;
	}

	pRawModel->m_RawMeshes.push_back(pRawMesh);
}

void ContructSphereVertexBuffer(CRawModel *pRawModel)
{
	CRawMesh *pRawMesh = new CRawMesh(pRawModel, E_MESH_TYPE::SPHERE, E_MESH_TOPOLOGY::TRIANGLESTRIP, pRawModel->m_eRidigBodyFlag);

	float fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pRawModel->m_vScale)));

	const DWORD dwSphereNumVertices = 64 * 32;

	int i = 0,
		nNumSlices = 32,
		nNumStacks = (dwSphereNumVertices / nNumSlices);
	nNumStacks = nNumStacks / 2 * 2;
	//dwSphereNumVertices = nNumStacks * nNumSlices;

	float
		fTheta = 0.0f,
		fPhi = 0.0f;

	for (int nStack = 0; nStack < nNumStacks; nStack++)
	{
		fPhi = 0.0f;

		for (int nSlice = 0; nSlice < nNumSlices; nSlice++)
		{
			CVertex v;

			v.vPosition = XMFLOAT3(
				cosf(fPhi) * sinf(fTheta) * fRadius,
				cosf(fTheta) * fRadius,
				sinf(fPhi) * sinf(fTheta) * fRadius
				);

			fPhi += XM_PI / nNumSlices * 2.0f;

			if (nSlice % 2)
				fTheta += XM_PI / nNumStacks * 2.0f;
			else
				fTheta -= XM_PI / nNumStacks * 2.0f;

			pRawMesh->m_Vertices.push_back(v);
		}

		fTheta += XM_PI / nNumStacks * 2.0f;
	}

	pRawModel->m_RawMeshes.push_back(pRawMesh);
}

void ContructRegularGrid(XMINT2 const &_vGeometryDetail, CRawModel *pRawModel)
{
	CRawMesh *pRawMesh = new CRawMesh(pRawModel, E_MESH_TYPE::PLANE, E_MESH_TOPOLOGY::TRIANGLESTRIP, pRawModel->m_eRidigBodyFlag);

	int nSlicesX = _vGeometryDetail.x,
		nSlicesZ = _vGeometryDetail.y;

	nSlicesX = MathHelper::Clamp(nSlicesX, 2, 500);
	nSlicesZ = MathHelper::Clamp(nSlicesZ, 2, 500);

	for (int i = 0; i <= nSlicesZ; i++)
	{
		for (int j = 0; j <= nSlicesX; j++)
		{
			CVertex v;

			v.vPosition.y = 0.0f;
			v.vPosition.z = (float)i / nSlicesZ - 0.5f;
			v.vPosition.x = (float)j / nSlicesX - 0.5f;
			v.vPosition.x *= pRawModel->m_vScale.x;
			v.vPosition.z *= pRawModel->m_vScale.y;

			v.vTexCoords.x = (float)i / nSlicesZ;
			v.vTexCoords.y = (1.0f - (float)j / nSlicesX);

			v.vNormal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			pRawMesh->m_Vertices.push_back(v);
		}
	}
	
	for (int i = 0; i < nSlicesZ; i++)
	{
		pRawMesh->m_Indices.push_back((nSlicesX + 1) * i + 0);

		for (int j = 0; j <= nSlicesX; j++)
		{
			pRawMesh->m_Indices.push_back((nSlicesX + 1) * i + j);
			pRawMesh->m_Indices.push_back((nSlicesX + 1) * (i + 1) + j);
		}

		pRawMesh->m_Indices.push_back((nSlicesX + 1) * (i + 1) + nSlicesX);
	}

	pRawModel->m_RawMeshes.push_back(pRawMesh);
}