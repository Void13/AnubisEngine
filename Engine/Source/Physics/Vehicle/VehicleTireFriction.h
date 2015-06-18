#pragma once

#include "EnginePCH.h"
#include "../PhysXLoader.h"

enum E_DRIVABLE_SURFACE_TYPES
{
	SURFACE_TYPE_MUD = 0,
	SURFACE_TYPE_TARMAC,
	SURFACE_TYPE_SNOW,
	SURFACE_TYPE_GRASS,
	MAX_NUM_SURFACE_TYPES
};

//Tire types.
enum E_TIRE_TYPES
{
	TIRE_TYPE_WETS = 0,
	TIRE_TYPE_SLICKS,
	TIRE_TYPE_ICE,
	TIRE_TYPE_MUD,
	TIRE_TYPE_WORN,	// изношенные
	MAX_NUM_TIRE_TYPES
};

PxVehicleDrivableSurfaceToTireFrictionPairs *CreateFrictionPairs(PxMaterial const *SurfaceMaterials[MAX_NUM_SURFACE_TYPES]);