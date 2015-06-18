#ifndef H__BASEACTOR
#define H__BASEACTOR

#include "EnginePCH.h"
#include "Object.h"
#include "IntersectionMath.h"

class IPhysicalBehavior;

enum class E_ACTOR_TYPE
{
	SIMPLE = 0,
	CUBE,
	SPHERE,
	PLANE,
	FRUSTUM,
	CAMERA,
	VEHICLE
};

class IActor
{
public:
	virtual ~IActor()
	{
		for (auto iObject : m_Objects)
		{
			SAFE_DELETE(iObject);
		}
	};
public:
	// models from files
	IActor(std::string const &sActorFile, std::string const &sModelFile = "");

	// in-engine models
	IActor(
		E_ACTOR_TYPE const eActorType,
		IBaseModelRenderer *pModelRenderer,
		std::string const &sLinkedObject = "");

	E_ACTOR_TYPE GetActorType() const
	{
		return m_eActorType;
	};

	virtual void VInit();

	// to apply any physical behavior to some type actor.
	virtual IPhysicalBehavior *VGetPhysicsBehavior() = 0;
	virtual void VUpdate();
	virtual void VRecalcRenderMatrices(float fInterpolation);

	std::vector<CObject *> &GetObjects()
	{
		return m_Objects;
	};

	std::vector<CObject *> const &GetObjects() const
	{
		return m_Objects;
	};

	void SetPosition(XMFLOAT3 const &vPos)
	{
		m_pLinkedObject->SetPosition(vPos);

		m_bUpdateTransform = true;
	};

	void SetPosition(XMVECTOR const &xvPos)
	{
		m_pLinkedObject->SetPosition(xvPos);

		m_bUpdateTransform = true;
	};

	XMFLOAT3 const &GetPosition() const
	{
		return m_pLinkedObject->GetPosition();
	};

	void SetRotation(float fPitch, float fYaw, float fRoll)
	{
		m_pLinkedObject->SetRotation(fPitch, fYaw, fRoll);

		m_bUpdateTransform = true;
	};

	void SetRotation(XMFLOAT4 const &vQuat)
	{
		m_pLinkedObject->SetRotation(vQuat);

		m_bUpdateTransform = true;
	};

	void SetRotation(XMVECTOR const &xvQuat)
	{
		m_pLinkedObject->SetRotation(xvQuat);

		m_bUpdateTransform = true;
	};

	XMFLOAT4 const &GetRotationQuat() const
	{
		return m_pLinkedObject->GetRotationQuat();
	};

	XMFLOAT3 GetRotationEuler() const
	{
		return m_pLinkedObject->GetRotationEuler();
	};

	void SetTransform(XMMATRIX const &mTransform, bool bRecalcPos = true)
	{
		m_pLinkedObject->SetTransform(mTransform, bRecalcPos);

		m_bUpdateTransform = true;
	};

	XMMATRIX const &GetGlobalTransform() const
	{
		return m_pLinkedObject->GetGlobalTransform();
	};

	XMMATRIX const &GetPrevGlobalTransform() const
	{
		return m_pLinkedObject->GetPrevGlobalTransform();
	};

	XMMATRIX const &GetRenderTransform() const
	{
		return m_pLinkedObject->GetGlobalRenderTransform();
	};

	CAABB &GetAABB()
	{
		return m_AABB;
	};

	CBSphere &GetBSphere()
	{
		return m_BSphere;
	};

	IBaseModelRenderer *GetModelRenderer() const
	{
		if (m_pModelRenderer)
			return m_pModelRenderer;
		else
		{
			IResource *pResource = CResourceManager::GetInstance()->GetResource(m_ModelResource);

			if (pResource)
				return pResource->ToModelResource()->GetModelRenderer();
		}
		
		return nullptr;
	};

	CObject *GetObjectByName(std::string const &sObjectName) const
	{
		auto it = m_ObjectNameToObject.find(sObjectName);
		if (it == m_ObjectNameToObject.end())
		{
			return nullptr;
		}

		return it->second;
	};

	void OnModelLoaded(IResource *pResource)
	{
		m_ModelResource.m_dwUniqueResourceID = pResource->GetResourceID();
	}

	// physics!
	void ApplyForce(XMFLOAT3 const &vForceDir);
protected:
	// хранит указатель на класс, описывающий поведение актёра.
	IPhysicalBehavior		*m_pPhysicsBehavior = nullptr;

	CResourceLoadingData	m_ModelResource;

	E_ACTOR_TYPE			m_eActorType;

	CAABB					m_AABB;
	CBSphere				m_BSphere;

	std::vector<CObject *>	m_Objects;

	bool					m_bUpdateTransform = true;
private:
	std::string				m_sActorFile;

	CObject					*m_pLinkedObject = nullptr;
	std::unordered_map<std::string, CObject *> 
							m_ObjectNameToObject;

	// it
	IBaseModelRenderer		*m_pModelRenderer = nullptr;
};

#endif