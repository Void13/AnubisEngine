#include "EnginePCH.h"

#include "Vehicle4WSimulationData.h"
#include "VehicleTireFriction.h"

#include "VehicleBehavior.h"

void CreateVehicle4WSimulationData(
	float fChassisMass, CAABB const &ChassisAABB,
	float WheelMasses[4], CAABB const WheelAABBs[4],
	XMFLOAT3 const WheelCenterOffsets[4],
	PxVehicleWheelsSimData &WheelsData,
	PxVehicleChassisData &ChassisData)
{
	XMFLOAT3 vChassisDims = ChassisAABB.GetSize();

	// The origin is at the center of the chassis mesh.
	// Set the center of mass to be below this point and a little towards the front.
	XMFLOAT3 vChassisCMOffset = XMFLOAT3(0.0f, -vChassisDims.y * 0.5f + 0.65f, 0.25f);

	// Ix = m / 12 * (y * y + z * z);
	// Iy = m / 12 * (x * x + z * z);
	// Iz = m / 12 * (y * y + x * x);

	XMFLOAT3 vChassisMOI(
		(vChassisDims.y * vChassisDims.y + vChassisDims.z * vChassisDims.z) * fChassisMass / 12.0f,
		(vChassisDims.x * vChassisDims.x + vChassisDims.z * vChassisDims.z) * fChassisMass / 12.0f,
		(vChassisDims.x * vChassisDims.x + vChassisDims.y * vChassisDims.y) * fChassisMass / 12.0f);

	// more responsive turning
	vChassisMOI.y *= 0.8f;

	ChassisData.mMass = fChassisMass;
	ChassisData.mMOI = vChassisMOI;
	ChassisData.mCMOffset = vChassisCMOffset;

	PxVec3 PxWheelCenterOffsets[4];
	for (int i = 0; i < 4; i++)
	{
		PxWheelCenterOffsets[i] = WheelCenterOffsets[i];
	}

	float SuspensionSprungMasses[4];
	PxVehicleComputeSprungMasses(4, PxWheelCenterOffsets, vChassisCMOffset, fChassisMass, 1, SuspensionSprungMasses);

	float WheelMOIs[4];
	float WheelWidths[4];
	float WheelRadiuses[4];

	for (int i = 0; i < 4; i++)
	{
		WheelWidths[i] = WheelAABBs[i].GetSize().x;
		WheelRadiuses[i] = std::max(WheelAABBs[i].GetSize().y, WheelAABBs[i].GetSize().z) * 0.60f;

		// TODO!! check another formula of MOI
		// formula MOI: m * r * r / 2
		WheelMOIs[i] = 0.5f * WheelMasses[i] * WheelRadiuses[i] * WheelRadiuses[i];
	}

	PxVehicleWheelData Wheels[4];
	for (int i = 0; i < 4; i++)
	{
		Wheels[i].mRadius = WheelRadiuses[i];
		Wheels[i].mMass = WheelMasses[i];
		Wheels[i].mMOI = WheelMOIs[i];
		Wheels[i].mWidth = WheelWidths[i];

		// TODO
		Wheels[i].mDampingRate = 2.0f;
	}

	Wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mToeAngle = XMConvertToRadians(0.2f);
	Wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mToeAngle = XMConvertToRadians(0.2f);
	Wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mToeAngle = 0.0f;
	Wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mToeAngle = 0.0f;

	// disable handbrake to front wheels and enable for rear
	Wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxBrakeTorque = 13000.0f;
	Wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxBrakeTorque = 13000.0f;
	Wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxBrakeTorque = 13000.0f;
	Wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxBrakeTorque = 13000.0f;

	Wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxHandBrakeTorque = 0.0f;
	Wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxHandBrakeTorque = 0.0f;
	Wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
	Wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;

	// enable steering for front wheels and disable for rear
	Wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = XM_PI * 0.33f;
	Wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = XM_PI * 0.33f;
	Wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxSteer = 0.0f;
	Wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxSteer = 0.0f;

	PxVehicleTireData Tires[4];
	Tires[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mType = TIRE_TYPE_SLICKS;
	Tires[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mType = TIRE_TYPE_SLICKS;
	Tires[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mType = TIRE_TYPE_SLICKS;
	Tires[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mType = TIRE_TYPE_SLICKS;

	PxVehicleSuspensionData Susps[4];

	// V-shape if positive value
	// A-shape if negative value
	float fCamberAtRest = -0.0f;
	float fCamberAtMaxDroop = 0.0f;
	float fCamberAtMaxCompression = -0.0f;

	for (int i = 0; i < 4; i++)
	{
		Susps[i].mMaxCompression = 0.3f;
		Susps[i].mMaxDroop = 0.3f;
		Susps[i].mSpringStrength = 30500.0f;
		Susps[i].mSpringDamperRate = 5000.0f;
		Susps[i].mSprungMass = SuspensionSprungMasses[i];

		// 0, 2 - positive
		float fNegative = (i % 2 == 0) ? 1.0f : -1.0f;
		Susps[i].mCamberAtRest = fCamberAtRest * fNegative;
		Susps[i].mCamberAtMaxDroop = fCamberAtMaxDroop * fNegative;
		Susps[i].mCamberAtMaxCompression = fCamberAtMaxCompression * fNegative;
	}

	// TODO: Caster angle
	// absulutely vertical.
	XMFLOAT3 SuspTravelDirs[4] =
	{
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
	};

	XMFLOAT3 WheelCenterCMOffsets[4];
	XMFLOAT3 SuspForceAppCMOffsets[4];
	XMFLOAT3 TireForceAppCMOffsets[4];

	// assume that we apply the tire and suspension forces 30cm below the center of mass
	for (int i = 0; i < 4; i++)
	{
		WheelCenterCMOffsets[i] = (PxVec3)WheelCenterOffsets[i] - vChassisCMOffset;
		SuspForceAppCMOffsets[i] = XMFLOAT3(WheelCenterOffsets[i].x, -0.3f, WheelCenterOffsets[i].z);
		TireForceAppCMOffsets[i] = XMFLOAT3(WheelCenterOffsets[i].x, -0.3f, WheelCenterOffsets[i].z);
	}

	for (int i = 0; i < 4; i++)
	{
		WheelsData.setWheelData(i, Wheels[i]);
		WheelsData.setTireData(i, Tires[i]);
		WheelsData.setSuspensionData(i, Susps[i]);
		WheelsData.setSuspTravelDirection(i, SuspTravelDirs[i]);
		WheelsData.setWheelCentreOffset(i, WheelCenterCMOffsets[i]);
		WheelsData.setSuspForceAppPointOffset(i, SuspForceAppCMOffsets[i]);
		WheelsData.setTireForceAppPointOffset(i, TireForceAppCMOffsets[i]);
	}

	WheelsData.setSubStepCount(5.0f, 10, 5);
}

void SetupDriveSimulationData(
	XMFLOAT3 const WheelCenterOffsets[4], 
	PxVehicleDriveSimData4W &DriveData)
{
	// Differential
	PxVehicleDifferential4WData Diff;
	Diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
	DriveData.setDiffData(Diff);

	// Engine
	PxVehicleEngineData Engine;
	Engine.mPeakTorque = 1000.0f;
	Engine.mDampingRateFullThrottle = 0.3f;
	Engine.mDampingRateZeroThrottleClutchEngaged = 1.0f;

	float fRPM = 7000.0f;
	Engine.mMaxOmega = ConvertRPMToRPS(fRPM);
	DriveData.setEngineData(Engine);

	// Gears
	PxVehicleGearsData Gears;
	Gears.mSwitchTime = 0.1f;
	Gears.mFinalRatio = 4.0f;
	DriveData.setGearsData(Gears);

	// Clutch
	PxVehicleClutchData Clutch;
	Clutch.mStrength = 100.0f;
	DriveData.setClutchData(Clutch);

	PxVehicleAutoBoxData AutoBox;
	//AutoBox.setLatency();
	DriveData.setAutoBoxData(AutoBox);

	// Ackermann steer accuracy
	PxVehicleAckermannGeometryData Ackermann;
	Ackermann.mAccuracy = 1.0f;
	Ackermann.mAxleSeparation =
		WheelCenterOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].z - 
		WheelCenterOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT].z;

	Ackermann.mFrontWidth =
		WheelCenterOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].x -
		WheelCenterOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].x;

	Ackermann.mRearWidth =
		WheelCenterOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].x -
		WheelCenterOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT].x;

	DriveData.setAckermannGeometryData(Ackermann);
}