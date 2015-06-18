#include "EnginePCH.h"
#include "VehicleWheelQueryResult.h"

#undef new

#include <PhysX\foundation\include\PsAllocator.h>

CVehicleWheelQueryResults* CVehicleWheelQueryResults::allocate(const PxU32 maxNumWheels)
{
	const PxU32 size = sizeof(CVehicleWheelQueryResults) + sizeof(PxWheelQueryResult) * maxNumWheels;

	CVehicleWheelQueryResults* resData =
		(CVehicleWheelQueryResults*)PX_ALLOC(size, PX_DEBUG_EXP("SampleVehicleWheelQueryResults"));

	resData->init();

	PxU8* ptr = (PxU8 *)resData;
	ptr += sizeof(CVehicleWheelQueryResults);
	resData->mWheelQueryResults = (PxWheelQueryResult *)ptr;
	ptr += sizeof(PxWheelQueryResult) * maxNumWheels;
	resData->mMaxNumWheels = maxNumWheels;

	for (PxU32 i = 0; i < maxNumWheels; i++)
	{
		new(&resData->mWheelQueryResults[i]) PxWheelQueryResult();
	}

	return resData;
}

void CVehicleWheelQueryResults::free()
{
	PX_FREE(this);
}

PxWheelQueryResult* CVehicleWheelQueryResults::addVehicle(const PxU32 numWheels)
{
	PX_ASSERT((mNumWheels + numWheels) <= mMaxNumWheels);

	PxWheelQueryResult* r = &mWheelQueryResults[mNumWheels];
	mNumWheels += numWheels;

	return r;
}