#include "EnginePCH.h"
#include "Vehicle.h"

#include "MainLoopHandler.h"
#include "Physics/Vehicle/VehicleManager.h"

IPhysicalBehavior *CVehicleActor::VGetPhysicsBehavior()
{
	if (!m_pPhysicsBehavior)
	{
		m_pPhysicsBehavior = new CVehiclePhysics();
	}

	return m_pPhysicsBehavior;
};

void CVehicleActor::VInit()
{
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_KEYS_INPUT, &CVehicleActor::OnKeysInput, this);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_MOUSE_INPUT, &CVehicleActor::OnMouseInput, this);

	IActor::VInit();

	auto pVehicleBehavior = (CVehiclePhysics *)VGetPhysicsBehavior();
	m_pVehicle = pVehicleBehavior->m_pVehicle;
	m_nVehicleID = pVehicleBehavior->m_nVehicleID;

	m_pVehicle->mDriveDynData.mUseAutoGears = true;

	m_pVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
};

XMFLOAT3 CVehicleActor::GetVehicleForwardDirection() const
{
	PxQuat vQuat = m_pVehicle->getRigidDynamicActor()->getGlobalPose().q;
	vQuat.normalize();

	// try + and -
	PxVec3 vDir;
	vDir.x = 2.0f * (vQuat.x * vQuat.z + vQuat.w * vQuat.y);
	vDir.y = 2.0f * (vQuat.y * vQuat.x - vQuat.w * vQuat.x);
	vDir.z = 1.0f - 2.0f * (vQuat.x * vQuat.x + vQuat.y * vQuat.y);

	return vDir;
};

XMFLOAT3 CVehicleActor::GetVehicleRightDirection() const
{
	PxQuat vQuat = m_pVehicle->getRigidDynamicActor()->getGlobalPose().q;
	vQuat.normalize();

	PxVec3 vDir;
	vDir.x = 1.0f - 2.0f * (vQuat.y * vQuat.y + vQuat.z * vQuat.z);
	vDir.y = 2.0f * (vQuat.x * vQuat.y + vQuat.w * vQuat.z);
	vDir.z = 2.0f * (vQuat.x * vQuat.z - vQuat.w * vQuat.y);

	return vDir;
};

XMFLOAT3 CVehicleActor::GetVehicleUpDirection() const
{
	PxQuat vQuat = m_pVehicle->getRigidDynamicActor()->getGlobalPose().q;
	vQuat.normalize();

	PxVec3 vDir;
	vDir.x = 2.0f * (vQuat.x * vQuat.y - vQuat.w * vQuat.z);
	vDir.y = 1.0f - 2.0f * (vQuat.x * vQuat.x + vQuat.z * vQuat.z);
	vDir.z = 2.0f * (vQuat.y * vQuat.z + vQuat.w * vQuat.x);

	return vDir;
};

