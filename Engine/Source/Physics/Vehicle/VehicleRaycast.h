#pragma once

#include "../PhysXLoader.h"

enum E_SURFACE_TYPE
{
	DRIVABLE_SURFACE = 0xFFFF0000,
	UNDRIVABLE_SURFACE = 0x0000FFFF
};

void SetupDrivableSurface(PxFilterData &FilterData); 
void SetupNonDrivableSurface(PxFilterData &FilterData);

PxQueryHitType::Enum WheelRaycastPreFilter(
	PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	PxHitFlags& queryFlags);

//Data structure for quick setup of scene queries for suspension raycasts.
class CVehicleSceneQueryData
{
public:
	//Allocate scene query data for up to maxNumWheels suspension raycasts.
	static CVehicleSceneQueryData* allocate(const PxU32 maxNumWheels);

	//Free allocated buffer for scene queries of suspension raycasts.
	void free();

	//Create a PxBatchQuery instance that will be used as a single batched raycast of multiple suspension lines of multiple vehicles
	PxBatchQuery* setUpBatchedSceneQuery(PxScene* scene);

	//Get the buffer of scene query results that will be used by PxVehicleNWSuspensionRaycasts
	PxRaycastQueryResult* getRaycastQueryResultBuffer()
	{
		return mSqResults;
	}

	//Get the number of scene query results that have been allocated for use by PxVehicleNWSuspensionRaycasts
	PxU32 getRaycastQueryResultBufferSize() const
	{
		return mNumQueries;
	}

	//Set the pre-filter shader 
	void setPreFilterShader(PxBatchQueryPreFilterShader preFilterShader)
	{
		mPreFilterShader = preFilterShader;
	}
private:

	//One result for each wheel.
	PxRaycastQueryResult* mSqResults;
	PxU32 mNbSqResults;

	//One hit for each wheel.
	PxRaycastHit* mSqHitBuffer;

	//Filter shader used to filter drivable and non-drivable surfaces
	PxBatchQueryPreFilterShader mPreFilterShader;

	//Maximum number of suspension raycasts that can be supported by the allocated buffers 
	//assuming a single query and hit per suspension line.
	PxU32 mNumQueries;

	void init()
	{
		mPreFilterShader = WheelRaycastPreFilter;
	}

	CVehicleSceneQueryData()
	{
		init();
	}

	~CVehicleSceneQueryData()
	{
	}
};