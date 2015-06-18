#include "EnginePCH.h"

#include "PrimitiveObjects.h"
#include "Camera.h"

void CPlane::VInit()
{
	if (m_pObject)
		m_pObject->SetColor(XMCOLOR(255.0f, 0.0f, 0.0f, 255.0f));
	/*
	IActor::SetPosition(XMVectorReplicate(m_vPlane.w));

	float fDist = m_vPlane.w;
	XMStoreFloat4(&m_vPlane, XMPlaneNormalize(XMLoadFloat4(&m_vPlane)));
	m_vPlane.w = fDist;

	IActor::SetRotation(acos(m_vPlane.y), atan2(m_vPlane.x, m_vPlane.z), 0.0f);
	*/
	XMMATRIX mTransform = XMMatrixRotationRollPitchYaw(acos(m_vPlane.y), atan2(m_vPlane.x, m_vPlane.z), XM_PI / 2);
	mTransform.r[3] = XMVectorReplicate(m_vPlane.w);
	mTransform._44 = 1.0f;

	IActor::VInit();

	SetTransform(mTransform);
};

void CFrustum::VUpdate()
{
	IActor::VUpdate();

	XMMATRIX const &m_mViewProj = m_pCamera->GetViewProjMatrix();

	// left plane
	m_vPlanes[0].x = m_mViewProj._14 + m_mViewProj._11;
	m_vPlanes[0].y = m_mViewProj._24 + m_mViewProj._21;
	m_vPlanes[0].z = m_mViewProj._34 + m_mViewProj._31;
	m_vPlanes[0].w = m_mViewProj._44 + m_mViewProj._41;

	// right plane
	m_vPlanes[1].x = m_mViewProj._14 - m_mViewProj._11;
	m_vPlanes[1].y = m_mViewProj._24 - m_mViewProj._21;
	m_vPlanes[1].z = m_mViewProj._34 - m_mViewProj._31;
	m_vPlanes[1].w = m_mViewProj._44 - m_mViewProj._41;

	// up plane
	m_vPlanes[2].x = m_mViewProj._14 - m_mViewProj._12;
	m_vPlanes[2].y = m_mViewProj._24 - m_mViewProj._22;
	m_vPlanes[2].z = m_mViewProj._34 - m_mViewProj._32;
	m_vPlanes[2].w = m_mViewProj._44 - m_mViewProj._42;

	// down plane
	m_vPlanes[3].x = m_mViewProj._14 + m_mViewProj._12;
	m_vPlanes[3].y = m_mViewProj._24 + m_mViewProj._22;
	m_vPlanes[3].z = m_mViewProj._34 + m_mViewProj._32;
	m_vPlanes[3].w = m_mViewProj._44 + m_mViewProj._42;

	// near plane
	m_vPlanes[4].x = m_mViewProj._13;
	m_vPlanes[4].y = m_mViewProj._23;
	m_vPlanes[4].z = m_mViewProj._33;
	m_vPlanes[4].w = m_mViewProj._43;

	// far plane
	m_vPlanes[5].x = m_mViewProj._14 - m_mViewProj._13;
	m_vPlanes[5].y = m_mViewProj._24 - m_mViewProj._23;
	m_vPlanes[5].z = m_mViewProj._34 - m_mViewProj._33;
	m_vPlanes[5].w = m_mViewProj._44 - m_mViewProj._43;
};

void CFrustum::VRecalcRenderMatrices(float fInterpolation)
{
	XMMATRIX mWorld = XMMatrixInverse(nullptr, m_pCamera->GetViewProjMatrix());

	IActor::SetTransform(mWorld, false);
	IActor::VRecalcRenderMatrices(fInterpolation);
};