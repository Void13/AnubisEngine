#include "EnginePCH.h"
#include "PhysXLoader.h"

#include "MainLoopHandler.h"

#include "BaseActor.h"

#ifndef _DEBUG
	#pragma comment( lib, "PhysX\\PhysX3_x86.lib" )
	#pragma comment( lib, "PhysX\\PhysX3Common_x86.lib" )
	#pragma comment( lib, "PhysX\\PhysX3Cooking_x86.lib" )
	#pragma comment( lib, "PhysX\\PxTask.lib" )
	#pragma comment( lib, "PhysX\\PhysXProfileSDK.lib" )
	#pragma comment( lib, "PhysX\\PhysX3Extensions.lib" )
	#pragma comment( lib, "PhysX\\PhysX3CharacterKinematic_x86.lib" )
	#pragma comment( lib, "PhysX\\PhysXVisualDebuggerSDK.lib" )
	#pragma comment( lib, "PhysX\\PhysX3Vehicle.lib" )
#else
	#pragma comment( lib, "PhysX\\PhysX3DEBUG_x86.lib" )
	#pragma comment( lib, "PhysX\\PhysX3CommonDEBUG_x86.lib" )
	#pragma comment( lib, "PhysX\\PhysX3CookingDEBUG_x86.lib" )
	#pragma comment( lib, "PhysX\\PxTaskDEBUG.lib" )
	#pragma comment( lib, "PhysX\\PhysXProfileSDKDEBUG.lib" )
	#pragma comment( lib, "PhysX\\PhysX3ExtensionsDEBUG.lib" )
	#pragma comment( lib, "PhysX\\PhysX3CharacterKinematicDEBUG_x86.lib" )
	#pragma comment( lib, "PhysX\\PhysXVisualDebuggerSDKDEBUG.lib" )
	#pragma comment( lib, "PhysX\\PhysX3VehicleDEBUG.lib" )
#endif

#ifdef USE_D3D11
	#include "../GAPIControllers/D3D11Controller.h"
#endif

#include "FilterShader.h"
#include "Vehicle\VehicleManager.h"

void CPhysicsSystem::Release()
{
	// reverse order
	CVehicleManager::DestroyInstance();

	if (m_pPDV)
		m_pPDV->disconnect();
	if (m_pPDV)
		m_pPDV->release();

	m_pScene->release();
	m_pCooking->release();
	m_pPhysicsSDK->release();
	m_pFoundation->release();
}

void CPhysicsSystem::Simulate()
{
	//if (m_bIsFetchDone)
	{
		float fElapsed = (float)CMainLoopHandler::GetInstance()->GetLogicElapsedTime() * 0.01f;
		//fElapsed = MathHelper::Clamp(fElapsed, 0.015f, 0.017f);

		CVehicleManager::GetInstance()->Update(fElapsed);
		m_pScene->simulate(fElapsed);
	}

	m_bIsFetchDone = m_pScene->fetchResults(true);
}

void CPhysicsSystem::Init()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, 
		m_DefaultAllocatorCallback, m_DefaultErrorCallback);
	if (!m_pFoundation)
	{
		LOG_ERROR("PhysX init failed");
		return;
	}

	PxProfileZoneManager *pProfileZoneManager = 
		&PxProfileZoneManager::createProfileZoneManager(m_pFoundation);
	if (!pProfileZoneManager)
	{
		LOG_ERROR("PhysX init failed");
		return;
	}

	m_pPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation,
		PxTolerancesScale(), true, pProfileZoneManager);
	if (!m_pPhysicsSDK)
	{
		LOG_ERROR("PhysX init failed");
		return;
	}

	PxTolerancesScale Scale;
	Scale.length = 1.0f;

	PxCookingParams CookParams(Scale);
	CookParams.meshWeldTolerance = 0.001f;
	CookParams.meshPreprocessParams = PxMeshPreprocessingFlags(
		PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES | 
		PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES |
		PxMeshPreprocessingFlag::eWELD_VERTICES);

	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, CookParams);
	if (!m_pCooking)
	{
		LOG_ERROR("PhysX init failed");
		return;
	}

	//PxProfileZone *pProfileZone = PxProfileZone::createProfileZone(m_pFoundation, "SampleProfileZone", );
	//pProfileZoneManager->addProfileZone();

	if (!PxInitExtensions(*m_pPhysicsSDK))
	{
		LOG_ERROR("PhysX init failed");
		return;
	}

	// creating scene
	PxSceneDesc SceneDesc(Scale);
	SceneDesc.gravity = m_vGravity;
	//SceneDesc.bounceThresholdVelocity = ;
	//SceneDesc.bounceThresholdVelocity = 20.5f;
	//SceneDesc.frictionType = PxFrictionType::eTWO_DIRECTIONAL;
	//SceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
	SceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	//SceneDesc.flags |= PxSceneFlag::eENABLE_AVERAGE_POINT;

	m_pCPUDispatcher = PxDefaultCpuDispatcherCreate(1);
	if (!m_pCPUDispatcher)
	{
		LOG_ERROR("PhysX init failed");
		return;
	}

	SceneDesc.cpuDispatcher = m_pCPUDispatcher;
	//SceneDesc.filterShader = &PxDefaultSimulationFilterShader;
	SceneDesc.filterShader = &VehicleFilterShader;
	/*
	PxCudaContextManagerDesc cudaCMDesc;
#ifdef USE_D3D11
	cudaCMDesc.graphicsDevice = CD3D11Controller::GetInstance()->GetDevice();
	cudaCMDesc.interopMode = PxCudaInteropMode::D3D11_INTEROP;
#endif

	auto pCudaContext = PxCreateCudaContextManager(*m_pFoundation, 
		cudaCMDesc, pProfileZoneManager);
	if (pCudaContext)
	{
		if (!pCudaContext->contextIsValid())
		{
			pCudaContext->release();
			pCudaContext = nullptr;
		}
		else
		{
			m_pGPUDispatcher = pCudaContext->getGpuDispatcher();
			SceneDesc.gpuDispatcher = m_pGPUDispatcher;
		}
	}
	*/
	m_pScene = m_pPhysicsSDK->createScene(SceneDesc);
	if (!m_pScene)
	{
		LOG_ERROR("PhysX init failed");
		return;
	}

	CVehicleManager::GetInstance()->Init();

	CreatePVDConnection();
}

void CPhysicsSystem::CreatePVDConnection()
{
	m_pPDV = m_pPhysicsSDK->getPvdConnectionManager();
	if (!m_pPDV)
		return;

	m_pPDV->addHandler(*this);

	PxVisualDebuggerConnectionFlags theConnectionFlags(
		PxVisualDebuggerConnectionFlag::eDEBUG | 
		PxVisualDebuggerConnectionFlag::ePROFILE | 
		PxVisualDebuggerConnectionFlag::eMEMORY);

	const char *pPVD_Host_IP = "127.0.0.1";
	int			nPort = 5425;
	unsigned int nTimeout = 100;

	PxVisualDebuggerExt::createConnection(m_pPDV, pPVD_Host_IP, nPort, nTimeout, theConnectionFlags);
}

void CPhysicsSystem::onPvdConnected(PVD::PvdConnection&)
{
	m_pPhysicsSDK->getVisualDebugger()->setVisualizeConstraints(true);
	m_pPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
	m_pPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
	m_pPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONSTRAINTS, true);
}

void CPhysicsSystem::onPvdDisconnected(PVD::PvdConnection& conn)
{
	conn.release();
}