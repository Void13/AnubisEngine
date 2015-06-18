#include "EnginePCH.h"

#include "FilterShader.h"

PxFilterFlags VehicleFilterShader(
	PxFilterObjectAttributes Attr0, PxFilterData FilterData0,
	PxFilterObjectAttributes Attr1, PxFilterData FilterData1,
	PxPairFlags &PairFlags, void const *pConstantBlock, PxU32 dwConstantBlockSize)
{
	PX_UNUSED(FilterData0);
	PX_UNUSED(FilterData1);

	if (PxFilterObjectIsTrigger(Attr0) || PxFilterObjectIsTrigger(Attr1))
	{
		PairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlags();
	}



	// use a group-based mechanism for all other pairs:
	// - Objects within the default group (mask 0) always collide
	// - By default, objects of the default group do not collide
	//   with any other group. If they should collide with another
	//   group then this can only be specified through the filter
	//   data of the default group objects (objects of a different
	//   group can not choose to do so)
	// - For objects that are not in the default group, a bitmask
	//   is used to define the groups they should collide with
	if ((FilterData0.word0 != 0 || FilterData1.word0 != 0) &&
		!(FilterData0.word0 & FilterData1.word1 || FilterData1.word0 & FilterData0.word1))
		return PxFilterFlag::eSUPPRESS;

	PairFlags = PxPairFlag::eCONTACT_DEFAULT;

	//enable CCD stuff -- for now just for everything or nothing.
	if ((FilterData0.word3 | FilterData1.word3) & 1)
		PairFlags |= PxPairFlag::eCCD_LINEAR;

	// The pairFlags for each object are stored in word2 of the filter data. Combine them.
	PairFlags |= PxPairFlags(PxU16(FilterData0.word2 | FilterData1.word2));

	return PxFilterFlags();
}