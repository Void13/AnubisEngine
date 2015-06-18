#pragma once

#include <PhysX\\PxPhysicsAPI.h>

namespace PVD
{
	using namespace physx::debugger;
	using namespace physx::debugger::comm;
}

using namespace physx;

class CErrorCallback : public PxErrorCallback
{
public:
	CErrorCallback()
	{
	};

	~CErrorCallback()
	{
	};

	void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		if (code == 0)
			return;

		if (code == 1 || code == 2 || code == 128)
		{
			CLogManager::GetInstance()->AddMessage(E_LOGTYPE::LOG_WARNING, "PhysX(%d): %s", file, line, code, message);
		}
		else
		{
			CLogManager::GetInstance()->AddMessage(E_LOGTYPE::LOG_ERROR, "PhysX(%d): %s", file, line, code, message);

			CHError er = new CError_PhysXError(file, line);
		}
	}
};

class CPhysicsSystem : 
	public CSingleton<CPhysicsSystem>,
	public PVD::PvdConnectionHandler
{
	friend class CSingleton<CPhysicsSystem>;
public:
	void Init();
	void Simulate();
	void Release();

	PxPhysics *GetPhysics()
	{
		return m_pPhysicsSDK;
	}

	PxScene *GetScene()
	{
		return m_pScene;
	}

	PxCooking *GetCooking()
	{
		return m_pCooking;
	};

	PxMaterial *CPhysicsSystem::GetMUDMaterial()
	{
		if (!m_pMaterialMUD)
		{
			m_pMaterialMUD = m_pPhysicsSDK->createMaterial(0.5f, 0.5f, 0.1f);
		}

		return m_pMaterialMUD;
	};

	PxMaterial *CPhysicsSystem::GetTarmacMaterial()
	{
		if (!m_pMaterialTarmac)
		{
			m_pMaterialTarmac = m_pPhysicsSDK->createMaterial(0.72f, 0.62f, 0.01f);
		}

		return m_pMaterialTarmac;
	};

	PxMaterial *CPhysicsSystem::GetRubberMaterial()
	{
		if (!m_pMaterialRubber)
		{
			m_pMaterialRubber = m_pPhysicsSDK->createMaterial(1.16f, 0.9f, 0.1f);
		}

		return m_pMaterialRubber;
	};

	PxMaterial *CPhysicsSystem::GetIceMaterial()
	{
		if (!m_pMaterialIce)
		{
			m_pMaterialIce = m_pPhysicsSDK->createMaterial(0.08f, 0.05f, 0.1f);
		}

		return m_pMaterialIce;
	};

	PxMaterial *CPhysicsSystem::GetGrassMaterial()
	{
		if (!m_pMaterialGrass)
		{
			m_pMaterialGrass = m_pPhysicsSDK->createMaterial(0.35f, 0.25f, 0.1f);
		}

		return m_pMaterialGrass;
	};

	XMFLOAT3 const &GetGravity() const
	{
		return m_vGravity;
	};
private:
	// какая-то грязь, глина, песок.
	PxMaterial				*m_pMaterialMUD = nullptr;
	PxMaterial				*m_pMaterialTarmac = nullptr;
	PxMaterial				*m_pMaterialIce = nullptr;
	PxMaterial				*m_pMaterialGrass = nullptr;
	PxMaterial				*m_pMaterialRubber = nullptr;

	PxScene					*m_pScene = nullptr;
	PxCooking				*m_pCooking = nullptr;

	PxPhysics				*m_pPhysicsSDK = nullptr;
	PxFoundation			*m_pFoundation = nullptr;

	PxCpuDispatcher			*m_pCPUDispatcher = nullptr;
	PxGpuDispatcher			*m_pGPUDispatcher = nullptr;

	CErrorCallback			m_DefaultErrorCallback;
	PxDefaultAllocator		m_DefaultAllocatorCallback;

	bool					m_bIsFetchDone = true;

	XMFLOAT3				m_vGravity = XMFLOAT3(0.0f, -9.81f, 0.0f);

	// PVD

	PxVisualDebuggerConnectionManager	*m_pPDV = nullptr;

	void					CreatePVDConnection();
	virtual			void	onPvdSendClassDescriptions(PVD::PvdConnection&)
	{
	};

	virtual			void	onPvdConnected(PVD::PvdConnection& inFactory);
	virtual			void	onPvdDisconnected(PVD::PvdConnection& inFactory);
};