#include "EnginePCH.h"
#include "PrimitivesBehavior.h"
#include "PrimitiveObjects.h"

void CPrimitivePhysics::VInit(IActor *pActor)
{
	IBaseModelRenderer *pModel = pActor->GetModelRenderer();
	if (!pModel)
	{
		return;
	}

	m_pActor = pActor;
	auto pPhysics = CPhysicsSystem::GetInstance()->GetPhysics();

	auto _CreatePhysXActor = [&](CObject *iObject)
	{
		auto Transform = PxTransform(iObject->GetGlobalTransform());

		PxRigidActor *pObjectActor;

		switch (pModel->GetRigidBodyFlag())
		{
		case E_RIGID_BODY_FLAG::DYNAMIC:
		{
			auto pRigidBody = pPhysics->createRigidDynamic(Transform);

			//pRigidBody->setAngularDamping(0.5f);
			//pRigidBody->setLinearDamping(0.5f);
			//pRigidBody->setMaxAngularVelocity(100.0f);
			//pRigidBody->setSolverIterationCounts(20, 15);
			//pRigidBody->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

			pObjectActor = pRigidBody;
		}
			break;
		case E_RIGID_BODY_FLAG::KINEMATIC:
		{
			auto pRigidBody = pPhysics->createRigidDynamic(Transform);
			pRigidBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

			pObjectActor = pRigidBody;
		}
			break;
		case E_RIGID_BODY_FLAG::STATIC:
		{
			pObjectActor = pPhysics->createRigidStatic(Transform);
		}
			break;
		}

		pObjectActor->userData = iObject;
		iObject->SetPhysicsActor(pObjectActor);

		return pObjectActor;
	};

	// тут 100% есть линкованный объект, и у него есть мешка какая-то(и физ. шейп)
	for (auto iObject : pActor->GetObjects())
	{
		IBaseMeshRenderer *pMesh = pModel->GetMeshByMeshName(iObject->GetName());
		if (!pMesh)
		{
			continue;
		}

		if (pMesh->GetPhysXShape())
		{
			auto pObjectActor = _CreatePhysXActor(iObject);
			iObject->SetPhysicsShape(pMesh->GetPhysXShape());

			pObjectActor->attachShape(*iObject->GetPhysicsShape());

			if (pModel->GetRigidBodyFlag() != E_RIGID_BODY_FLAG::STATIC)
			{
				PxRigidBodyExt::updateMassAndInertia(*pObjectActor->isRigidBody(), 1.0f);
			}

			CPhysicsSystem::GetInstance()->GetScene()->addActor(*pObjectActor);
		}
	}
}