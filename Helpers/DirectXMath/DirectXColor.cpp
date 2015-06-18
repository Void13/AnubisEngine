#pragma once

#include "EnginePCH.h"

#include "DirectXColor.h"

inline void XMStoreColor
(
	XMCOLOR* pDestination,
	FXMVECTOR V
)
{
#if defined(_XM_NO_INTRINSICS_)

	XMVECTOR               N;
	//static const XMVECTORF32  Scale = {255.0f, 255.0f, 255.0f, 255.0f};

	assert(pDestination);

	//N = XMVectorSaturate(V);
	//N = XMVectorMultiply(N, Scale.v);
	N = XMVectorRound(V);

	pDestination->c = ((UINT)N.vector4_f32[3] << 24) |
		((UINT)N.vector4_f32[2] << 16) |
		((UINT)N.vector4_f32[1] << 8) |
		((UINT)N.vector4_f32[0]);

#elif defined(_XM_SSE_INTRINSICS_)
	assert(pDestination);
	static const XMVECTORF32  Scale = { 255.0f, 255.0f, 255.0f, 255.0f };
	// Set <0 to 0
	XMVECTOR vResult = _mm_max_ps(V, g_XMZero);
	// Set>1 to 1
	vResult = _mm_min_ps(vResult, g_XMOne);
	// Convert to 0-255
	vResult = _mm_mul_ps(vResult, Scale);
	// Shuffle RGBA to ARGB
	vResult = _mm_shuffle_ps(vResult, vResult, _MM_SHUFFLE(3, 0, 1, 2));
	// Convert to int 
	__m128i vInt = _mm_cvtps_epi32(vResult);
	// Mash to shorts
	vInt = _mm_packs_epi32(vInt, vInt);
	// Mash to bytes
	vInt = _mm_packus_epi16(vInt, vInt);
	// Store the color
	_mm_store_ss(reinterpret_cast<float *>(&pDestination->c), reinterpret_cast<__m128 *>(&vInt)[0]);
#else // _XM_VMX128_INTRINSICS_
#endif // _XM_VMX128_INTRINSICS_
}

inline XMVECTOR XMLoadColor
(
	const XMCOLOR* pSource
)
{
	assert(pSource);
#if defined(_XM_NO_INTRINSICS_) || defined(_XM_ARM_NEON_INTRINSICS_)
	// int32_t -> Float conversions are done in one instruction.
	// uint32_t -> Float calls a runtime function. Keep in int32_t
	int32_t iColor = (int32_t)(pSource->c);
	XMVECTORF32 vColor = {
		(float)((iColor >> 16) & 0xFF) * (1.0f / 255.0f),
		(float)((iColor >> 8) & 0xFF) * (1.0f / 255.0f),
		(float)(iColor & 0xFF) * (1.0f / 255.0f),
		(float)((iColor >> 24) & 0xFF) * (1.0f / 255.0f)
	};
	return vColor.v;
#elif defined(_XM_SSE_INTRINSICS_)
	// Splat the color in all four entries
	__m128i vInt = _mm_set1_epi32(pSource->c);
	// Shift R&0xFF0000, G&0xFF00, B&0xFF, A&0xFF000000
	vInt = _mm_and_si128(vInt, g_XMMaskA8R8G8B8);
	// a is unsigned! Flip the bit to convert the order to signed
	vInt = _mm_xor_si128(vInt, g_XMFlipA8R8G8B8);
	// Convert to floating point numbers
	XMVECTOR vTemp = _mm_cvtepi32_ps(vInt);
	// RGB + 0, A + 0x80000000.f to undo the signed order.
	vTemp = _mm_add_ps(vTemp, g_XMFixAA8R8G8B8);
	// Convert 0-255 to 0.0f-1.0f
	return _mm_mul_ps(vTemp, g_XMNormalizeA8R8G8B8);
#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)
#endif // _XM_VMX128_INTRINSICS_
}

XMCOLOR::XMCOLOR(float _r, float _g, float _b, float _a)
{
	XMStoreColor(this, XMVectorSet(_r, _g, _b, _a));
};

XMCOLOR::XMCOLOR(const float *pArray)
{
	XMStoreColor(this, XMLoadFloat4((XMFLOAT4 *)pArray));
};