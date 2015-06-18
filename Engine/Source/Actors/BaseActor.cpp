#include "EnginePCH.h"
#include "BaseActor.h"
#include "Physics\\BasePhysicsBehavior.h"
#include "MainLoopHandler.h"

// models from files
IActor::IActor(std::string const &sActorFile, std::string const &sModelFile)
{
	std::string sTemp;

	INI<> ini(sActorFile, true);

	/*
	if ((sTemp = ini.get("Globals", "Type", INI_ERROR)) != INI_ERROR)
	{
		if (sTemp == "SIMPLE")
			m_eActorType = E_ACTOR_TYPE::SIMPLE;
		else if (sTemp == "CUBE")
			m_eActorType = E_ACTOR_TYPE::CUBE;
		else if (sTemp == "SPHERE")
			m_eActorType = E_ACTOR_TYPE::SPHERE;
		else if (sTemp == "PLANE")
			m_eActorType = E_ACTOR_TYPE::PLANE;
		else if (sTemp == "FRUSTUM")
			m_eActorType = E_ACTOR_TYPE::FRUSTUM;
		else if (sTemp == "CAMERA")
			m_eActorType = E_ACTOR_TYPE::CAMERA;
	}
	*/

	m_ModelResource.m_bIsInfinite = true;
	m_ModelResource.m_ePriority = E_RESOURCE_PRIORITY::MEDIUM;
	m_ModelResource.m_eResourceType = E_RESOURCE_TYPE::MODEL;
	m_ModelResource.m_OnResourceLoaded = CEventManager::GetInstance()->RegCallback(&IActor::OnModelLoaded, this);
	m_ModelResource.m_pAFCLibrary = CMainLoopHandler::GetInstance()->GetAFCLibrary();
	//m_ModelResource.m_pAddData = new CModelAdditionalData(1.0f);

	if ((sTemp = ini.get("Globals", "Model", INI_ERROR)) != INI_ERROR)
		m_ModelResource.m_sFileName = sTemp;

	if (!sModelFile.empty())
	{
		m_ModelResource.m_sFileName = sModelFile;
	}

	// read objects
	ini.select("Objects");

	int i = 0;
	while ((sTemp = ini.get("Objects", "obj" + StringHelper::NumberToString(i), INI_ERROR)) != INI_ERROR)
	{
		auto pObject = new CObject(this, sTemp);
		m_Objects.push_back(pObject);
		m_ObjectNameToObject.emplace(pObject->GetName(), pObject);

		i++;
	}

	m_sActorFile = sActorFile;
};


// in-engine models
IActor::IActor(
	E_ACTOR_TYPE const eActorType,
	IBaseModelRenderer *pModelRenderer,
	std::string const &sLinkedObject) :
	m_pModelRenderer(pModelRenderer),
	m_eActorType(eActorType)
{
	IBaseMeshRenderer *pLinkedMesh = nullptr;

	if (m_pModelRenderer)
	{
		m_ModelResource.m_sFileName = pModelRenderer->GetName();

		for (auto iMesh : m_pModelRenderer->GetMeshes())
		{
			if (!pLinkedMesh)
				pLinkedMesh = iMesh;

			auto pObject = new CObject(this, iMesh->GetName());
			m_Objects.push_back(pObject);
			m_ObjectNameToObject.emplace(pObject->GetName(), pObject);

			if (sLinkedObject == iMesh->GetName() || (sLinkedObject.empty() && !m_pLinkedObject))
			{
				m_pLinkedObject = pObject;
			}
		}
	}

	// если не нашли слинкованный объект, создаём пустой
	if (!m_pLinkedObject)
	{
		std::string sLOName = "nullobject";
		if (pLinkedMesh)
			sLOName = pLinkedMesh->GetName();

		m_pLinkedObject = new CObject(this, sLOName);
		m_Objects.push_back(m_pLinkedObject);
		m_ObjectNameToObject.emplace(m_pLinkedObject->GetName(), m_pLinkedObject);
	}
};

