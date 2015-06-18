#pragma once

#include "..\\BasePhysicsBehavior.h"

#include "Physx//vehicle//PxVehicleUtil.h"

// (Radians per second) to (Revolutions per minute)
float ConvertRPSToRPM(float fRPS);

// (Revolutions per minute) to (radians per second)
float ConvertRPMToRPS(float fRPM);

// описывает конкретное поведение актёра
class CVehiclePhysics final : public IPhysicalBehavior
{
public:
	virtual ~CVehiclePhysics()
	{
	};

	virtual void VInit(IActor *pActor) override;

	PxVehicleDrive		*m_pVehicle = nullptr;
	size_t				m_nVehicleID = -1;
};