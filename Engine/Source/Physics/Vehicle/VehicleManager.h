#pragma once

#include "../PhysXLoader.h"
#include "EnginePCH.h"

#include "VehicleWheelQueryResult.h"
#include "VehicleRaycast.h"

#define MAX_4W_VEHICLES 1
#define MAX_6W_VEHICLES 0

#define MAX_WHEELS (MAX_4W_VEHICLES * 4 + MAX_6W_VEHICLES * 6)
#define MAX_VEHICLES (MAX_4W_VEHICLES + MAX_6W_VEHICLES)

class CVehicleManager : public CSingleton<CVehicleManager>
{
	friend class CSingleton<CVehicleManager>;

	~CVehicleManager();
public:
	void Init();

	// return ID of vehicle
	size_t CreateVehicle4W(PxRigidDynamic *pRigidActor, CAABB const &ChassisAABB, CAABB const WheelAABBs[4], size_t const WheelShapeIDs[4]);
	void Update(float fElapsed);

	PxVehicleDrive *GetVehicle(int i)
	{
		return (PxVehicleDrive *)m_Vehicles[i];
	};

	PxVehicleWheelQueryResult const &GetVehicleQueryResult(int i)
	{
		return m_VehicleWheelQueryResults[i];
	};
private:
	CVehicleSceneQueryData			*m_pSceneQueryData = nullptr;
	CVehicleWheelQueryResults		*m_pWheelQueryResults = nullptr;

	// friction from combinations of tire and surface types
	PxVehicleDrivableSurfaceToTireFrictionPairs 
									*m_pSurfaceTirePairs = nullptr;

	PxBatchQuery					*m_pBatchQuery = nullptr;

	PxSerializationRegistry			*m_pVehicleSerReg = nullptr;

	PxVehicleWheels					*m_Vehicles[MAX_VEHICLES];
	PxVehicleWheelQueryResult		m_VehicleWheelQueryResults[MAX_VEHICLES];
	size_t							m_nNumVehicles = 0;
};