#include "EnginePCH.h"

#include "Camera.h"
#include "MainLoopHandler.h"

ICamera::ICamera() :
	IActor(E_ACTOR_TYPE::CAMERA, nullptr)
{
	m_pFrustum = new CFrustum(this);

	VInit();
}

ICamera::ICamera(const XMFLOAT3 &_vPos, const XMFLOAT3 &_vRot) :
	IActor(E_ACTOR_TYPE::CAMERA, nullptr)
{
	m_pFrustum = new CFrustum(this);

	m_vPos = _vPos;
	m_fPitch = _vRot.x;
	m_fYaw = _vRot.y;
	m_fRoll = _vRot.z;

	VInit();
}

void ICamera::VRecalcRenderMatrices(float fInterpolation)
{
	for (int i = 0; i < 4; i++)
		m_mView.r[i] = XMVectorLerp(GetPrevGlobalTransform().r[i], GetGlobalTransform().r[i], fInterpolation);

	m_mViewProj = m_mView * m_mProj;
}

void ICamera::VInit()
{
	VClear();

	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_KEYS_INPUT, &ICamera::OnKeysInput, this);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_MOUSE_INPUT, &ICamera::OnMouseInput, this);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::RESET_SYSTEMS, &ICamera::OnReset, this);

	IActor::VInit();
}

void ICamera::OnReset()
{
	float fAspectRatio = (float)CSettingsHandler::GetInstance()->GetWindowWidth() / (float)CSettingsHandler::GetInstance()->GetWindowHeight();
	SetPerspectiveMatrix(GetFOV(), fAspectRatio, 0.1f, GetFarPlane());
}

void ICamera::VClear()
{
	m_fMoveSpeed = 10.0f;
	m_fRotateSpeed = 0.5f;

	m_vPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fPitch = m_fYaw = m_fRoll = 0.0f;

	m_mView = XMMatrixIdentity();

	m_MappedKeys.clear();
	m_vMappedMouseDelta = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void GetSinCos(float _fAngle, float &fSin, float &fCos)
{
	fSin = sinf(XMConvertToRadians(_fAngle));
	fCos = cosf(XMConvertToRadians(_fAngle));
}

void ICamera::UpdateMatrices()
{
	float sy, cy, sp, cp, sr, cr;
	GetSinCos(m_fYaw, sy, cy);
	GetSinCos(m_fPitch, sp, cp);
	GetSinCos(m_fRoll, sr, cr);

	m_mView._11 = cy * cr + sy * sp * sr;
	m_mView._12 = -cy * sr + sy * sp * cr;
	m_mView._13 = sy * cp;

	m_mView._21 = sr * cp;
	m_mView._22 = cr * cp;
	m_mView._23 = -sp;

	m_mView._31 = -sy * cr + cy * sp * sr;
	m_mView._32 = sr * sy + cy * sp * cr;
	m_mView._33 = cy * cp;

	/*
	_11, _21, _31 - right vector
	_12, _22, _32 - up vector
	_13, _23, _33 - forward vector
	*/

	m_mView._14 = m_mView._24 = m_mView._34 = 0.0f;

	m_mView._44 = 1.0f;

	m_mView._41 = -(m_vPos.x * m_mView._11 + m_vPos.y * m_mView._21 + m_vPos.z * m_mView._31);
	m_mView._42 = -(m_vPos.x * m_mView._12 + m_vPos.y * m_mView._22 + m_vPos.z * m_mView._32);
	m_mView._43 = -(m_vPos.x * m_mView._13 + m_vPos.y * m_mView._23 + m_vPos.z * m_mView._33);

	IActor::SetTransform(m_mView, false);

	m_mViewProj = m_mView * m_mProj;

	m_pFrustum->VUpdate();
}

void ICamera::SetLookAt(const XMFLOAT3 &vTo)
{
	XMFLOAT3 v;
	XMStoreFloat3(&v, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&vTo), XMLoadFloat3(&m_vPos))));

	m_fYaw = XMConvertToDegrees(atan2(v.x, v.z));
	m_fPitch = XMConvertToDegrees(acosf(v.y)) - 90.0f;
}

