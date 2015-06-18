#pragma once

#include "EnginePCH.h"
#include "BaseActor.h"

#include "Physics\\PrimitivesBehavior.h"

class ICamera;

static const float		g_fOffset = 0.3f;

class CPlane : public IActor
{
public:
	CPlane(IBaseModelRenderer *pModelRenderer = nullptr) :
		IActor(E_ACTOR_TYPE::PLANE, pModelRenderer)
	{
		if (!m_Objects.empty())
			SetObject(m_Objects[0]);
	};

	virtual ~CPlane()
	{
	};

	IPhysicalBehavior *VGetPhysicsBehavior() override
	{
		if (!m_pPhysicsBehavior)
		{
			m_pPhysicsBehavior = new CPrimitivePhysics();
		}

		return m_pPhysicsBehavior;
	};

	virtual void VInit() override;
	/*
	virtual void VUpdate() override
	{
		auto pModelRenderer = GetModelRenderer();

		XMMATRIX mTransform = XMMatrixRotationQuaternion(XMLoadFloat4(&IActor::GetRotationQuat()));
		mTransform.r[3] = XMLoadFloat3(&IActor::GetPosition());
		mTransform._44 = 1.0f;

		SetTransform(mTransform, false);

		if (VGetPhysicsBehavior())
		{
			VGetPhysicsBehavior()->UpdateAABB();
		}
	};*/

	XMFLOAT4 &GetPlane()
	{
		return m_vPlane;
	};

	void SetObject(CObject *pObject)
	{
		m_pObject = pObject;
	};
protected:
	XMFLOAT4				m_vPlane = XMFLOAT4(0.5f, 0.0f, 0.5f, 0.0f);

	CObject					*m_pObject = nullptr;
};

class CSphere : public IActor
{
public:
	CSphere(IBaseModelRenderer *pModelRenderer = nullptr) :
		IActor(E_ACTOR_TYPE::SPHERE, pModelRenderer)
	{
		if (!m_Objects.empty())
			SetObject(m_Objects[0]);
	};

	CSphere(const XMFLOAT3& _vCenter, float _fRadius, IBaseModelRenderer *pModelRenderer = nullptr) :
		IActor(E_ACTOR_TYPE::SPHERE, pModelRenderer)
	{
		m_BSphere.SetPosition(_vCenter);
		m_BSphere.SetRadius(_fRadius);

		if (!m_Objects.empty())
			SetObject(m_Objects[0]);
	};

	CSphere(const XMVECTOR& _xvCenter, float _fRadius, IBaseModelRenderer *pModelRenderer = nullptr) :
		IActor(E_ACTOR_TYPE::SPHERE, pModelRenderer)
	{
		m_BSphere.SetPosition(_xvCenter);
		m_BSphere.SetRadius(_fRadius);

		if (!m_Objects.empty())
			SetObject(m_Objects[0]);
	};

	virtual ~CSphere()
	{
	};

	virtual void VInit() override
	{
		if (m_pObject)
			m_pObject->SetColor(XMCOLOR(255.0f, 0.0f, 0.0f, 255.0f));

		XMFLOAT3 vStoredPos = m_BSphere.GetCenter();

		IActor::VInit();

		IActor::SetPosition(vStoredPos);
	};

	IPhysicalBehavior *VGetPhysicsBehavior() override
	{
		if (!m_pPhysicsBehavior)
		{
			m_pPhysicsBehavior = new CPrimitivePhysics();
		}

		return m_pPhysicsBehavior;
	};

	void SetObject(CObject *pObject)
	{
		m_pObject = pObject;
	};
protected:
	CObject					*m_pObject = nullptr;
};

class CBox : public IActor
{
public:
	CBox(IBaseModelRenderer *pModelRenderer = nullptr) :
		IActor(E_ACTOR_TYPE::CUBE, pModelRenderer)
	{
		if (!m_Objects.empty())
			SetObject(m_Objects[0]);
	}

	CBox(const XMFLOAT3& _vMin, const XMFLOAT3& _vMax, IBaseModelRenderer *pModelRenderer = nullptr) :
		IActor(E_ACTOR_TYPE::CUBE, pModelRenderer)
	{
		IActor::m_AABB.SetMinMax(_vMin, _vMax);

		if (!m_Objects.empty())
			SetObject(m_Objects[0]);
	}

	CBox(const XMVECTOR& _xvMin, const XMVECTOR& _xvMax, IBaseModelRenderer *pModelRenderer = nullptr) :
		IActor(E_ACTOR_TYPE::CUBE, pModelRenderer)
	{
		IActor::m_AABB.SetMinMax(_xvMin, _xvMax);

		if (!m_Objects.empty())
			SetObject(m_Objects[0]);
	}

	virtual void VInit() override
	{
		if (m_pObject)
			m_pObject->SetColor(XMCOLOR(255.0f, 0.0f, 0.0f, 255.0f));

		XMFLOAT3 vStoredPos = m_AABB.GetPosition();

		IActor::VInit();

		IActor::SetPosition(vStoredPos);
	};

	IPhysicalBehavior *VGetPhysicsBehavior() override
	{
		if (!m_pPhysicsBehavior)
		{
			m_pPhysicsBehavior = new CPrimitivePhysics();
		}

		return m_pPhysicsBehavior;
	};

	virtual ~CBox()
	{
	};

	void SetObject(CObject *pObject)
	{
		m_pObject = pObject;
	};
protected:
	CObject					*m_pObject = nullptr;
};

class CFrustum : public IActor
{
public:
	CFrustum(ICamera const *const _pCamera, IBaseModelRenderer *pModelRenderer = nullptr) :
		m_pCamera(_pCamera), 
		IActor(E_ACTOR_TYPE::FRUSTUM, pModelRenderer)
	{
	};

	virtual ~CFrustum()
	{
	};

	virtual void VInit() override
	{
		if (m_pObject)
			m_pObject->SetColor(XMCOLOR(255.0f, 0.0f, 0.0f, 255.0f));

		IActor::VInit();
	};

	IPhysicalBehavior *VGetPhysicsBehavior() override
	{
		return nullptr;
	};

	XMFLOAT4 const *GetPlanes() const
	{
		return m_vPlanes;
	};

	ICamera const *GetCamera() const
	{
		return m_pCamera;
	};

	void SetObject(CObject *pObject)
	{
		m_pObject = pObject;
	};

	virtual void VUpdate() override;
	virtual void VRecalcRenderMatrices(float fInterpolation) override;
protected:
	XMFLOAT4			m_vPlanes[6];
	ICamera const		*m_pCamera = nullptr;

	CObject				*m_pObject = nullptr;
};