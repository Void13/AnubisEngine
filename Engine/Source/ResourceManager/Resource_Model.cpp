#include "EnginePCH.h"
#include "Resource_Model.h"
#include "GraphicsRenderers\\BaseMeshRenderer.h"
#include "AMFModelLoader.h"

void CModelResource::VLoadResource(CResourceLoadingData const &LoadingData)
{
	//m_sName = LoadingData.m_sFileName;
	m_bIsInfinite = LoadingData.m_bIsInfinite;
	
	CRawModel RawModel;

	std::string sModelFileName;
	std::string sPhysicsFileName;

	// read ini file in data
	//INI<> ini((void *)LoadingData.GetFileData(), LoadingData.GetFileDataSize(), true);
	INI<> ini(CSettingsHandler::GetDataFolder() + "modelsdef\\" + LoadingData.m_sFileName, true);
	std::string sTemp;

	if ((sTemp = ini.get("Globals", "Model", INI_ERROR)) != INI_ERROR)
	{
		sModelFileName = sTemp;
	}

	if ((sTemp = ini.get("Globals", "PhysicsModel", INI_ERROR)) != INI_ERROR)
	{
		sPhysicsFileName = sTemp;
	}

	if ((sTemp = ini.get("LODs", "LOD0", INI_ERROR)) != INI_ERROR)
	{
		//sModelFileName = sTemp;
	}

	if ((sTemp = ini.get("LODs", "LOD1", INI_ERROR)) != INI_ERROR)
	{
		//sModelFileName = sTemp;
	}

	if ((sTemp = ini.get("LODs", "LOD2", INI_ERROR)) != INI_ERROR)
	{
		//sModelFileName = sTemp;
	}

	if ((sTemp = ini.get("LODs", "LOD3", INI_ERROR)) != INI_ERROR)
	{
		//sModelFileName = sTemp;
	}

	if ((sTemp = ini.get("Globals", "MainMesh", INI_ERROR)) != INI_ERROR)
	{
		RawModel.m_sCenterMesh = sTemp;
	}



	if ((sTemp = ini.get("Globals", "Scale", INI_ERROR)) != INI_ERROR)
	{
		auto Splits = StringHelper::Split(sTemp, ";");

		if (Splits.size() == 1)
		{
			RawModel.m_vScale = XMFLOAT3(
				StringHelper::StringToNumber<float>(Splits[0]), 
				StringHelper::StringToNumber<float>(Splits[0]), 
				StringHelper::StringToNumber<float>(Splits[0]));
		}
		else
		{
			RawModel.m_vScale = XMFLOAT3(
				StringHelper::StringToNumber<float>(Splits[0]),
				StringHelper::StringToNumber<float>(Splits[1]),
				StringHelper::StringToNumber<float>(Splits[2]));
		}
	}

	if ((sTemp = ini.get("Globals", "BodyFlag", INI_ERROR)) != INI_ERROR)
	{
		StringHelper::ToLower(sTemp);

		if (sTemp == "static")
			RawModel.m_eRidigBodyFlag = E_RIGID_BODY_FLAG::STATIC;
		else if (sTemp == "kinematic")
			RawModel.m_eRidigBodyFlag = E_RIGID_BODY_FLAG::KINEMATIC;
		else
			RawModel.m_eRidigBodyFlag = E_RIGID_BODY_FLAG::DYNAMIC;
	}

	if (LoadingData.m_pAddData)
	{
		CModelAdditionalData *pModelAddData = (CModelAdditionalData *)LoadingData.m_pAddData;
		RawModel.m_vScale = pModelAddData->m_vScale;
	}

	RawModel.m_sModelName = sModelFileName;
	//RawModel.m_sModelName = sPhysicsFileName;

	auto NameAndExt = StringHelper::Split(sModelFileName, ".");
	if (NameAndExt.size() < 2)
		return;

	std::string sFileName = NameAndExt[0];
	std::string sFileExt = NameAndExt[1];

	// my format
	if (sFileExt == "amf")
	{
		LoadAMFModel(&RawModel, LoadingData.m_pAFCLibrary);
	}
	// any other format
	else
	{
		//Assimp::LoadModel(pModelRenderer, LoadingData);
	}

	pModelRenderer = CDistributedObjectCreator::GetInstance()->CreateModelRenderer();
	pModelRenderer->SetCenterMeshName(RawModel.m_sCenterMesh);
	pModelRenderer->SetRigidBodyFlag(RawModel.m_eRidigBodyFlag);
	pModelRenderer->SetRenderMethod(E_RENDERMETHOD::SOLID);
	for (auto pMat : RawModel.m_Materials)
		pModelRenderer->AddMaterial(pMat);
	pModelRenderer->SetLODDistance(RawModel.m_fLODDistance);
	pModelRenderer->SetName(RawModel.m_sModelName);

	if (sModelFileName != sPhysicsFileName)
	{
		CRawModel PhysRawModel(RawModel);
		PhysRawModel.m_Materials.clear();
		PhysRawModel.m_RawMeshes.clear();
		PhysRawModel.m_sModelName = sPhysicsFileName;

		LoadAMFModel(&PhysRawModel, LoadingData.m_pAFCLibrary);

		if (PhysRawModel.m_RawMeshes.size() != RawModel.m_RawMeshes.size())
		{
			CHError er = new CError_ModelLoadingError(AT);
			return;
		}

		for (size_t i = 0; i < RawModel.m_RawMeshes.size(); i++)
		{
			pModelRenderer->VAddMesh(RawModel.m_RawMeshes[i], PhysRawModel.m_RawMeshes[i]);
		}
	}
	else
	{
		for (size_t i = 0; i < RawModel.m_RawMeshes.size(); i++)
		{
			pModelRenderer->VAddMesh(RawModel.m_RawMeshes[i], RawModel.m_RawMeshes[i]);
		}
	}
};

CModelResource::CModelResource(RESOURCE_ID nResourceID, bool bIsUnique) :
	IResource(nResourceID, m_bIsUnique)
{
	m_eResourceType = E_RESOURCE_TYPE::MODEL;
}

CModelResource::~CModelResource()
{
	SAFE_DELETE(pModelRenderer);
}