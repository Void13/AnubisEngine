#include "EnginePCH.h"

#include "VehicleManager.h"
#include "Vehicle4WSimulationData.h"
#include "VehicleTireFriction.h"

void CVehicleManager::Init()
{
	auto pPhysics = CPhysicsSystem::GetInstance()->GetPhysics();

	m_pVehicleSerReg = PxSerialization::createSerializationRegistry(*pPhysics);
	PxInitVehicleSDK(*pPhysics, m_pVehicleSerReg);

	PxVec3 vUp(0.0f, 1.0f, 0.0f);
	PxVec3 vForward(0.0f, 0.0f, 1.0f);
	PxVehicleSetBasisVectors(vUp, vForward);

	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	m_pSceneQueryData = CVehicleSceneQueryData::allocate(MAX_WHEELS);
	m_pWheelQueryResults = CVehicleWheelQueryResults::allocate(MAX_WHEELS);

	PxMaterial const *SuftTypes[MAX_NUM_SURFACE_TYPES] =
	{
		CPhysicsSystem::GetInstance()->GetMUDMaterial(),
		CPhysicsSystem::GetInstance()->GetTarmacMaterial(),
		CPhysicsSystem::GetInstance()->GetIceMaterial(),
		CPhysicsSystem::GetInstance()->GetGrassMaterial()
	};

	m_pSurfaceTirePairs = CreateFrictionPairs(SuftTypes);
}

size_t CVehicleManager::CreateVehicle4W(PxRigidDynamic *pRigidActor, CAABB const &ChassisAABB, CAABB const WheelAABBs[4], size_t const WheelShapeIDs[4])
{
	auto pPhysics = CPhysicsSystem::GetInstance()->GetPhysics();

	XMFLOAT3 WheelCenterOffsets[4];

	for (int i = 0; i < 4; i++)
	{
		WheelCenterOffsets[i] = WheelAABBs[i].GetPosition();
		//WheelCenterOffsets[i].y -= 0.2f;
		//WheelCenterOffsets[i] = WheelCenterOffsets[1];
	}
	/*
	WheelCenterOffsets[1].x *= -1;
	WheelCenterOffsets[2].z *= -1;
	WheelCenterOffsets[3].x *= -1;
	WheelCenterOffsets[3].z *= -1;
	*/
	PxFilterData VehicleQueryFilterData;
	VehicleQueryFilterData.word3 = (PxU32)E_SURFACE_TYPE::UNDRIVABLE_SURFACE;

	PxVehicleWheelsSimData *pWheelsSimData = PxVehicleWheelsSimData::allocate(4);
	PxVehicleDriveSimData4W DriveSimData;
	PxVehicleChassisData ChassisData;

	float WheelMasses[4] = { 20.0f, 20.0f, 20.0f, 20.0f };
	CreateVehicle4WSimulationData(
		1500.0f, ChassisAABB, WheelMasses, WheelAABBs, WheelCenterOffsets,
		*pWheelsSimData, ChassisData);

	SetupDriveSimulationData(WheelCenterOffsets, DriveSimData);

	pRigidActor->setMass(ChassisData.mMass);
	pRigidActor->setMassSpaceInertiaTensor(ChassisData.mMOI);
	pRigidActor->setCMassLocalPose(PxTransform(ChassisData.mCMOffset, PxQuat(PxIdentity)));

	pWheelsSimData->setWheelShapeMapping(PxVehicleDrive4WWheelOrder::eFRONT_LEFT,	WheelShapeIDs[0]);
	pWheelsSimData->setWheelShapeMapping(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT,	WheelShapeIDs[1]);
	pWheelsSimData->setWheelShapeMapping(PxVehicleDrive4WWheelOrder::eREAR_LEFT,	WheelShapeIDs[2]);
	pWheelsSimData->setWheelShapeMapping(PxVehicleDrive4WWheelOrder::eREAR_RIGHT,	WheelShapeIDs[3]);

	pWheelsSimData->setSceneQueryFilterData(0, VehicleQueryFilterData);
	pWheelsSimData->setSceneQueryFilterData(1, VehicleQueryFilterData);
	pWheelsSimData->setSceneQueryFilterData(2, VehicleQueryFilterData);
	pWheelsSimData->setSceneQueryFilterData(3, VehicleQueryFilterData);

	PxVehicleDrive4W *pPxVehicle = PxVehicleDrive4W::allocate(4);
	pPxVehicle->setup(pPhysics, pRigidActor, *pWheelsSimData, DriveSimData, 0);
	pWheelsSimData->free();

	pPxVehicle->setToRestState();

	m_Vehicles[m_nNumVehicles] = pPxVehicle;
	m_VehicleWheelQueryResults[m_nNumVehicles].nbWheelQueryResults = 4;
	m_VehicleWheelQueryResults[m_nNumVehicles].wheelQueryResults = m_pWheelQueryResults->addVehicle(4);

	m_nNumVehicles++;

	return m_nNumVehicles - 1;
}

void CVehicleManager::Update(float fElapsed)
{
	auto pScene = CPhysicsSystem::GetInstance()->GetScene();

	if (!m_pBatchQuery)
	{
		m_pBatchQuery = m_pSceneQueryData->setUpBatchedSceneQuery(pScene);
	}

	PxVehicleSuspensionRaycasts(m_pBatchQuery, m_nNumVehicles, m_Vehicles,
		m_pSceneQueryData->getRaycastQueryResultBufferSize(), m_pSceneQueryData->getRaycastQueryResultBuffer());

	PxVehicleUpdates(fElapsed, CPhysicsSystem::GetInstance()->GetGravity(), *m_pSurfaceTirePairs, 
		m_nNumVehicles, m_Vehicles, m_VehicleWheelQueryResults);
}

CVehicleManager::~CVehicleManager()
{
	for (PxU32 i = 0; i < m_nNumVehicles; i++)
	{
		switch (m_Vehicles[i]->getVehicleType())
		{
		case PxVehicleTypes::eDRIVE4W:
		{
			PxVehicleDrive4W* veh = (PxVehicleDrive4W *)m_Vehicles[i];
			veh->free();
		}
			break;
		case PxVehicleTypes::eDRIVENW:
		{
			PxVehicleDriveNW* veh = (PxVehicleDriveNW *)m_Vehicles[i];
			veh->free();
		}
			break;
		case PxVehicleTypes::eDRIVETANK:
		{
			PxVehicleDriveTank* veh = (PxVehicleDriveTank *)m_Vehicles[i];
			veh->free();
		}
			break;
		default:
			PX_ASSERT(false);
			break;
		}
	}

	m_pSceneQueryData->free();
	m_pWheelQueryResults->free();
	m_pSurfaceTirePairs->release();

	PxCloseVehicleSDK(m_pVehicleSerReg);
	if (m_pVehicleSerReg)
		m_pVehicleSerReg->release();
}