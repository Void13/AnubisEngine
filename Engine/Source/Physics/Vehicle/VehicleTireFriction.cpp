#include "EnginePCH.h"

#include "VehicleTireFriction.h"

//Tire model friction for each combination of drivable surface type and tire type.
static PxF32 g_TireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
{
	//WETS	SLICKS	ICE		MUD
	{ 0.95f, 0.95f, 0.95f, 0.95f },		//MUD
	{ 1.10f, 2.15f, 1.10f, 1.10f },		//TARMAC
	{ 0.70f, 0.70f, 0.70f, 0.70f },		//ICE
	{ 0.80f, 0.80f, 0.80f, 0.80f }		//GRASS
};

PxVehicleDrivableSurfaceToTireFrictionPairs *CreateFrictionPairs(PxMaterial const *SurfaceMaterials[MAX_NUM_SURFACE_TYPES])
{
	PxVehicleDrivableSurfaceType SurfaceTypes[MAX_NUM_SURFACE_TYPES];
	PxMaterial const *pSurfaceMaterials[MAX_NUM_SURFACE_TYPES];

	for (int i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
	{
		pSurfaceMaterials[i] = SurfaceMaterials[i];
		SurfaceTypes[i].mType = SURFACE_TYPE_MUD + i;
	}

	PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
		PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES);

	surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, pSurfaceMaterials, SurfaceTypes);

	for (PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
	{
		for (PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++)
		{
			surfaceTirePairs->setTypePairFriction(i, j, g_TireFrictionMultipliers[i][j]);
		}
	}

	return surfaceTirePairs;
}