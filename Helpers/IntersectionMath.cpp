#include "EnginePCH.h"

#include "IntersectionMath.h"
#include "Camera.h"

// return: the closest point on [AB] from P
XMFLOAT3 GetClosestPointLine(const XMFLOAT3 *_pvA, const XMFLOAT3 *_pvB, const XMFLOAT3 *_pvP)
{
	XMVECTOR
		xvA = XMLoadFloat3(_pvA),
		xvAB = XMVectorSubtract(XMLoadFloat3(_pvB), xvA);

	float
		fTmp = XMVectorGetX(XMVector3Dot(XMVectorSubtract(XMLoadFloat3(_pvP), xvA), xvAB)) / XMVectorGetX(XMVector3Dot(xvAB, xvAB));

	if (fTmp < 0.0f)
		fTmp = 0.0f;
	else if (fTmp > 1.0f)
		fTmp = 1.0f;

	XMFLOAT3 vRet;

	XMStoreFloat3(&vRet, XMVectorAdd(xvA, XMVectorScale(xvAB, fTmp)));

	return vRet;
}

bool IsPointInSphere(const XMFLOAT3 *_pvCenter, const float _fRad, const XMFLOAT3 *_pvPoint)
{
	XMVECTOR
		xvPQ = XMVectorSubtract(XMLoadFloat3(_pvPoint), XMLoadFloat3(_pvCenter));

	return (XMVectorGetX(XMVector3Dot(xvPQ, xvPQ)) <= _fRad * _fRad);
}


E_INTERSECTION TestAABBAABB(const CAABB& aabb1, const CAABB& aabb2)
{
	XMVECTOR T;
	T = XMVectorAbs(XMVectorSubtract(XMLoadFloat3(&aabb2.GetPosition()), XMLoadFloat3(&aabb1.GetPosition())));

	XMVECTOR r;
	r = XMVectorAdd(XMLoadFloat3(&aabb2.GetHalfSize()), XMLoadFloat3(&aabb1.GetHalfSize()));

	if (XMVector3LessOrEqual(T, r))
	{
		XMVECTOR r1;
		r1 = XMVectorSubtract(XMLoadFloat3(&aabb1.GetHalfSize()), XMLoadFloat3(&aabb2.GetHalfSize()));
		if (XMVector3LessOrEqual(T, r1))
		{
			return E_INTERSECTION::INSIDE;
		}
		return E_INTERSECTION::INTERSECT;
	}
	return E_INTERSECTION::OUTSIDE;
}

E_INTERSECTION TestAABBFrustum(const CAABB& aabb, CFrustum *pFrustum)
{
	//return I_INTERSECT;
	float m, n;
	E_INTERSECTION result = E_INTERSECTION::INSIDE;
	XMFLOAT3 v[2];
	v[0] = aabb.GetMin();
	v[1] = aabb.GetMax();

	for (int i = 0; i < 6; i++)
	{
		const XMFLOAT4 *pPlane = &pFrustum->GetPlanes()[i];

		m = (pPlane->x * v[pPlane->x > 0].x) + (pPlane->y * v[pPlane->y > 0].y) + (pPlane->z * v[pPlane->z > 0].z);
		if (m < -pPlane->w)
			return E_INTERSECTION::OUTSIDE;

		n = (pPlane->x * v[pPlane->x < 0].x) + (pPlane->y * v[pPlane->y < 0].y) + (pPlane->z * v[pPlane->z < 0].z);
		if (n < -pPlane->w)
			result = E_INTERSECTION::INTERSECT;
	}
	return result;
}


