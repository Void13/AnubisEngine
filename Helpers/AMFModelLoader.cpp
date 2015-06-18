#include "EnginePCH.h"
#include "AMFModelLoader.h"

#include "GraphicsRenderers\\BaseMeshRenderer.h"
#include "GraphicsRenderers\\BaseModelRenderer.h"

template<class T>
T const ReadData(char const *&pTmpFileContent)
{
	T Data;

	Data = *((T *)pTmpFileContent);

	pTmpFileContent += sizeof(T);

	return Data;
};

template<>
std::string const ReadData<std::string>(char const *&pTmpFileContent)
{
	std::string Data(pTmpFileContent);

	pTmpFileContent += Data.size() + 1;

	return Data;
};

void LoadAMFModel(CRawModel *pRawModel, CAFC const *pAFCLibrary)
{
	char *pLoadedData = nullptr;
	size_t nDataSize = 0;

	CResourceManager::GetInstance()->ReadResource(pAFCLibrary, pRawModel->m_sModelName, pLoadedData, nDataSize);

	char const *pData = (char *)pLoadedData;

	enum class ESection : unsigned int
	{
		MODELINFO = 0,
		MATERIAL,
		MESH,
		VERTICES,
		INDICES
	};

	enum class EModelType : DWORD
	{
		NONE = 0,
		GENERIC,
		LA,
		SF,
		VEGAS,
		COUNTRY,
		LEVELDES,
		INTERIOR
	};

	struct CSectionHeader
	{
		ESection nID;
		int nSize;
	};

	CSectionHeader Section;

	Section = ReadData<CSectionHeader>(pData);

	if (Section.nID != ESection::MODELINFO)
	{
		CHError er = new CError_ModelLoadingError(AT);
		return;
	}

	DWORD dwNumMaterials = ReadData<DWORD>(pData);
	DWORD dwNumFaces = ReadData<DWORD>(pData);
	DWORD dwNumMeshes = ReadData<DWORD>(pData);

	CAABB aabb;
	aabb.SetMinMax(ReadData<XMFLOAT3>(pData), ReadData<XMFLOAT3>(pData));

	CBSphere bsphere;
	bsphere.SetPosition(ReadData<XMFLOAT3>(pData));
	bsphere.SetRadius(ReadData<float>(pData));

	EModelType eModelType = ReadData<EModelType>(pData);
	
	float fLODDistance = ReadData<float>(pData);
	pRawModel->m_fLODDistance = fLODDistance;

	std::string sModelName = ReadData<std::string>(pData);

	//pModel->m_nIndices = dwNumFaces * 3;

	for (DWORD i = 0; i < dwNumMaterials; i++)
	{
		Section = ReadData<CSectionHeader>(pData);
		if (Section.nID != ESection::MATERIAL)
		{
			CHError er = new CError_ModelLoadingError(AT);
			return;
		}

		CMaterial *pMaterial = new CMaterial;

		pMaterial->m_CBMaterial = ReadData<CBMaterial>(pData);
		pMaterial->m_DiffuseTexture.m_sFileName = ReadData<std::string>(pData);
		pMaterial->m_DiffuseTexture.m_bIsInfinite = true;
		pMaterial->m_DiffuseTexture.m_ePriority = E_RESOURCE_PRIORITY::MEDIUM;
		pMaterial->m_DiffuseTexture.m_eResourceType = E_RESOURCE_TYPE::TEXTURE;
		pMaterial->m_DiffuseTexture.m_pAFCLibrary = pAFCLibrary;
		pMaterial->m_DiffuseTexture.m_OnResourceLoaded = CEventManager::GetInstance()->RegCallback(&CMaterial::OnModelLoaded, pMaterial);

		pMaterial->m_sName = ReadData<std::string>(pData);

		// texture will loaded next

		pRawModel->m_Materials.push_back(pMaterial);
	}

	CRawMesh const *pCenterMesh = nullptr;

	for (DWORD m = 0; m < dwNumMeshes; m++)
	{
		Section = ReadData<CSectionHeader>(pData);
		if (Section.nID != ESection::MESH)
		{
			CHError er = new CError_ModelLoadingError(AT);
			return;
		}

		CRawMesh *pRawMesh = new CRawMesh(pRawModel, E_MESH_TYPE::TRIANGLEMESH, E_MESH_TOPOLOGY::TRIANGLELIST, pRawModel->m_eRidigBodyFlag);

		DWORD dwMaterialID = ReadData<DWORD>(pData);
		DWORD dwNumIndices = ReadData<DWORD>(pData);
		DWORD dwNumVertices = ReadData<DWORD>(pData);

		CAABB aabb;
		aabb.SetMinMax(ReadData<XMFLOAT3>(pData), ReadData<XMFLOAT3>(pData));
		pRawMesh->m_AABB = aabb;

		CBSphere bsphere;
		bsphere.SetPosition(ReadData<XMFLOAT3>(pData));
		bsphere.SetRadius(ReadData<float>(pData));

		pRawMesh->m_mLocalWorld = ReadData<XMMATRIX>(pData);
		pRawMesh->m_sName = ReadData<std::string>(pData);

		if (pRawMesh->m_sName == pRawModel->m_sCenterMesh)
		{
			pCenterMesh = pRawMesh;
		}

		if (dwMaterialID != -1 && dwMaterialID >= 0 && dwMaterialID < pRawModel->m_Materials.size())
		{
			pRawMesh->m_pMaterial = pRawModel->m_Materials[dwMaterialID];
		}

		{
			Section = ReadData<CSectionHeader>(pData);
			if (Section.nID != ESection::VERTICES)
			{
				CHError er = new CError_ModelLoadingError(AT);
				return;
			}

			pRawMesh->m_Vertices.resize(dwNumVertices);
			memcpy(&pRawMesh->m_Vertices[0], pData, dwNumVertices * sizeof(CVertex));
			pData += dwNumVertices * sizeof(CVertex);

			XMVECTOR xvScale, xvQuat, xvPos;

			if (!XMMatrixDecompose(&xvScale, &xvQuat, &xvPos, pRawMesh->m_mLocalWorld))
			{
				xvScale = g_XMOne;
				xvQuat = g_XMZero;
				xvPos = g_XMOne;
			}

			if (XMVector3NotEqual(xvScale, g_XMOne) || XMVector3NotEqual(XMLoadFloat3(&pRawModel->m_vScale), g_XMOne))
			{
				// iVertex in local space of mesh
				for (CVertex &iVertex : pRawMesh->m_Vertices)
				{
					iVertex.vPosition.x *= XMVectorGetX(xvScale) * pRawModel->m_vScale.x;
					iVertex.vPosition.y *= XMVectorGetY(xvScale) * pRawModel->m_vScale.y;
					iVertex.vPosition.z *= XMVectorGetZ(xvScale) * pRawModel->m_vScale.z;

					iVertex.vNormal.x *= XMVectorGetX(xvScale) * pRawModel->m_vScale.x;
					iVertex.vNormal.y *= XMVectorGetY(xvScale) * pRawModel->m_vScale.y;
					iVertex.vNormal.z *= XMVectorGetZ(xvScale) * pRawModel->m_vScale.z;

					XMStoreFloat3(&iVertex.vNormal, XMVector3Normalize(XMLoadFloat3(&iVertex.vNormal)));
				}

				xvPos *= XMLoadFloat3(&pRawModel->m_vScale);

				// recreate matrix without scaling
				pRawMesh->m_mLocalWorld = XMMatrixRotationQuaternion(xvQuat);
				pRawMesh->m_mLocalWorld.r[3] = xvPos;
				pRawMesh->m_mLocalWorld._44 = 1.0f;
			}
		}

		{
			Section = ReadData<CSectionHeader>(pData);
			if (Section.nID != ESection::INDICES)
			{
				CHError er = new CError_ModelLoadingError(AT);
				return;
			}

			pRawMesh->m_Indices.resize(dwNumIndices);
			memcpy(&pRawMesh->m_Indices[0], pData, dwNumIndices * 4);
			pData += dwNumIndices * 4;
		}

		pRawModel->m_RawMeshes.push_back(pRawMesh);
	}
	/*
	// зависимость только по координатам
	// у pCenterMesh есть какие-то координаты
	// нужно ото всех остальных вычесть эти координаты
	XMVECTOR xvCenterMeshPos = g_XMZero;
	if (pCenterMesh)
	{
		xvCenterMeshPos = pCenterMesh->m_mLocalWorld.r[3];
	}

	for (auto pRawMesh : pRawModel->m_RawMeshes)
	{
		pRawMesh->m_mLocalWorld.r[3] = pRawMesh->m_mLocalWorld.r[3] - xvCenterMeshPos;
		pRawMesh->m_mLocalWorld._44 = 1.0f;
	}
	*/
	
	// если не выбрана главная мешка - выбираем руками
	if (!pCenterMesh && dwNumMeshes)
	{
		pCenterMesh = pRawModel->m_RawMeshes[0];
		pRawModel->m_sCenterMesh = pCenterMesh->m_sName;
	}
	
	// полная зависимость(скалинг, поворот и позиция)
	XMMATRIX mDeltaLocal = XMMatrixInverse(nullptr, pCenterMesh->m_mLocalWorld);

	// нужно локальные координаты мешки с центром ноль, переделать в координаты с центром в pCenterMesh
	for (auto pRawMesh : pRawModel->m_RawMeshes)
	{
		if (pRawMesh != pCenterMesh)
			pRawMesh->m_mLocalWorld *= mDeltaLocal;
	}
	
	// идея в том, что мы пропускаем центральную мешку, и не преобразуем её координаты
	// получается так, что центральная мешка сохраняет свои координаты, а все остальные становятся вокруг нуля
	// и нужно поставить актёра на эту матрицу первую

	SAFE_DELETE_ARRAY(pLoadedData);
}