void IActor::VInit()
{
	auto pModel = GetModelRenderer();
	if (pModel)
	{
		IBaseMeshRenderer *pLinkedMesh = nullptr;

		// need to add garbage object for physics
		if (pModel->GetMeshes().size() > m_Objects.size())
		{
			for (auto iMesh : pModel->GetMeshes())
			{
				if (!pLinkedMesh)
					pLinkedMesh = iMesh;

				auto pObject = GetObjectByName(iMesh->GetName());
				if (!pObject)
				{
					auto pGarbageObject = new CObject(this, iMesh->GetName());
					m_Objects.push_back(pGarbageObject);
					m_ObjectNameToObject.emplace(pGarbageObject->GetName(), pGarbageObject);
				}
			}
		}

		m_pLinkedObject = GetObjectByName(pModel->GetCenterMeshName());

		// если не нашли слинкованный объект, то всё плохо, чо.
		if (!m_pLinkedObject)
		{
			CHError er = new CError_ObjectModelCreatingFailed(AT);
			return;
		}
	}

	for (auto pObject : m_Objects)
	{
		pObject->Init();
	}

	// сохраняем позицию главного объекта в модельке
	XMFLOAT3 vPos = m_pLinkedObject->GetPosition();
	XMFLOAT4 vQuat = m_pLinkedObject->GetRotationQuat();

	// обновляем трансформацию в ноль главному объекту
	m_pLinkedObject->SetTransform(XMMatrixIdentity());
	// если чо, надо ещё раз будет сделать SetTransform(XMMatrixIdentity(), false);

	// восстанавливаем позицию
	m_pLinkedObject->SetPosition(vPos);
	m_pLinkedObject->SetRotation(vQuat);
	
	std::string sTemp;

	INI<> ini(m_sActorFile, true);
	
	if ((sTemp = ini.get("Globals", "InitRot", INI_ERROR)) != INI_ERROR)
	{
		auto Splits = StringHelper::Split(sTemp, ";");
		SetRotation(
			XMConvertToRadians(StringHelper::StringToNumber<float>(Splits[0])),
			XMConvertToRadians(StringHelper::StringToNumber<float>(Splits[1])),
			XMConvertToRadians(StringHelper::StringToNumber<float>(Splits[2])));
	}
	
	if ((sTemp = ini.get("Globals", "InitPos", INI_ERROR)) != INI_ERROR)
	{
		auto Splits = StringHelper::Split(sTemp, ";");
		SetPosition(XMFLOAT3(
			StringHelper::StringToNumber<float>(Splits[0]),
			StringHelper::StringToNumber<float>(Splits[1]),
			StringHelper::StringToNumber<float>(Splits[2])));
	}

	m_pLinkedObject->Update();
	for (auto pObject : m_Objects)
	{
		if (pObject != m_pLinkedObject)
		{
			pObject->Update(true);
		}
	}

	if (VGetPhysicsBehavior())
	{
		VGetPhysicsBehavior()->VInit(this);
		VGetPhysicsBehavior()->UpdateAABB();
	}
}

void IActor::ApplyForce(XMFLOAT3 const &vForceDir)
{
	if (VGetPhysicsBehavior())
		VGetPhysicsBehavior()->ApplyForce(vForceDir);
};

void IActor::VUpdate()
{
	bool bNeedToUpdateObjectPhysics = m_pLinkedObject->Update();

	for (auto iObject : m_Objects)
	{
		if (iObject != m_pLinkedObject)
		{
			// суммируем необходимость апдейта. если что-то нужно апдейтнуть - то это выйлется потом
			// плюс кидаем флаг о том, что нужно каждый объект подвинуть из-за того, что подвинулся слинкованный объект
			bNeedToUpdateObjectPhysics |= iObject->Update(m_bUpdateTransform);
		}
	}

	if (VGetPhysicsBehavior())
	{
		VGetPhysicsBehavior()->UpdateAABB();

		if (bNeedToUpdateObjectPhysics || m_bUpdateTransform) //&& GetBodyFlag() != E_RIGID_BODY_FLAG::STATIC)
			VGetPhysicsBehavior()->UpdatePhysicsPosition();
	}
	
	m_bUpdateTransform = false;
};

void IActor::VRecalcRenderMatrices(float fInterpolation)
{
	for (auto iObject : m_Objects)
	{
		iObject->RecalcRenderMatrix(fInterpolation);
	}
}