E_INTERSECTION TestSphereTriangle(const CBSphere& s1, const XMFLOAT3 s2[3])
{
	XMVECTOR	vA = XMLoadFloat3(&s2[0]),
		vB = XMLoadFloat3(&s2[1]),
		vC = XMLoadFloat3(&s2[2]),
		vP = XMLoadFloat3(&s1.GetCenter());

	vA = XMVectorSubtract(vA, vP);
	vB = XMVectorSubtract(vB, vP);
	vC = XMVectorSubtract(vC, vP);

	float fRR = s1.GetRadius();
	fRR *= fRR;

	XMVECTOR vV = XMVector3Cross(XMVectorSubtract(vB, vA), XMVectorSubtract(vC, vA));

	float	fD = XMVectorGetX(XMVector3Dot(vA, vV)),
		fE = XMVectorGetX(XMVector3Dot(vV, vV));

	if (fD * fD > fRR * fE)
		return E_INTERSECTION::INTERSECT;

	float	fAA = XMVectorGetX(XMVector3Dot(vA, vA)),
		fAB = XMVectorGetX(XMVector3Dot(vA, vB)),
		fAC = XMVectorGetX(XMVector3Dot(vA, vC)),
		fBB = XMVectorGetX(XMVector3Dot(vB, vB)),
		fBC = XMVectorGetX(XMVector3Dot(vB, vC)),
		fCC = XMVectorGetX(XMVector3Dot(vC, vC));

	if ((fAA > fRR) & (fAB > fAA) & (fAC > fAA))
		return E_INTERSECTION::INTERSECT;

	if ((fBB > fRR) & (fAB > fBB) & (fBC > fBB))
		return E_INTERSECTION::INTERSECT;

	if ((fCC > fRR) & (fAC > fCC) & (fBC > fCC))
		return E_INTERSECTION::INTERSECT;

	XMVECTOR	vAB = XMVectorSubtract(vB, vA),
		vBC = XMVectorSubtract(vC, vB),
		vCA = XMVectorSubtract(vA, vC);

	float	fD1 = fAB - fAA,
		fD2 = fBC - fBB,
		fD3 = fAC - fCC,
		fE1 = XMVectorGetX(XMVector3Dot(vAB, vAB)),
		fE2 = XMVectorGetX(XMVector3Dot(vBC, vBC)),
		fE3 = XMVectorGetX(XMVector3Dot(vCA, vCA));

	XMVECTOR	vQ1 = XMVectorSubtract(XMVectorScale(vA, fE1), XMVectorScale(vAB, fD1)),
		vQ2 = XMVectorSubtract(XMVectorScale(vB, fE2), XMVectorScale(vBC, fD2)),
		vQ3 = XMVectorSubtract(XMVectorScale(vC, fE3), XMVectorScale(vCA, fD3)),
		vQC = XMVectorSubtract(XMVectorScale(vC, fE1), vQ1),
		vQA = XMVectorSubtract(XMVectorScale(vA, fE2), vQ2),
		vQB = XMVectorSubtract(XMVectorScale(vB, fE3), vQ3);

	if ((XMVectorGetX(XMVector3Dot(vQ1, vQ1)) > fRR * fE1 * fE1) & (XMVectorGetX(XMVector3Dot(vQ1, vQC)) > 0))
		return E_INTERSECTION::INTERSECT;

	if ((XMVectorGetX(XMVector3Dot(vQ2, vQ2)) > fRR * fE2 * fE2) & (XMVectorGetX(XMVector3Dot(vQ2, vQA)) > 0))
		return E_INTERSECTION::INTERSECT;

	if ((XMVectorGetX(XMVector3Dot(vQ3, vQ3)) > fRR * fE3 * fE3) & (XMVectorGetX(XMVector3Dot(vQ3, vQB)) > 0))
		return E_INTERSECTION::INTERSECT;

	return E_INTERSECTION::OUTSIDE;
}

E_INTERSECTION TestSphereSphere(const CBSphere& s1, const CBSphere& s2)
{
	float fDest = XMVectorGetX(XMVector3Length(XMLoadFloat3(&s1.GetCenter()) - XMLoadFloat3(&s2.GetCenter())));
	if (fDest <= s1.GetRadius() + s2.GetRadius())
	{
		if (fDest <= s1.GetRadius() - s2.GetRadius())
			return E_INTERSECTION::INSIDE;
		return E_INTERSECTION::INTERSECT;
	}
	return E_INTERSECTION::OUTSIDE;
}