void ICamera::SetLookAt(const XMVECTOR &xvTo)
{
	XMFLOAT3 v;
	XMStoreFloat3(&v, XMVector3Normalize(XMVectorSubtract(xvTo, XMLoadFloat3(&m_vPos))));

	m_fYaw = XMConvertToDegrees(atan2(v.x, v.z));
	m_fPitch = XMConvertToDegrees(acosf(v.y)) - 90.0f;
}

void ICamera::SetFrontVector(const XMVECTOR &xvFrontVector)
{
	XMFLOAT3 v;
	XMStoreFloat3(&v, XMVector3Normalize(xvFrontVector));

	m_fYaw = XMConvertToDegrees(atan2(v.x, v.z));
	m_fPitch = XMConvertToDegrees(acosf(v.y)) - 90.0f;
}

XMFLOAT3 ICamera::GetUpVector() const
{
	return XMFLOAT3(m_mView._12, m_mView._22, m_mView._32);
}

XMFLOAT3 ICamera::GetFrontVector() const
{
	return XMFLOAT3(m_mView._13, m_mView._23, m_mView._33);
}

XMFLOAT3 ICamera::GetRightVector() const
{
	return XMFLOAT3(m_mView._11, m_mView._21, m_mView._31);
}

XMFLOAT3 ICamera::GetLookAt(float fDist) const
{
	XMFLOAT3 vTo = XMFLOAT3(m_mView._13, m_mView._23, m_mView._33);

	XMStoreFloat3(&vTo, 
		XMVectorScale(XMVector3Normalize(XMLoadFloat3(&vTo)), fDist) + XMLoadFloat3(&m_vPos));

	return vTo;
}

void ICamera::SetFreeze(bool const _bFreezeState)
{
	m_bIsFreezed = _bFreezeState;

	// if freezed - clear all input
	if (m_bIsFreezed)
	{
		m_MappedKeys.clear();
		m_vMappedMouseDelta = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}

void ICamera::OnKeysInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown)
{
	switch (_eInputEvent.e_event)
	{
	case E_ENGINE_INPUT_EVENTS::KEY_INC_ROLL:
	case E_ENGINE_INPUT_EVENTS::KEY_DEC_ROLL:
	case E_ENGINE_INPUT_EVENTS::KEY_INC_MOVE_SPEED:
	case E_ENGINE_INPUT_EVENTS::KEY_DEC_MOVE_SPEED:
		m_MappedKeys[_eInputEvent.e_event] = _bKeyDown;
	};

	if (m_bIsFreezed)
		return;

	switch (_eInputEvent.e_event)
	{
	case E_ENGINE_INPUT_EVENTS::KEY_ACCELERATE_MOVE:
	case E_ENGINE_INPUT_EVENTS::KEY_MOVE_FRONT:
	case E_ENGINE_INPUT_EVENTS::KEY_MOVE_BACK:
	case E_ENGINE_INPUT_EVENTS::KEY_MOVE_LEFT:
	case E_ENGINE_INPUT_EVENTS::KEY_MOVE_RIGHT:
	case E_ENGINE_INPUT_EVENTS::KEY_MOVE_UP:
	case E_ENGINE_INPUT_EVENTS::KEY_MOVE_DOWN:
		m_MappedKeys[_eInputEvent.e_event] = _bKeyDown;
	};
}

void ICamera::OnMouseInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDelta, XMINT2 const &_vGlobalPos)
{
	if (m_bIsFreezed)
		return;

	m_vMappedMouseDelta.x = (float)_vDelta.x;
	m_vMappedMouseDelta.y = (float)_vDelta.y;
	m_vMappedMouseDelta.z = 0;
}

// implementation of first person camera

