#pragma once

#include "EnginePCH.h"
#include "../PhysXLoader.h"

void CreateVehicle4WSimulationData(
	float fChassisMass, CAABB const &ChassisAABB,
	float WheelMasses[4], CAABB const WheelAABBs[4],
	XMFLOAT3 const WheelCenterOffsets[4],
	PxVehicleWheelsSimData &WheelsData,
	PxVehicleChassisData &ChassisData);

void SetupDriveSimulationData(
	XMFLOAT3 const WheelCenterOffsets[4],
	PxVehicleDriveSimData4W &DriveData);