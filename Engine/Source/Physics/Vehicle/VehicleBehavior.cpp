#include "EnginePCH.h"

#include "VehicleBehavior.h"

#include "Vehicle.h"
#include "../FilterShader.h"
#include "VehicleManager.h"

// (Radians per second) to (Revolutions per minute)
float ConvertRPSToRPM(float fRPS)
{
	// rpm = 30 * rps / pi;
	return fRPS * 30.0f / XM_PI;
}

// (Revolutions per minute) to (radians per second)
float ConvertRPMToRPS(float fRPM)
{
	// 1 rpm = pi / 30 rad/s
	// rps = rpm * pi / 30;
	return fRPM * XM_PI / 30.0f;
}

void CVehiclePhysics::VInit(IActor *pActor)
{
	CVehicleActor *pVehicle = (CVehicleActor *)pActor;

	IBaseModelRenderer *pModel = pActor->GetModelRenderer();
	if (!pModel)
	{
		return;
	}

	m_pActor = pActor;
	auto pPhysics = CPhysicsSystem::GetInstance()->GetPhysics();

	PxRigidDynamic *pObjectActor = pPhysics->createRigidDynamic(PxTransform(m_pActor->GetGlobalTransform()));
	//pObjectActor->setSolverIterationCounts(20, 15);
	//pObjectActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

	pObjectActor->userData = m_pActor->GetObjectByName(pModel->GetCenterMeshName());
	pObjectActor->setGlobalPose(PxTransform(pActor->GetGlobalTransform()));

	for (auto pObject : pActor->GetObjects())
	{
		pObject->SetPhysicsActor(pObjectActor);

		auto pMesh = pModel->GetMeshByMeshName(pObject->GetName());
		auto pShape = pMesh->GetPhysXShape();
		if (!pShape)
		{
			CHError er = new CError_PhysXError(AT);
			return;
		}

		switch (pShape->getGeometryType())
		{
			case physx::PxGeometryType::eCONVEXMESH:
			{
				physx::PxConvexMeshGeometry geom;
				pShape->getConvexMeshGeometry(geom);

				PxMaterial *pMat = CPhysicsSystem::GetInstance()->GetTarmacMaterial();

				auto pCopiedShape = pObjectActor->createShape(geom, *pMat);
				if (!pCopiedShape)
				{
					CHError er = new CError_PhysXError(AT);
					return;
				}

				if (StringHelper::FindSubstrCI(pObject->GetName(), std::string("wheel")) != -1)
				{
					pMat = CPhysicsSystem::GetInstance()->GetRubberMaterial();

					auto AABB = PxShapeExt::getWorldBounds(*pCopiedShape, *pObjectActor);
					pObjectActor->detachShape(*pCopiedShape);

					float fRadius = std::max(AABB.getExtents().y, AABB.getExtents().z) * 0.975f;
					float fWidth = AABB.getExtents().x * 2.0f;

					PxVec3 verts[2 * 16];
					PxU32 numVerts = 2 * 8;

					const PxF32 dtheta = 2 * PxPi / (1.0f * 8);
					for (PxU32 i = 0; i < 16; i++)
					{
						const PxF32 theta = dtheta * i;
						const PxF32 cosTheta = fRadius * PxCos(theta);
						const PxF32 sinTheta = fRadius * PxSin(theta);
						verts[2 * i + 0] = PxVec3(-0.5f * fWidth, cosTheta, sinTheta);
						verts[2 * i + 1] = PxVec3(+0.5f * fWidth, cosTheta, sinTheta);
					}

					PxConvexMeshDesc ConvexMeshDesc;
					ConvexMeshDesc.points.count = 2 * 16;
					ConvexMeshDesc.points.stride = sizeof(PxVec3);
					ConvexMeshDesc.points.data = verts;
					ConvexMeshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

					auto pPhysics = CPhysicsSystem::GetInstance()->GetPhysics();
					PxCooking *pCooking = CPhysicsSystem::GetInstance()->GetCooking();

					PxDefaultMemoryOutputStream OutputStream;
					bool bResult = pCooking->cookConvexMesh(ConvexMeshDesc, OutputStream);
					if (!bResult)
						return;

					PxDefaultMemoryInputData InputStream(OutputStream.getData(), OutputStream.getSize());
					PxConvexMesh *pMesh = pPhysics->createConvexMesh(InputStream);
					PxGeometry &Geometry = PxConvexMeshGeometry(pMesh);

					pCopiedShape = pObjectActor->createShape(Geometry, *pMat);
				}

				pCopiedShape->userData = pObject;
				pCopiedShape->setLocalPose(PxTransform(pObject->GetLocalTransform()));

				pObject->SetPhysicsShape(pCopiedShape);

				break;
			}
			default:
			{
				CHError er = new CError_PhysXError(AT);
				return;
			}
		};
	}
	
	PxFilterData WheelCollidingFilterData;
	WheelCollidingFilterData.word0 = E_COLLISION_FLAG::COLLISION_FLAG_WHEEL;
	WheelCollidingFilterData.word1 = E_COLLISION_FLAG::COLLISION_FLAG_WHEEL_AGAINST;

	PxFilterData ChassisCollidingFilterData;
	ChassisCollidingFilterData.word0 = E_COLLISION_FLAG::COLLISION_FLAG_CHASSIS;
	ChassisCollidingFilterData.word1 = E_COLLISION_FLAG::COLLISION_FLAG_CHASSIS_AGAINST;
	
	PxBounds3 PxChassisAABB;
	PxChassisAABB.setEmpty();
	PxBounds3 PxWheelAABBs[4];
	
	size_t WheelShapeIDs[4] = { -1, -1, -1, -1 };
	
	PxFilterData VehicleQueryFilterData;
	VehicleQueryFilterData.word3 = (PxU32)E_SURFACE_TYPE::UNDRIVABLE_SURFACE;


	int nOverallShapes = 0;
	for (auto pObject : pActor->GetObjects())
	{
		auto pShape = pObject->GetPhysicsShape();

		// get wheel0
		if (StringHelper::FindSubstrCI(pObject->GetName(), std::string("wheel")) != -1)
		{
			std::string sWheelNumber = pObject->GetName().substr(5);
			int nWheelID = StringHelper::StringToNumber<int>(sWheelNumber);
			if (nWheelID > 3 && nWheelID < 0)
			{
				CHError er = new CError_PhysXError(AT);
				return;
			}

			// it's wheel
			WheelShapeIDs[nWheelID] = nOverallShapes;
			PxWheelAABBs[nWheelID] = PxShapeExt::getWorldBounds(*pShape, *pObjectActor);

			pShape->setSimulationFilterData(WheelCollidingFilterData);

			pShape->setLocalPose(PxTransform(PxIdentity));
		}
		else
		{
			// it's chassis
			PxBounds3 ChassisPartWorldBounds = PxShapeExt::getWorldBounds(*pShape, *pObjectActor);
			PxChassisAABB.include(ChassisPartWorldBounds);
			
			pShape->setSimulationFilterData(ChassisCollidingFilterData);

			//pShape->setLocalPose(PxTransform(PxIdentity));
			pShape->setLocalPose(PxTransform(pActor->GetGlobalTransform()) * pShape->getLocalPose());
		}

		pShape->setQueryFilterData(VehicleQueryFilterData);

		nOverallShapes++;
	}

	pObjectActor->setGlobalPose(PxTransform(PxIdentity));
	//m_pActor->SetTransform(XMMatrixIdentity());
	m_pActor->SetPosition(g_XMZero);
	m_pActor->SetRotation(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	CAABB ChassisAABB(PxChassisAABB.minimum, PxChassisAABB.maximum);
	CAABB WheelAABBs[4];
	
	for (int i = 0; i < 4; i++)
	{
		WheelAABBs[i].SetMinMax(PxWheelAABBs[i].minimum, PxWheelAABBs[i].maximum);
	}
	/*
	WheelAABBs[0].SetPosition(XMFLOAT3(ChassisAABB.GetMin().x, ChassisAABB.GetMin().y, ChassisAABB.GetMax().z));
	WheelAABBs[1].SetPosition(XMFLOAT3(ChassisAABB.GetMax().x, ChassisAABB.GetMin().y, ChassisAABB.GetMax().z));
	WheelAABBs[2].SetPosition(ChassisAABB.GetMin());
	WheelAABBs[3].SetPosition(XMFLOAT3(ChassisAABB.GetMax().x, ChassisAABB.GetMin().y, ChassisAABB.GetMin().z));

	WheelAABBs[0].SetHalfSize(XMFLOAT3(0.3f, 0.7f, 0.7f));
	WheelAABBs[1].SetHalfSize(XMFLOAT3(0.3f, 0.7f, 0.7f));
	WheelAABBs[2].SetHalfSize(XMFLOAT3(0.3f, 0.7f, 0.7f));
	WheelAABBs[3].SetHalfSize(XMFLOAT3(0.3f, 0.7f, 0.7f));
	*/
	CPhysicsSystem::GetInstance()->GetScene()->addActor(*pObjectActor);

	m_nVehicleID = CVehicleManager::GetInstance()->CreateVehicle4W(pObjectActor, ChassisAABB, WheelAABBs, WheelShapeIDs);
	m_pVehicle = CVehicleManager::GetInstance()->GetVehicle(m_nVehicleID);
};