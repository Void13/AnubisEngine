#include "EnginePCH.h"
#include "BasePhysicsBehavior.h"

#include "PhysXLoader.h"

void IPhysicalBehavior::UpdatePhysicsPosition()
{
	for (auto iObject : m_pActor->GetObjects())
	{
		if (!iObject->GetPhysicsActor())
			continue;

		//if (m_pActor->GetBodyFlag() == E_RIGID_BODY_FLAG::STATIC)
		//	CPhysicsSystem::GetInstance()->GetScene()->removeActor(*m_pPhysActor);

		iObject->GetPhysicsActor()->setGlobalPose(PxTransform(PxMat44(iObject->GetGlobalTransform())));

		//if (m_pActor->GetBodyFlag() == E_RIGID_BODY_FLAG::STATIC)
		//	CPhysicsSystem::GetInstance()->GetScene()->addActor(*m_pPhysActor);
	}
};

void IPhysicalBehavior::UpdateAABB()
{
	PxBounds3 ActorAABB;
	ActorAABB.setEmpty();

	for (auto iObject : m_pActor->GetObjects())
	{
		if (iObject->GetPhysicsActor())
		{
			PxBounds3 ObjectAABB = iObject->GetPhysicsActor()->getWorldBounds();
			ActorAABB.include(ObjectAABB);

			iObject->GetAABB().SetMinMax(ObjectAABB.minimum, ObjectAABB.maximum);
		}
	}

	m_pActor->GetAABB().SetHalfSize(ActorAABB.getExtents());
	m_pActor->GetAABB().SetPosition(ActorAABB.getCenter());
};

void IPhysicalBehavior::ApplyForce(XMFLOAT3 const &vForceDir)
{
	for (auto iObject : m_pActor->GetObjects())
	{
		if (iObject->GetPhysicsActor()->isRigidDynamic())
		{
			PxRigidDynamic *pActor = iObject->GetPhysicsActor()->is<PxRigidDynamic>();

			pActor->addForce(vForceDir, PxForceMode::eIMPULSE);
		}
	}
};