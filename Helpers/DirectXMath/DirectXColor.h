#pragma once

#include <cmath>
#include <float.h>
#include <malloc.h>
#include <DirectXMath.h>

using namespace DirectX;

struct XMCOLOR
{
	union
	{
		struct
		{
			uint32_t a : 8;  // Alpha:   0/255 to 255/255
			uint32_t b : 8;  // Blue:    0/255 to 255/255
			uint32_t g : 8;  // Green:   0/255 to 255/255
			uint32_t r : 8;  // Red:     0/255 to 255/255
		};
		uint32_t c;
	};

	XMCOLOR()
	{
	};
	XMCOLOR(uint32_t Color) : c(Color)
	{
	};

	// from 0 to 255
	XMCOLOR(float _r, float _g, float _b, float _a);
	XMCOLOR(const float *pArray);

	operator uint32_t () const
	{
		return c;
	};
	
	operator XMFLOAT3 () const
	{
		return XMFLOAT3(r * 0.00392f, g * 0.00392f, b * 0.00392f);
	};

	operator XMFLOAT4 () const
	{
		return XMFLOAT4(r * 0.00392f, g * 0.00392f, b * 0.00392f, a * 0.00392f);
	};
	
	XMCOLOR& operator= (const XMCOLOR& Color)
	{
		c = Color.c;
		return *this;
	};
	XMCOLOR& operator= (const uint32_t Color)
	{
		c = Color;
		return *this;
	};
};

void XMStoreColor(XMCOLOR* pDestination, FXMVECTOR V);
XMVECTOR XMLoadColor(const XMCOLOR* pSource);