void CCameraFirstPerson::VUpdate()
{
	const XMVECTORF32 xvFrontBack = { m_mView._13, m_mView._23, m_mView._33 };
	const XMVECTORF32 xvLeftRight = { m_mView._11, 0.0f, m_mView._31 };
	const XMVECTORF32 xvUpDown = { 0.0f, 1.0f, 0.0f };

	float fElapsedTime = (float)CMainLoopHandler::GetInstance()->GetLogicElapsedTime() * 0.001f;

	XMVECTOR xvMovingDir = XMVectorZero();

	float fAdding = 0.1f;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_INC_ROLL])
		m_vMappedMouseDelta.z += fAdding;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_DEC_ROLL])
		m_vMappedMouseDelta.z -= fAdding;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_INC_MOVE_SPEED] && m_fMoveSpeed < 10000.0f)
		m_fMoveSpeed += fAdding;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_DEC_MOVE_SPEED] && m_fMoveSpeed > 0.01f)
		m_fMoveSpeed -= fAdding;

	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_FRONT])
		xvMovingDir += xvFrontBack;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_BACK])
		xvMovingDir -= xvFrontBack;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_UP])
		xvMovingDir += xvUpDown;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_DOWN])
		xvMovingDir -= xvUpDown;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_RIGHT])
		xvMovingDir += xvLeftRight;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_LEFT])
		xvMovingDir -= xvLeftRight;

	xvMovingDir = XMVector3Normalize(xvMovingDir);

	xvMovingDir *= m_fMoveSpeed;

	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_ACCELERATE_MOVE])
		xvMovingDir *= 10.0f;

	// moving
	XMVECTOR xvVelocity = XMLoadFloat3(&m_vMoveVelocity);

	if (XMVectorGetX(XMVector3LengthSq(xvMovingDir)) > 0.0f)
	{
		xvVelocity = xvMovingDir;
		m_fDragTimer = 0.25f;
		if (m_fDragTimer < fElapsedTime)
			m_fDragTimer = fElapsedTime;

		m_xvVelocityDrag = xvMovingDir / (float)m_fDragTimer;
	}
	else
	{
		if (m_fDragTimer > 0.0f)
		{
			xvVelocity -= m_xvVelocityDrag * fElapsedTime;
			m_fDragTimer -= fElapsedTime;
		}
		else
		{
			xvVelocity = XMVectorZero();
		}
	}

	XMStoreFloat3(&m_vMoveVelocity, xvVelocity);

	XMStoreFloat3(&m_vPos, XMLoadFloat3(&m_vPos) + xvVelocity * fElapsedTime);

	// rotating
	m_vCurrentMouseDelta.x = m_vCurrentMouseDelta.x * m_fPercentOfOld + m_vMappedMouseDelta.x * m_fPercentOfNew;
	m_vCurrentMouseDelta.y = m_vCurrentMouseDelta.y * m_fPercentOfOld + m_vMappedMouseDelta.y * m_fPercentOfNew;
	m_vCurrentMouseDelta.z = m_vCurrentMouseDelta.z * m_fPercentOfOld + m_vMappedMouseDelta.z * m_fPercentOfNew;

	m_fYaw += m_vCurrentMouseDelta.x * m_fRotateSpeed;
	m_fPitch += m_vCurrentMouseDelta.y * m_fRotateSpeed;
	m_fRoll += m_vCurrentMouseDelta.z * m_fRotateSpeed;

	m_fPitch = fmod(m_fPitch, 360.0f);
	// limit pitch to straight up or down
	m_fPitch = std::min(+90.0f, m_fPitch);
	m_fPitch = std::max(-90.0f, m_fPitch);

	m_fRoll = fmod(m_fRoll, 360.0f);
	m_fYaw = fmod(m_fYaw, 360.0f);

	UpdateMatrices();

	m_vMappedMouseDelta = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

