#include "EnginePCH.h"
#include "VehicleRaycast.h"

#undef new

#include <PhysX\foundation\include\PsAllocator.h>

#define SIZEALIGN16(size) (((unsigned)(size) + 15) & ((unsigned)(~15)));

PxQueryHitType::Enum WheelRaycastPreFilter(
	PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	PxHitFlags& queryFlags)
{
	//filterData0 is the vehicle suspension raycast.
	//filterData1 is the shape potentially hit by the raycast.
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);
	PX_UNUSED(filterData0);
	PX_UNUSED(queryFlags);
	return ((0 == (filterData1.word3 & E_SURFACE_TYPE::DRIVABLE_SURFACE)) ?
		PxQueryHitType::eNONE : PxQueryHitType::eBLOCK);
}

CVehicleSceneQueryData* CVehicleSceneQueryData::allocate(const PxU32 maxNumWheels)
{
	const PxU32 size0 = SIZEALIGN16(sizeof(CVehicleSceneQueryData));
	const PxU32 size1 = SIZEALIGN16(sizeof(PxRaycastQueryResult)* maxNumWheels);
	const PxU32 size2 = SIZEALIGN16(sizeof(PxRaycastHit)* maxNumWheels);
	const PxU32 size = size0 + size1 + size2;

	CVehicleSceneQueryData* sqData = (CVehicleSceneQueryData*)PX_ALLOC(size, PX_DEBUG_EXP("PxVehicleNWSceneQueryData"));
	sqData->init();

	PxU8* ptr = (PxU8*)sqData;
	ptr += size0;
	sqData->mSqResults = (PxRaycastQueryResult*)ptr;
	sqData->mNbSqResults = maxNumWheels;
	ptr += size1;
	sqData->mSqHitBuffer = (PxRaycastHit*)ptr;
	ptr += size2;
	sqData->mNumQueries = maxNumWheels;

	return sqData;
}

void CVehicleSceneQueryData::free()
{
	PX_FREE(this);
}

PxBatchQuery* CVehicleSceneQueryData::setUpBatchedSceneQuery(PxScene* scene)
{
	PxBatchQueryDesc sqDesc(mNbSqResults, 0, 0);

	sqDesc.queryMemory.userRaycastResultBuffer = mSqResults;
	sqDesc.queryMemory.userRaycastTouchBuffer = mSqHitBuffer;
	sqDesc.queryMemory.raycastTouchBufferSize = mNumQueries;
	sqDesc.preFilterShader = mPreFilterShader;

	return scene->createBatchQuery(sqDesc);
}