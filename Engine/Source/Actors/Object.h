#pragma once

#include "EnginePCH.h"
#include "GraphicsRenderers\\BaseGraphicsRenderer.h"
#include "GraphicsRenderers\\BaseMeshRenderer.h"
#include "Physics\\PhysXLoader.h"

class CAABB;
class CBSphere;
class IActor;

class CObject final
{
public:
	CObject(IActor *pActor, std::string const &sObjectName);

	virtual ~CObject()
	{
	};

	void Init();

	// возвращает флаг о том, что нужно ли апдейтить физику или нет
	bool Update(bool bIsActorMoved = false);

	void SetTransform(XMMATRIX const &mTransform, bool bRecalcPos = true);

	XMMATRIX const &GetGlobalTransform() const
	{
		return m_mGlobalCurrent;
	};

	XMMATRIX const &GetPrevGlobalTransform() const
	{
		return m_mGlobalLast;
	};

	XMMATRIX const &GetGlobalRenderTransform() const
	{
		return m_mGlobalInterpolated;
	};

	XMMATRIX GetLocalTransform() const;

	void InterpolateMatrices(float fInterpolation);
	void RecalcRenderMatrix(float fInterpolation);

	IActor const *GetActor() const
	{
		return m_pActor;
	};

	void SetPosition(XMFLOAT3 const &vPos)
	{
		m_vPos = vPos;

		m_bIsNeedPhysicsUpdate = true;
	};

	void SetPosition(XMVECTOR const &xvPos)
	{
		XMStoreFloat3(&m_vPos, xvPos);

		m_bIsNeedPhysicsUpdate = true;
	};

	XMFLOAT3 const &GetPosition() const
	{
		return m_vPos;
	};

	void SetRotation(float fPitch, float fYaw, float fRoll)
	{
		XMVECTOR xvQuat = XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(fPitch, fYaw, fRoll, 0.0f));
		xvQuat = XMQuaternionNormalize(xvQuat);
		XMStoreFloat4(&m_vQuat, xvQuat);

		m_bIsNeedPhysicsUpdate = true;
	};

	void SetRotation(XMFLOAT4 const &vQuat)
	{
		m_vQuat = vQuat;
		XMStoreFloat4(&m_vQuat, XMQuaternionNormalize(XMLoadFloat4(&vQuat)));

		m_bIsNeedPhysicsUpdate = true;
	};

	void SetRotation(XMVECTOR const &xvQuat)
	{
		XMStoreFloat4(&m_vQuat, XMQuaternionNormalize(xvQuat));

		m_bIsNeedPhysicsUpdate = true;
	};

	XMFLOAT4 const &GetRotationQuat() const
	{
		return m_vQuat;
	};

	XMFLOAT3 GetRotationEuler() const
	{
		XMFLOAT3 vRot;
		MathHelper::ConvertQuatToEuler(XMLoadFloat4(&m_vQuat), vRot);

		return vRot;
	};

	XMCOLOR GetColor() const
	{
		return m_Color;
	}

	void SetColor(const XMCOLOR& _Color)
	{
		m_Color = _Color;
	}

	std::string const &GetName() const
	{
		return m_sObjectName;
	};

	PxRigidActor *GetPhysicsActor()
	{
		return m_pPhysActor;
	};

	void SetPhysicsActor(PxRigidActor *pPhysicsActor)
	{
		m_pPhysActor = pPhysicsActor;
	};

	PxShape *GetPhysicsShape() const
	{
		return m_pShape;
	};

	void SetPhysicsShape(PxShape *pShape)
	{
		m_pShape = pShape;
	}
	
	CAABB &GetAABB()
	{
		return m_AABB;
	};
private:
	void UpdateTransform();

	PxRigidActor	*m_pPhysActor = nullptr;
	// or vector. need to think about that )
	PxShape			*m_pShape = nullptr;

	std::string		m_sObjectName;

	CAABB			m_AABB;

	XMFLOAT3		m_vPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4		m_vQuat = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX		m_mGlobalInterpolated = XMMatrixIdentity();
	XMMATRIX		m_mGlobalLast = XMMatrixIdentity();
	XMMATRIX		m_mGlobalCurrent = XMMatrixIdentity();
	
	bool			m_bIsNeedPhysicsUpdate = true;

	XMCOLOR			m_Color;

	IActor			*m_pActor = nullptr;
};