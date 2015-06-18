#pragma once

#include "EnginePCH.h"

#include "BaseActor.h"
#include "PrimitiveObjects.h"

class ICamera;
class CCameraFirstPerson;

class ICamera : public IActor
{
protected:
	virtual void VInit();
	void UpdateMatrices();
public:
	ICamera();
	ICamera(const XMFLOAT3 &_vPos, const XMFLOAT3 &_vRot);

	virtual ~ICamera()
	{
	};

	virtual IPhysicalBehavior *VGetPhysicsBehavior() override
	{
		return nullptr;
	};

	virtual void VUpdate() override = 0;
	virtual void VRecalcRenderMatrices(float fInterpolation) override;

	void VClear();

	void OnReset();
	void OnKeysInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown);
	void OnMouseInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDelta, XMINT2 const &_vGlobalPos);

	// matrices work
	const XMMATRIX &GetViewMatrix() const
	{
		return m_mView;
	};

	const XMMATRIX &GetProjMatrix() const
	{
		return m_mProj;
	};

	const XMMATRIX &GetViewProjMatrix() const
	{
		return m_mViewProj;
	};

	// pos work

	void SetPosition(const XMFLOAT3 &vNewPos)
	{
		m_vPos = vNewPos;
	};

	void SetPosition(const XMVECTOR &xvNewPos)
	{
		XMStoreFloat3(&m_vPos, xvNewPos);
	};

	XMFLOAT3 GetPosition() const
	{
		return m_vPos;
	};

	// rot work

	void SetRotation(const XMFLOAT3 &vNewRot)
	{
		m_fPitch = vNewRot.x;
		m_fYaw = vNewRot.y;
		m_fRoll = vNewRot.z;
	};

	void SetRotation(const XMVECTOR &xvNewRot)
	{
		m_fPitch = XMVectorGetX(xvNewRot);
		m_fYaw = XMVectorGetY(xvNewRot);
		m_fRoll = XMVectorGetZ(xvNewRot);
	};

	XMFLOAT3 GetRotation() const
	{
		return XMFLOAT3(m_fPitch, m_fYaw, m_fRoll);
	};

	// look at work

	void SetLookAt(const XMFLOAT3 &vTo);
	void SetLookAt(const XMVECTOR &xvTo);
	void SetFrontVector(const XMVECTOR &xvFrontVector);

	XMFLOAT3 GetUpVector() const;
	XMFLOAT3 GetFrontVector() const;
	XMFLOAT3 GetRightVector() const;
	XMFLOAT3 GetLookAt(float fDist = 1.0f) const;

	// other funcs
	XMFLOAT3 GetMovingVector() const
	{
		return m_vMoveVelocity;
	};

	void SetMoveSpeed(float _fMoveSpeed)
	{
		m_fMoveSpeed = _fMoveSpeed;
	};

	float GetMoveSpeed() const
	{
		return m_fMoveSpeed;
	};

	void SetRotSpeed(float _fRotSpeed)
	{
		m_fRotateSpeed = _fRotSpeed;
	};

	float GetRotSpeed() const
	{
		return m_fRotateSpeed;
	};

	CFrustum *GetFrustum() const
	{
		return m_pFrustum;
	};

	void SetFreeze(bool _bFreeze);
	bool IsFreezed() const
	{
		return m_bIsFreezed;
	};

	void SetPerspectiveMatrix(float FovAngleY, float AspectHByW, float NearZ, float FarZ)
	{
		m_mProj = XMMatrixPerspectiveFovLH(FovAngleY, AspectHByW, NearZ, FarZ);
		m_fFarPlane = FarZ;
		m_fFOV = FovAngleY;
	};

	void SetOrthogonalMatrix(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
	{
		m_mProj = XMMatrixOrthographicLH(ViewWidth, ViewHeight, NearZ, FarZ);
		m_fFarPlane = FarZ;
		m_fFOV = 0.0f;
	};

	float GetFOV() const
	{
		return m_fFOV;
	};

	float GetFarPlane() const
	{
		return m_fFarPlane;

		// orthographic:
		//return m_mProj.m[3][2] / (1.0f - m_mProj.m[2][2]);

		// perspective
		//return (1.0f - m_mProj.m[3][2]) / m_mProj.m[2][2];
	};
protected:
	// input
	std::unordered_map<INPUTEVENT, bool>
					m_MappedKeys;

	XMFLOAT3		m_vMappedMouseDelta;
	
	float const		m_fPercentOfNew = 0.4f;
	float const		m_fPercentOfOld = 1.0f - m_fPercentOfNew;
	XMFLOAT3		m_vCurrentMouseDelta = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3		m_vMoveVelocity;
	XMVECTOR		m_xvVelocityDrag;
	double			m_fDragTimer = 0.0f;

	// matrices
	XMMATRIX		m_mView;
	XMMATRIX		m_mProj;
	XMMATRIX		m_mViewProj;

	XMFLOAT3		m_vPos;
	XMFLOAT3		m_vLookAt;

	mutable CFrustum *m_pFrustum;

	float			m_fFOV;
	float			m_fFarPlane;

	float			m_fPitch;
	float			m_fYaw;
	float			m_fRoll;
	float			m_fMoveSpeed;
	float			m_fRotateSpeed;

	bool			m_bIsFreezed = true;

	// к какому актёру прикреплена камера
	IActor			*m_pAttachedActor;
};

class CCameraFirstPerson final : public ICamera
{
public:
	CCameraFirstPerson() :
		ICamera()
	{
	};

	CCameraFirstPerson(const XMFLOAT3 &_vPos, const XMFLOAT3 &_vRot) :
		ICamera(_vPos, _vRot)
	{
	};

	virtual ~CCameraFirstPerson()
	{
	};

	virtual void VUpdate() override;
};

class CCameraThirdPerson final : public ICamera
{
public:
	CCameraThirdPerson() :
		ICamera()
	{
	};

	CCameraThirdPerson(const XMFLOAT3 &_vPos, const XMFLOAT3 &_vRot) :
		ICamera(_vPos, _vRot)
	{
	};

	virtual ~CCameraThirdPerson()
	{
	};

	virtual void VUpdate() override;
};

ICamera *ReflectCamera(const ICamera *pCamera, const XMFLOAT4 &plPlane);
void ScreenToWorld(const ICamera *pCamera, XMINT2 const &vScreenPos, OUT XMFLOAT3 &vRayDir, OUT XMFLOAT3 &vRayOrigin);
void ScreenToWorld(const ICamera *pCamera, XMINT2 const &vScreenPos, OUT XMVECTOR &xvRayDir, OUT XMVECTOR &xvRayOrigin);