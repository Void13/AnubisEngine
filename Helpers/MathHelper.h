#pragma once

#include "EnginePCH.h"

class CCircleBy3Points
{
public:
	float GetRadius() const;
	XMFLOAT3 const &GetCenter() const;
	CCircleBy3Points(XMFLOAT3 const &vA, XMFLOAT3 const &vB, XMFLOAT3 const &vC);
private:
	float m_fRadius;
	XMFLOAT3 m_Center;
};

class CCatmullRom
{
protected:
	std::vector<XMVECTOR>  m_vFactor;
public:
	CCatmullRom();
	void SetPoints(XMFLOAT3 const &_v1, XMFLOAT3 const &_v2, XMFLOAT3 const &_v3, XMFLOAT3 const &_v4);

	//¬ычисление координаты сплайна от параметра t = [0, 1]
	XMFLOAT3 operator()(float t) const;

	//¬ычисление скорости в данной позиции
	XMFLOAT3 GetVelocity(float t) const;
};

namespace MathHelper
{
	void ConvertQuatToEuler(XMVECTOR const &_xvQuat, XMFLOAT3 &vEulerAngles);

	template<typename T>
	T Clamp(T Value, T Min, T Max)
	{
		return std::max(Min, std::min(Value, Max));
	}

	template<typename T>
	T NextPowerOf2(T const &Number)
	{
		T Power = 1;
		while (Power < Number)
		{
			Power <<= 1;
		}

		return Power;
	}

	float GetRandom(float fLeftBorder, float fRightBorder);
	int GetRandom(int nLeftBorder, int nRightBorder);
};