#pragma once

#include "EnginePCH.h"
#include "BaseActor.h"
#include "Object.h"

#include "Physics/Vehicle/VehicleBehavior.h"

class CVehicleActor : public IActor
{
public:
	CVehicleActor(std::string const &sActorFile, std::string const &sModelFile = "") :
		IActor(sActorFile, sModelFile)
	{
		m_eActorType = E_ACTOR_TYPE::VEHICLE;
		m_ModelResource.m_bIsUnique = true;

		m_bIsReverse = false;
		m_bIsAccel = false;
		m_bIsBrake = false;
	};

	virtual ~CVehicleActor()
	{
	};

	XMFLOAT3 GetVehicleForwardDirection() const;
	XMFLOAT3 GetVehicleUpDirection() const;
	XMFLOAT3 GetVehicleRightDirection() const;

	// from -1 to 1
	void SetSteering(float fSteer)
	{
		float fEps = 0.01f;

		fSteer = MathHelper::Clamp(fSteer, -1.0f, 1.0f);

		if (fSteer < -fEps)
		{
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, -fSteer);
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, 0.0f);
		}
		else if (fSteer > fEps)
		{
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, 0.0f);
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, fSteer);
		}
		else
		{
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, 0.0f);
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, 0.0f);
		}
	};

	// from 0 to 1
	void SetBraking(float fBrake)
	{
		m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, fBrake);
	};

	// from 0 to 1
	void SetAccel(float fAccel)
	{
		m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, fAccel);
	};

	IPhysicalBehavior *VGetPhysicsBehavior() override;

	virtual void VInit() override;

	PxVehicleDrive *GetVehicle() const
	{
		return m_pVehicle;
	};

	virtual void VUpdate() override;

	void OnKeysInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown);
	void OnMouseInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDelta, XMINT2 const &_vGlobalPos);
protected:
	bool					m_bIsReverse = false;
	bool					m_bIsAccel = false;
	bool					m_bIsBrake = false;
	bool					m_bIsLeft = false;
	bool					m_bIsRight = false;

	// from -1 to 1
	float					m_fSteerAngle = 0.0f;

	size_t					m_nVehicleID = -1;
	PxVehicleDrive			*m_pVehicle = nullptr;

	std::vector<CObject *>	m_Wheels;
	std::vector<CObject *>	m_Chassis;
};