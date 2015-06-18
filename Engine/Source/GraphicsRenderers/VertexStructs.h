#pragma once

#include "EnginePCH.h"

typedef unsigned int CIndex;

class CVertex
{
public:
	CVertex()
	{
	}

	CVertex(const XMFLOAT3 &_vPos, const XMFLOAT2 &_vTex, const XMFLOAT3 &_vNormal) : vPosition(_vPos), vTexCoords(_vTex), vNormal(_vNormal)
	{
	}

	CVertex(const XMFLOAT3 &_vPos) : vPosition(_vPos), vTexCoords(XMFLOAT2()), vNormal(XMFLOAT3())
	{
	}

	XMFLOAT3					vPosition;
	XMFLOAT3					vNormal;
	XMFLOAT2					vTexCoords;
};