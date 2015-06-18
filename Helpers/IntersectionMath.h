#pragma once

#include "EnginePCH.h"

class CFrustum;

enum class E_INTERSECTION : DWORD
{
	OUTSIDE = 0,
	INTERSECT,
	INSIDE
};

class CBSphere
{
public:
	CBSphere()
	{
		m_BSphere.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_BSphere.Radius = 0.0f;
	}

	CBSphere(const XMFLOAT3& _vCenter, float _fRadius)
	{
		m_BSphere.Center = _vCenter;
		m_BSphere.Radius = _fRadius;
	}

	CBSphere(const XMVECTOR& _xvCenter, float _fRadius)
	{
		SetPosition(_xvCenter);
		m_BSphere.Radius = _fRadius;
	}

	CBSphere &operator=(CBSphere const & inst)
	{
		m_BSphere.Center = inst.GetCenter();
		m_BSphere.Radius = inst.GetRadius();

		return *this;
	};

	void Transform(XMMATRIX const &_mTransform)
	{
		m_BSphere.Transform(m_BSphere, _mTransform);
	}

	void SetPosition(const XMFLOAT3& _vCenter)
	{
		m_BSphere.Center = _vCenter;
	}

	void SetPosition(const XMVECTOR& _xvCenter)
	{
		XMStoreFloat3(&m_BSphere.Center, _xvCenter);
	}

	void SetRadius(float _fRadius)
	{
		m_BSphere.Radius = _fRadius;
	}

	const XMFLOAT3& GetCenter() const
	{
		return m_BSphere.Center;
	}

	float GetRadius() const
	{
		return m_BSphere.Radius;
	}
protected:
	DirectX::BoundingSphere m_BSphere;
};

class CAABB
{
public:
	CAABB()
	{
		m_AABB.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_AABB.Extents = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	CAABB(const CAABB& inst)
	{
		m_AABB.Center = inst.m_AABB.Center;
		m_AABB.Extents = inst.m_AABB.Extents;
	}

	CAABB(const XMFLOAT3& _vMin, const XMFLOAT3& _vMax)
	{
		SetMinMax(_vMin, _vMax);
	}

	CAABB(const XMVECTOR& _xvMin, const XMVECTOR& _xvMax)
	{
		SetMinMax(_xvMin, _xvMax);
	}

	CAABB &operator=(CAABB const & inst)
	{
		m_AABB.Center = inst.GetPosition();
		m_AABB.Extents = inst.GetHalfSize();

		return *this;
	};

	void SetPosition(const XMFLOAT3& _vPosition)
	{
		m_AABB.Center = _vPosition;
	}

	void SetHalfSize(const XMFLOAT3& _vHalfSize)
	{
		m_AABB.Extents = _vHalfSize;
	}

	void SetMinMax(const XMFLOAT3& _vMin, const XMFLOAT3& _vMax)
	{
		XMVECTOR xvMin = XMLoadFloat3(&_vMin);
		XMVECTOR xvMax = XMLoadFloat3(&_vMax);

		XMStoreFloat3(&m_AABB.Center, (xvMax + xvMin) * 0.5f);
		XMStoreFloat3(&m_AABB.Extents, (xvMax - xvMin) * 0.5f);
	}

	void Transform(XMMATRIX const &_mTransform)
	{
		m_AABB.Transform(m_AABB, _mTransform);
	}

	void SetPosition(const XMVECTOR& _xvPosition)
	{
		XMStoreFloat3(&m_AABB.Center, _xvPosition);
	}

	void SetHalfSize(const XMVECTOR& _xvHalfSize)
	{
		XMStoreFloat3(&m_AABB.Extents, _xvHalfSize);
	}

	void SetMinMax(const XMVECTOR& _xvMin, const XMVECTOR& _xvMax)
	{
		XMVECTOR xvMin = _xvMin;
		XMVECTOR xvMax = _xvMax;

		XMStoreFloat3(&m_AABB.Center, (xvMax + xvMin) * 0.5f);
		XMStoreFloat3(&m_AABB.Extents, (xvMax - xvMin) * 0.5f);
	}

	const XMFLOAT3& GetPosition() const
	{
		return m_AABB.Center;
	}

	const XMFLOAT3& GetHalfSize() const
	{
		return m_AABB.Extents;
	}

	XMFLOAT3 GetSize() const
	{
		XMFLOAT3 vExt = m_AABB.Extents;
		XMStoreFloat3(&vExt, 2.0f * XMLoadFloat3(&vExt));

		return vExt;
	}

	XMFLOAT3 GetMin() const
	{
		XMFLOAT3 vMin;
		XMStoreFloat3(&vMin, XMLoadFloat3(&m_AABB.Center) - XMLoadFloat3(&m_AABB.Extents));
		return vMin;
	}

	XMFLOAT3 GetMax() const
	{
		XMFLOAT3 vMax;
		XMStoreFloat3(&vMax, XMLoadFloat3(&m_AABB.Center) + XMLoadFloat3(&m_AABB.Extents));
		return vMax;
	}

	float GetDiagonal() const
	{
		return XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&m_AABB.Extents)));
	}

	DirectX::BoundingBox GetAABB() const
	{
		return m_AABB;
	};
protected:
	DirectX::BoundingBox m_AABB;
};

E_INTERSECTION TestSphereSphere(const CBSphere& s1, const CBSphere& s2);
E_INTERSECTION TestAABBAABB(const CAABB& aabb1, const CAABB& aabb2);
E_INTERSECTION TestSphereAABB(const CBSphere& s, const CAABB& aabb);
float GetAABBDest(const CAABB& aabb, const XMFLOAT3& c);
E_INTERSECTION TestSphereTriangle(const CBSphere& s1, const XMFLOAT3 s2[3]);
E_INTERSECTION TestSphereFrustum(const CBSphere& s, CFrustum *pFrustum);
E_INTERSECTION TestAABBFrustum(const CAABB& aabb, CFrustum *pFrustum);

bool IntersectRayTriangle(
	const XMFLOAT3 *_pvRayOrg, const XMFLOAT3 *_pvRayDir,
	const XMFLOAT3 *_pvV0, const XMFLOAT3 *_pvV1, const XMFLOAT3 *_pvV2,
	OUT float &fU, OUT float &fV, OUT float &fT);

bool IntersectRaySphere(
	const XMFLOAT3 *_pvRayOrg, const XMFLOAT3 *_pvRayDir,
	const XMFLOAT3 *_pvCenter, const float _fRad);

bool IntersectRayAABB(
	const XMFLOAT3 *_pvRayOrg, const XMFLOAT3 *_pvRayDir,
	const XMFLOAT3 *_pvAABBMin, const XMFLOAT3 *_pvAABBMax,
	OUT float &fLengthToInterection);

XMFLOAT3 GetClosestPointLine(const XMFLOAT3 *_pvA, const XMFLOAT3 *_pvB, const XMFLOAT3 *_pvP);
bool IsPointInSphere(const XMFLOAT3 *_pvCenter, const float _fRad, const XMFLOAT3 *_pvPoint);

XMFLOAT3 LineLineIntersection(
	XMFLOAT3 const &vOriginD, XMFLOAT3 const &vDirD,
	XMFLOAT3 const &vOriginE, XMFLOAT3 const &vDirE);

XMFLOAT3 LineLineIntersection(
	XMVECTOR xvOriginD, XMVECTOR xvDirD,
	XMVECTOR xvOriginE, XMVECTOR xvDirE);