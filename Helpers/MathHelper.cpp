#include "EnginePCH.h"
#include "MathHelper.h"

CCircleBy3Points::CCircleBy3Points(XMFLOAT3 const &vA, XMFLOAT3 const &vB, XMFLOAT3 const &vC)
{
	XMVECTOR A = XMLoadFloat3(&vA);
	XMVECTOR B = XMLoadFloat3(&vB);
	XMVECTOR C = XMLoadFloat3(&vC);

	XMVECTOR AB = B - A;
	XMVECTOR AC = C - A;
	XMVECTOR N = XMVector3Cross(AB, AC);

	XMVECTOR xvToSphereCenter =
		(XMVector3Cross(N, AB) * XMVector3LengthSq(AC) + XMVector3Cross(AC, N) * XMVector3LengthSq(AB)) / (2.0f * XMVector3LengthSq(N));

	m_fRadius = XMVectorGetX(XMVector3Length(xvToSphereCenter));
	XMStoreFloat3(&m_Center, A + xvToSphereCenter);

	/*
	Vector3f a,b,c // are the 3 pts of the tri

	Vector3f ac = c - a ;
	Vector3f ab = b - a ;
	Vector3f abXac = ab.cross( ac ) ;

	// this is the vector from a TO the circumsphere center
	Vector3f toCircumsphereCenter = (abXac.cross( ab )*ac.len2() + ac.cross( abXac )*ab.len2()) / (2.f*abXac.len2()) ;
	float circumsphereRadius = toCircumsphereCenter.len() ;

	// The 3 space coords of the circumsphere center then:
	Vector3f ccs = a  +  toCircumsphereCenter ; // now this is the actual 3space location
	*/
}

XMFLOAT3 const &CCircleBy3Points::GetCenter() const
{
	return m_Center;
}

float CCircleBy3Points::GetRadius() const
{
	return m_fRadius;
}

CCatmullRom::CCatmullRom()
{
	m_vFactor.resize(4);
}

void CCatmullRom::SetPoints(XMFLOAT3 const &_v1, XMFLOAT3 const &_v2, XMFLOAT3 const &_v3, XMFLOAT3 const &_v4)
{
	XMVECTOR m0 = XMLoadFloat3(&_v1);
	XMVECTOR p0 = XMLoadFloat3(&_v2);
	XMVECTOR p1 = XMLoadFloat3(&_v3);
	XMVECTOR m1 = XMLoadFloat3(&_v4);

	m_vFactor[0] = (p0 - p1) * 1.5f + (m1 - m0) * 0.5f;
	m_vFactor[1] = p1 * 2.0f - p0 * 2.5f - m1 * 0.5f + m0;
	m_vFactor[2] = (p1 - m0) * 0.5f;
	m_vFactor[3] = p0;
}

XMFLOAT3 CCatmullRom::operator()(float t) const
{
	t = MathHelper::Clamp(t, 0.0f, 1.0f);

	XMFLOAT3 vP;
	XMStoreFloat3(&vP, ((m_vFactor[0] * t + m_vFactor[1]) * t + m_vFactor[2]) * t + m_vFactor[3]);
	return vP;
}

XMFLOAT3 CCatmullRom::GetVelocity(float t) const
{
	XMFLOAT3 vP;
	XMStoreFloat3(&vP, (m_vFactor[0] * 3.0f * t + m_vFactor[1] * 2.0f) * t + m_vFactor[2]);
	return vP;
}

namespace MathHelper
{
	void ConvertQuatToEuler(XMVECTOR const &_xvQuat, XMFLOAT3 &vEulerAngles)
	{
		XMFLOAT4 q;
		XMStoreFloat4(&q, _xvQuat);

		vEulerAngles.x = atan2(2.0f * q.x * q.w - 2.0f * q.y * q.z, 1 - 2.0f * (q.x * q.x + q.z * q.z));	// Roll
		vEulerAngles.y = atan2(2.0f * q.y * q.w - 2.0f * q.x * q.z, 1 - 2.0f * (q.y * q.y + q.z * q.z));	// Yaw
		vEulerAngles.z = asin(Clamp(2.0f * q.x * q.y + 2.0f * q.z * q.w, -1.0f, 1.0f));						// Pitch
	}

	/*
	template<typename T>
	T Clamp(T Value, T Min, T Max)
	{
		return std::max(Min, std::min(Value, Max));
	}*/

	float GetRandom(float fLeftBorder, float fRightBorder)
	{
		return ((float)rand() / RAND_MAX) * (fRightBorder - fLeftBorder) + fLeftBorder;
	}

	int GetRandom(int nLeftBorder, int nRightBorder)
	{
		return rand() % (nRightBorder - nLeftBorder) + nLeftBorder;
	}
}