E_INTERSECTION TestSphereFrustum(const CBSphere& s, CFrustum *pFrustum)
{
	float fDest;
	float fRadius = s.GetRadius() - 100.0f;
	const XMVECTOR xvCenter = XMLoadFloat3(&s.GetCenter());
	register E_INTERSECTION result = E_INTERSECTION::INSIDE;
	for (register DWORD i = 0; i < 6; i++)
	{
		XMFLOAT3 xvTmp = XMFLOAT3(pFrustum->GetPlanes()[i].x, pFrustum->GetPlanes()[i].y, pFrustum->GetPlanes()[i].z);
		fDest = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&xvTmp), xvCenter)) + pFrustum->GetPlanes()[i].w;
		// fDest [-inf; -fRad] vs [-inf; fRad];
		if (fDest < -fRadius)
			return E_INTERSECTION::OUTSIDE;
		//else if (fDest < fRadius)
		//	result = I_INTERSECT;
		//return I_INTERSECT;
	}
	return result;
}

E_INTERSECTION TestSphereAABB(const CBSphere& s, const CAABB& aabb)
{
	float
		d = 0.0f,
		a,
		m = 0.0f;
	bool bm = true;
	const XMFLOAT3& c = s.GetCenter();
	XMFLOAT3	vMin = aabb.GetMin(),
		vMax = aabb.GetMax(),
		vBC = aabb.GetPosition();
	E_INTERSECTION result = E_INTERSECTION::INSIDE;
	// проходим по осям X,Y,Z

	if (c.x < vMin.x)
	{
		a = c.x - vMin.x;
		d += a * a;
		result = E_INTERSECTION::INTERSECT;
	}
	else if (c.x > vMax.x)
	{
		a = c.x - vMax.x;
		d += a * a;
		result = E_INTERSECTION::INTERSECT;
	}
	else if (result == E_INTERSECTION::INSIDE)
	{
		a = (c.x < vBC.x) ? fabs(c.x - vMin.x) : fabs(c.x - vMax.x);
		if (a < m || bm)
		{
			bm = false;
			m = a;
		}
	}


	if (c.y < vMin.y)
	{
		a = c.y - vMin.y;
		d += a * a;
		result = E_INTERSECTION::INTERSECT;
	}
	else if (c.y > vMax.y)
	{
		a = c.y - vMax.y;
		d += a * a;
		result = E_INTERSECTION::INTERSECT;
	}
	else if (result == E_INTERSECTION::INSIDE)
	{
		a = (c.y < vBC.y) ? fabs(c.y - vMin.y) : fabs(c.y - vMax.y);
		if (a < m || bm)
		{
			bm = false;
			m = a;
		}
	}


	if (c.z < vMin.z)
	{
		a = c.z - vMin.z;
		d += a * a;
		result = E_INTERSECTION::INTERSECT;
	}
	else if (c.z > vMax.z)
	{
		a = c.z - vMax.z;
		d += a * a;
		result = E_INTERSECTION::INTERSECT;
	}
	else if (result == E_INTERSECTION::INSIDE)
	{
		a = (c.z < vBC.z) ? fabs(c.z - vMin.z) : fabs(c.z - vMax.z);
		if (a < m || bm)
		{
			bm = false;
			m = a;
		}
	}

	float r = s.GetRadius();
	if (result == E_INTERSECTION::INSIDE)
	{
		if (r <= m)
			return E_INTERSECTION::INSIDE;
		return E_INTERSECTION::INTERSECT;
	}
	else if (d <= r * r)
	{
		return E_INTERSECTION::INTERSECT;
	}
	return E_INTERSECTION::OUTSIDE;
}


