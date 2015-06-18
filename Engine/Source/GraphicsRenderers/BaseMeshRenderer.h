#pragma once

#include "EnginePCH.h"
#include "VertexStructs.h"
#include "BaseModelRenderer.h"
#include "Material.h"
#include "RawMesh.h"

class ICamera;

namespace physx
{
	class PxShape;
};

class IBaseMeshRenderer
{
public:
	IBaseMeshRenderer(IBaseModelRenderer *pModelRenderer) :
		m_pModelRenderer(pModelRenderer)
	{
	};

	virtual ~IBaseMeshRenderer()
	{
	};

	physx::PxShape *GetPhysXShape() const
	{
		return m_PhysXShape;
	};

	std::string const &GetName() const
	{
		return m_sMeshName;
	};

	XMMATRIX const &GetLocalWorld() const
	{
		return m_mLocalWorld;
	};

	CAABB const &GetAABB() const
	{
		return m_InitAABB;
	};

	// add mesh
	virtual void VCreateMesh(CRawMesh const &RawMesh, CRawMesh const &PhysicsRawMesh) = 0;

	virtual void VRender(ICamera const *const _pCamera, DWORD dwNumInstances) const = 0;
protected:
	XMMATRIX			m_mLocalWorld = XMMatrixIdentity();

	CAABB				m_InitAABB;

	std::string			m_sMeshName;
	DWORD				m_dwVertexSize;

	IBaseModelRenderer	*m_pModelRenderer = nullptr;

	physx::PxShape		*m_PhysXShape;
};