// TODO!!
void CCameraThirdPerson::VUpdate()
{
	const XMVECTORF32 xvFrontBack = { m_mView._13, m_mView._23, m_mView._33 };
	const XMVECTORF32 xvLeftRight = { m_mView._11, 0.0f, m_mView._31 };
	const XMVECTORF32 xvUpDown = { 0.0f, 1.0f, 0.0f };

	float fElapsedTime = (float)CMainLoopHandler::GetInstance()->GetLogicElapsedTime() * 0.001f;

	XMVECTOR xvMovingDir = XMVectorZero();

	float fAddingRot = 0.1f;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_INC_ROLL])
		m_vMappedMouseDelta.z += fAddingRot;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_DEC_ROLL])
		m_vMappedMouseDelta.z -= fAddingRot;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_INC_MOVE_SPEED] && m_fMoveSpeed < 100.0f)
		m_fMoveSpeed += 0.01f;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_DEC_MOVE_SPEED] && m_fMoveSpeed > 0.1f)
		m_fMoveSpeed -= 0.01f;

	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_FRONT])
		xvMovingDir += xvFrontBack;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_BACK])
		xvMovingDir -= xvFrontBack;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_UP])
		xvMovingDir += xvUpDown;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_DOWN])
		xvMovingDir -= xvUpDown;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_RIGHT])
		xvMovingDir += xvLeftRight;
	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_MOVE_LEFT])
		xvMovingDir -= xvLeftRight;

	xvMovingDir = XMVector3Normalize(xvMovingDir);

	xvMovingDir *= m_fMoveSpeed;

	if (m_MappedKeys[E_ENGINE_INPUT_EVENTS::KEY_ACCELERATE_MOVE])
		xvMovingDir *= 10.0f;

	// moving
	XMVECTOR xvVelocity = XMLoadFloat3(&m_vMoveVelocity);

	if (XMVectorGetX(XMVector3LengthSq(xvMovingDir)) > 0.0f)
	{
		xvVelocity = xvMovingDir;
		m_fDragTimer = 0.25f;
		if (m_fDragTimer < fElapsedTime)
			m_fDragTimer = fElapsedTime;

		m_xvVelocityDrag = xvMovingDir / (float)m_fDragTimer;
	}
	else
	{
		if (m_fDragTimer > 0.0f)
		{
			xvVelocity -= m_xvVelocityDrag * fElapsedTime;
			m_fDragTimer -= fElapsedTime;
		}
		else
		{
			xvVelocity = XMVectorZero();
		}
	}

	XMStoreFloat3(&m_vMoveVelocity, xvVelocity);

	XMStoreFloat3(&m_vPos, XMLoadFloat3(&m_vPos) + xvVelocity * fElapsedTime);

	// rotating
	m_vCurrentMouseDelta.x = m_vCurrentMouseDelta.x * m_fPercentOfOld + m_vMappedMouseDelta.x * m_fPercentOfNew;
	m_vCurrentMouseDelta.y = m_vCurrentMouseDelta.y * m_fPercentOfOld + m_vMappedMouseDelta.y * m_fPercentOfNew;
	m_vCurrentMouseDelta.z = m_vCurrentMouseDelta.z * m_fPercentOfOld + m_vMappedMouseDelta.z * m_fPercentOfNew;

	m_fYaw += m_vCurrentMouseDelta.x * m_fRotateSpeed;
	m_fPitch += m_vCurrentMouseDelta.y * m_fRotateSpeed;
	m_fRoll += m_vCurrentMouseDelta.z * m_fRotateSpeed;

	m_fPitch = fmod(m_fPitch, 360.0f);
	// limit pitch to straight up or down
	m_fPitch = std::min(+90.0f, m_fPitch);
	m_fPitch = std::max(-90.0f, m_fPitch);

	m_fRoll = fmod(m_fRoll, 360.0f);
	m_fYaw = fmod(m_fYaw, 360.0f);

	UpdateMatrices();

	m_vMappedMouseDelta = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

/*
A, B, C, D - components of equation plane of reflect
A, B, C - normal
D = -(A * x2 + B * y2 + C * z2)
где x2, y2, z2 - point on plane
xc, yc, zc - camera point
x0, y0, z0 - point of reflected camera
t = - (D + xc * A + yc * B + zc * C) / (A^2 + B^2 + C^2)
if normal normalized - delete code of divide

x0 = 2 * A * t + xc
y0 = 2 * B * t + yc
z0 = 2 * C * t + zc
*/