PxF32 g_SteerVsForwardSpeedData[2 * 8] =
{
	0.0f, 1.0f,
	5.0f, 1.0f,
	30.0f, 0.8f,
	60.0f, 0.6f,
	120.0f, 0.1f,

	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> g_SteerVsForwardSpeedTable(g_SteerVsForwardSpeedData, 4);

void CVehicleActor::VUpdate()
{
	if (m_pVehicle)
	{
		if (m_bUpdateTransform)
		{
			//m_pVehicle->mDriveDynData.setToRestState();
		}

		//m_pVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		
		/*
		float fForwardSpeed = m_pVehicle->computeForwardSpeed();
		float fSideSpeed = m_pVehicle->computeSidewaysSpeed();

		// если текущая передача == задняя
		if (m_bIsReverse)
		{
			if (m_pVehicle->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE)
			{
				// и нажата кнопка назад, значит мы едем назад
				m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, (float)m_bIsBrake);

				// если нажата кнопка вперёд - надо тормозить
				if (m_pVehicle->computeForwardSpeed() < 0)
				{
					m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, (float)m_bIsAccel);
				}
				else
				{
					// а если не едем - то включаем нулевую или первую
					m_pVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eNEUTRAL);
					m_bIsReverse = false;
				}
			}
		}
		else
		{
			// если не задняя

			// если стоим на месте и нажимаем назад - то меняем передачу
			if (fForwardSpeed < 0.1f && fSideSpeed < 0.1f)
			{
				if (m_bIsBrake)
				{
					m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, 0.0f);
					m_pVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
					m_bIsReverse = true;
				}
				else if (!m_bIsAccel)
				{
					// если не тормозим - ставим нейтралку
					m_pVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eNEUTRAL);
					m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, 1.0f);
				}
				else if (m_bIsAccel)
				{
					m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, 0.0f);
					m_pVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
				}

				m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, 0.0f);
			}
			else
			{
				m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, (float)m_bIsBrake);

				// если что, здесь надо поставить передачу нужную, если m_bIsBrake == 1
			}
			
			// переключаем скорость за 1000 до отсечки
			//if (ConvertRPSToRPM(m_pVehicle->mDriveDynData.getEngineRotationSpeed()) >
			//	ConvertRPSToRPM(((PxVehicleDrive4W *)m_pVehicle)->mDriveSimData.getEngineData().mMaxOmega) - 2000.0f)
			//{
			//	m_pVehicle->mDriveDynData.setGearUp(!m_bIsBrake);
			//}

			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, (float)m_bIsAccel);
		}
		
		float fElapsed = (float)CMainLoopHandler::GetInstance()->GetLogicElapsedTime() * 0.001f;

		float fEps = 0.01f;

		// поворот налево
		if (m_bIsLeft)
			m_fSteerAngle -= fElapsed * 2.0f;
		else if (m_bIsRight)
			m_fSteerAngle += fElapsed * 2.0f;
		else if (m_fSteerAngle < -fEps || m_fSteerAngle > fEps)
		{
			// go to 0
			if (m_fSteerAngle > 0)
				m_fSteerAngle -= fElapsed * 0.9f;
			else
				m_fSteerAngle += fElapsed * 0.9f;
		}
		else
			m_fSteerAngle = 0.0f;

		bool bIsVehicleInAir = PxVehicleIsInAir(CVehicleManager::GetInstance()->GetVehicleQueryResult(m_nVehicleID));
		m_fSteerAngle = m_fSteerAngle * (bIsVehicleInAir ? 1.0f : g_SteerVsForwardSpeedTable.getYVal(fForwardSpeed));

		m_fSteerAngle = MathHelper::Clamp(m_fSteerAngle, -1.0f, 1.0f);

		if (m_fSteerAngle < -fEps)
		{
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT,		-m_fSteerAngle);
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT,	0.0f);
		}
		else if (m_fSteerAngle > fEps)
		{
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT,		0.0f);
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT,	m_fSteerAngle);
		}
		else
		{
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT,		0.0f);
			m_pVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT,	0.0f);
		}*/
	}

	IActor::VUpdate();
}

void CVehicleActor::OnKeysInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown)
{
	switch (_eInputEvent.e_event)
	{
	case E_ENGINE_INPUT_EVENTS::VEHICLE_ACCELERATE:
	{
		m_bIsAccel = _bKeyDown;

		break;
	}
	case E_ENGINE_INPUT_EVENTS::VEHICLE_BRAKE:
	{
		m_bIsBrake = _bKeyDown;

		break;
	}
	case E_ENGINE_INPUT_EVENTS::VEHICLE_STEER_LEFT:
		m_bIsLeft = _bKeyDown;

		break;
	case E_ENGINE_INPUT_EVENTS::VEHICLE_STEER_RIGHT:
		m_bIsRight = _bKeyDown;

		break;
	};
}

void CVehicleActor::OnMouseInput(E_ENGINE_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDelta, XMINT2 const &_vGlobalPos)
{

}