float GetAABBDest(const CAABB& aabb, const XMFLOAT3& c)
{
	float	d = 0.0f,
		a,
		m = 0.0f;
	bool	bm = true,
		res = true;

	XMFLOAT3	vMin = aabb.GetMin(),
		vMax = aabb.GetMax(),
		vBC = aabb.GetPosition();

	// проходим по осям X,Y,Z

	if (c.x < vMin.x)
	{
		a = c.x - vMin.x;
		d += a * a;
		res = false;
	}
	else if (c.x > vMax.x)
	{
		a = c.x - vMax.x;
		d += a * a;
		res = false;
	}
	else if (res)
	{
		a = (c.x < vBC.x) ? fabs(c.x - vMin.x) : fabs(c.x - vMax.x);
		if (a < m || bm)
		{
			bm = false;
			m = a;
		}
	}


	if (c.y < vMin.y)
	{
		a = c.y - vMin.y;
		d += a * a;
		res = false;
	}
	else if (c.y > vMax.y)
	{
		a = c.y - vMax.y;
		d += a * a;
		res = false;
	}
	else if (res)
	{
		a = (c.y < vBC.y) ? fabs(c.y - vMin.y) : fabs(c.y - vMax.y);
		if (a < m || bm)
		{
			bm = false;
			m = a;
		}
	}


	if (c.z < vMin.z)
	{
		a = c.z - vMin.z;
		d += a * a;
		res = false;
	}
	else if (c.z > vMax.z)
	{
		a = c.z - vMax.z;
		d += a * a;
		res = false;
	}
	else if (res)
	{
		a = (c.z < vBC.z) ? fabs(c.z - vMin.z) : fabs(c.z - vMax.z);
		if (a < m || bm)
		{
			bm = false;
			m = a;
		}
	}

	if (res) return -m;
	return sqrtf(d);
}


bool IntersectRayAABB(
	const XMFLOAT3 *_pvRayOrg, const XMFLOAT3 *_pvRayDir,
	const XMFLOAT3 *_pvAABBMin, const XMFLOAT3 *_pvAABBMax,
	OUT float &fLengthToInterection)
{
	XMVECTOR
		xvDirFrac = XMVectorReplicate(1.0f) / XMLoadFloat3(_pvRayDir),
		xvRayOrg = XMLoadFloat3(_pvRayOrg),
		xvTmp1 = (XMLoadFloat3(_pvAABBMin) - xvRayOrg) * xvDirFrac,
		xvTmp2 = (XMLoadFloat3(_pvAABBMax) - xvRayOrg) * xvDirFrac;

	float fMin = std::max(
			std::max(
			std::min(xvTmp1.vector4_f32[0], xvTmp2.vector4_f32[0]),
			std::min(xvTmp1.vector4_f32[1], xvTmp2.vector4_f32[1])
		), std::min(xvTmp1.vector4_f32[2], xvTmp2.vector4_f32[2]));
	float fMax = std::min(
			std::min(
			std::max(xvTmp1.vector4_f32[0], xvTmp2.vector4_f32[0]),
			std::max(xvTmp1.vector4_f32[1], xvTmp2.vector4_f32[1])
		), std::max(xvTmp1.vector4_f32[2], xvTmp2.vector4_f32[2]));

	if (fMax < 0)
	{
		fLengthToInterection = fMax;
		return false;
	}

	if (fMin > fMax)
	{
		fLengthToInterection = fMax;
		return false;
	}

	fLengthToInterection = fMin;
	return true;
}
/*
bool IntersectRaySphere(
const XMFLOAT3 *_pvRayOrg, const XMFLOAT3 *_pvRayDir,
const XMFLOAT3 *_pvCenter, const float _fRad)
{
return IsPointInSphere(_pvCenter, _fRad, &GetClosestPointVector(_pvRayOrg, _pvRayDir, _pvCenter));
}
*/
bool IntersectRayTriangle(
	const XMFLOAT3 *_pvRayOrg, const XMFLOAT3 *_pvRayDir,
	const XMFLOAT3 *_pvV0, const XMFLOAT3 *_pvV1, const XMFLOAT3 *_pvV2,
	OUT float &fU, OUT float &fV, OUT float &fT)
{
	XMVECTOR
		xvV0 = XMLoadFloat3(_pvV0),
		xvV1 = XMLoadFloat3(_pvV1),
		xvV2 = XMLoadFloat3(_pvV2),
		xvRayDir = XMLoadFloat3(_pvRayDir),
		xvRayOrg = XMLoadFloat3(_pvRayOrg),

		xvE1 = xvV1 - xvV0,
		xvE2 = xvV2 - xvV0,
		xvH = XMVector3Cross(xvRayDir, xvE2);

	float
		fA = XMVectorGetX(XMVector3Dot(xvE1, xvH));

	if (fA > -0.00001f && fA < 0.00001f)
		return false;

	float
		fF = 1.0f / fA;

	XMVECTOR
		xvS = XMVectorSubtract(xvRayOrg, xvV0);

	fU = fF * XMVectorGetX(XMVector3Dot(xvS, xvH));

	if (fU < 0.0f || fU > 1.0f)
		return false;

	XMVECTOR
		xvQ = XMVector3Cross(xvS, xvE1);

	fV = fF * XMVectorGetX(XMVector3Dot(xvRayDir, xvQ));

	if (fV < 0.0f || fV + fU > 1.0f)
		return false;

	fT = fF * XMVectorGetX(XMVector3Dot(xvE2, xvQ));

	return (fT > 0.0001f);
}