ICamera *ReflectCamera(const ICamera *pCamera, const XMFLOAT4 &plPlane)
{
	XMVECTOR xvPlane = XMLoadFloat4(&plPlane);
	XMVECTOR xvCamPos = XMLoadFloat3(&pCamera->GetPosition());
	float fTempPos = (-XMVectorGetX(XMPlaneDotCoord(xvPlane, xvCamPos))) * 2.0f;
	xvCamPos = xvPlane * fTempPos + xvCamPos;
	XMFLOAT3 vCamPos;
	XMStoreFloat3(&vCamPos, xvCamPos);

	XMVECTOR xvCamLookAt = XMLoadFloat3(&pCamera->GetLookAt());
	float fTempLookAt = (-XMVectorGetX(XMPlaneDotCoord(xvPlane, xvCamLookAt))) * 2.0f;
	xvCamLookAt = xvPlane * fTempLookAt + xvCamLookAt;
	xvCamLookAt = XMVector3Normalize(xvCamLookAt - xvCamPos);
	XMFLOAT3 vCamLookAt;
	XMStoreFloat3(&vCamLookAt, xvCamLookAt);
	vCamLookAt = XMFLOAT3(XMConvertToDegrees(atan2(vCamLookAt.x, vCamLookAt.z)), XMConvertToDegrees(acos(vCamLookAt.y)) - 90.0f, 0.0f);

	ICamera *pReflectedCam = new CCameraFirstPerson(vCamPos, vCamLookAt);
	return pReflectedCam;
}

void ScreenToWorld(const ICamera *pCamera, XMINT2 const &vScreenPos, OUT XMVECTOR &xvRayDir, OUT XMVECTOR &xvRayOrigin)
{
	XMFLOAT3 vRayDir;
	XMFLOAT3 vRayOrigin;

	ScreenToWorld(pCamera, vScreenPos, vRayDir, vRayOrigin);

	xvRayDir = XMLoadFloat3(&vRayDir);
	xvRayOrigin = XMLoadFloat3(&vRayOrigin);
}

void ScreenToWorld(const ICamera *pCamera, XMINT2 const &vScreenPos, OUT XMFLOAT3 &vRayDir, OUT XMFLOAT3 &vRayOrigin)
{
	XMFLOAT3 vTmpDir;
	vTmpDir.x = (float)vScreenPos.x;
	vTmpDir.y = (float)vScreenPos.y;
	vTmpDir.z = 1.0f;

	// convert to projection space:

	// 2.0f * X / w - 1 = x
	// 1 - 2.0f * Y / h = y
	vTmpDir.x = 2.0f * vTmpDir.x / CSettingsHandler::GetWindowWidth() - 1.0f;
	vTmpDir.y = 1.0f - 2.0f * vTmpDir.y / CSettingsHandler::GetWindowHeight();

	// convert to view space:
	XMMATRIX const &mProj = pCamera->GetProjMatrix();
	vTmpDir.x /= mProj._11;
	vTmpDir.y /= mProj._22;

	// convert to world space:
	XMMATRIX const &mInvView = XMMatrixInverse(nullptr, pCamera->GetViewMatrix());

	vRayDir.x = vTmpDir.x * mInvView._11 + vTmpDir.y * mInvView._21 + vTmpDir.z * mInvView._31;
	vRayDir.y = vTmpDir.x * mInvView._12 + vTmpDir.y * mInvView._22 + vTmpDir.z * mInvView._32;
	vRayDir.z = vTmpDir.x * mInvView._13 + vTmpDir.y * mInvView._23 + vTmpDir.z * mInvView._33;

	vRayOrigin.x = vRayDir.x + mInvView._41;
	vRayOrigin.y = vRayDir.y + mInvView._42;
	vRayOrigin.z = vRayDir.z + mInvView._43;

	XMVECTOR xvRayOrigin = XMLoadFloat3(&vRayOrigin);
	XMStoreFloat3(&vRayOrigin, xvRayOrigin);

	XMVECTOR xvRayDir = XMLoadFloat3(&vRayDir);
	xvRayDir = XMVector3Normalize(xvRayDir);
	XMStoreFloat3(&vRayDir, xvRayDir);
}