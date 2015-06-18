#pragma once

#include "GraphicsRenderers\\VertexStructs.h"
#include "BaseActor.h"
#include "PhysXLoader.h"

class IPhysicalBehavior
{
public:
	virtual ~IPhysicalBehavior()
	{
		for (auto iObject : m_pActor->GetObjects())
			CPhysicsSystem::GetInstance()->GetScene()->removeActor(*iObject->GetPhysicsActor());
	};

	virtual void VInit(IActor *pActor) = 0;

	void UpdatePhysicsPosition();
	void UpdateAABB();

	void ApplyForce(XMFLOAT3 const &vForceDir);
protected:
	IActor			*m_pActor = nullptr;
};