XMFLOAT3 LineLineIntersection(
	XMVECTOR xvOriginD, XMVECTOR xvDirD,
	XMVECTOR xvOriginE, XMVECTOR xvDirE)
{
	xvDirD = XMVector3Normalize(xvDirD);
	xvDirE = XMVector3Normalize(xvDirE);

	XMVECTOR N = XMVector3Cross(xvDirD, xvDirE);
	XMVECTOR SR = xvOriginD - xvOriginE;

	float fAbsX = fabs(XMVectorGetX(N));
	float fAbsY = fabs(XMVectorGetY(N));
	float fAbsZ = fabs(XMVectorGetZ(N));

	float t;

	// 
	if (fAbsY > fAbsX && fAbsY > fAbsZ)
	{
		t = (XMVectorGetX(SR) * XMVectorGetY(xvDirE) - XMVectorGetY(SR) * XMVectorGetX(xvDirE)) / XMVectorGetZ(N);
	}
	else if (fAbsX > fAbsZ)
	{
		t = (XMVectorGetY(SR) * XMVectorGetZ(xvDirE) - XMVectorGetZ(SR) * XMVectorGetY(xvDirE)) / XMVectorGetX(N);
	}
	else
	{
		t = (XMVectorGetZ(SR) * XMVectorGetX(xvDirE) - XMVectorGetX(SR) * XMVectorGetZ(xvDirE)) / XMVectorGetY(N);
	}

	XMVECTOR xvRes = xvOriginD - t * xvDirD;
	XMFLOAT3 vRes;
	XMStoreFloat3(&vRes, xvRes);

	return vRes;
}

XMFLOAT3 LineLineIntersection(
	XMFLOAT3 const &vOriginD, XMFLOAT3 const &vDirD,
	XMFLOAT3 const &vOriginE, XMFLOAT3 const &vDirE)
{
	XMVECTOR xvOriginD = XMLoadFloat3(&vOriginD);
	XMVECTOR xvDirD = XMLoadFloat3(&vDirD);
	XMVECTOR xvOriginE = XMLoadFloat3(&vOriginE);
	XMVECTOR xvDirE = XMLoadFloat3(&vDirE);

	return LineLineIntersection(xvOriginD, xvDirD, xvOriginE, xvDirE);
}