#include "EnginePCH.h"

#include "ShapeCreator.h"

#include "Rescaling.h"
#include "AllocatorCallback.h"

#include "MeshSimplifier.h"
#include "FilterShader.h"
#include "Vehicle\\VehicleRaycast.h"

namespace ShapeCreator
{
	PxShape *CreateShape(CRawMesh const &RawMesh)
	{
		// надо брать откуда-то уникальность шейпа

		if (RawMesh.m_Vertices.size() < 8)
			return nullptr;
		if (RawMesh.m_Indices.size() < 10)
			return nullptr;

		auto pMaterial = CPhysicsSystem::GetInstance()->GetTarmacMaterial();

		auto _CreateShape = [&](PxGeometry *pGeometry)
		{
			//PhysXRescaling::Scale(*pGeometry, scale);
			
			auto pPhysics = CPhysicsSystem::GetInstance()->GetPhysics();

			// RawMesh.m_bIsUnique
			auto pShape = pPhysics->createShape(*pGeometry, *pMaterial, false);

			PxFilterData simFilterData;
			simFilterData.word0 = COLLISION_FLAG_DRIVABLE_OBSTACLE;
			simFilterData.word1 = COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST;

			PxFilterData qryFilterData;
			qryFilterData.word3 = (PxU32)DRIVABLE_SURFACE;

			pShape->setSimulationFilterData(simFilterData);
			pShape->setQueryFilterData(qryFilterData);

			return pShape;
		};

		switch (RawMesh.GetMeshType())
		{
		case E_MESH_TYPE::TRIANGLEMESH:
		{
			PxCooking *pCooking = CPhysicsSystem::GetInstance()->GetCooking();
			PxPhysics *pPhysics = CPhysicsSystem::GetInstance()->GetPhysics();

			int Flags = PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES |
				PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES;
			
			if (RawMesh.m_eBodyFlag == E_RIGID_BODY_FLAG::STATIC)
			{
				// get only decimated triangles

				PxTriangleMeshDesc MeshDesc;
				
				// indices
				MeshDesc.triangles.count = RawMesh.m_Indices.size() / 3;
				MeshDesc.triangles.stride = sizeof(CIndex) * 3;
				MeshDesc.triangles.data = &RawMesh.m_Indices[0];

				// vertices
				MeshDesc.points.count = RawMesh.m_Vertices.size();
				MeshDesc.points.stride = sizeof(CVertex);
				MeshDesc.points.data = &RawMesh.m_Vertices[0];

				PxDefaultMemoryOutputStream OutputStream;

				bool bResult = pCooking->cookTriangleMesh(MeshDesc, OutputStream);
				if (!bResult)
					return nullptr;

				PxDefaultMemoryInputData InputStream(OutputStream.getData(), OutputStream.getSize());

				PxTriangleMesh *pMesh = pPhysics->createTriangleMesh(InputStream);

				PxGeometry &Geometry = PxTriangleMeshGeometry(pMesh);
				
				return _CreateShape(&Geometry);
			}
			else
			{
				PxConvexMeshDesc ConvexMeshDesc;

				ConvexMeshDesc.points.count = RawMesh.m_Vertices.size();
				ConvexMeshDesc.points.stride = sizeof(CVertex);
				ConvexMeshDesc.points.data = &RawMesh.m_Vertices[0];

				ConvexMeshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX; // | PxConvexFlag::eCHECK_ZERO_AREA_TRIANGLES;

				PxDefaultMemoryOutputStream OutputStream;

				bool bResult = pCooking->cookConvexMesh(ConvexMeshDesc, OutputStream);
				if (!bResult)
					return nullptr;

				PxDefaultMemoryInputData InputStream(OutputStream.getData(), OutputStream.getSize());

				PxConvexMesh *pMesh = pPhysics->createConvexMesh(InputStream);

				PxGeometry &Geometry = PxConvexMeshGeometry(pMesh);

				return _CreateShape(&Geometry);
			}
		}
		
			break;
		case E_MESH_TYPE::SPHERE:
		{
			PxGeometry &Geometry = PxSphereGeometry(XMVectorGetX(XMVector3Length(XMLoadFloat3(&RawMesh.m_pRawModel->m_vScale))));
			return _CreateShape(&Geometry);
		}
			break;
		case E_MESH_TYPE::PLANE:
		{
			PxGeometry &Geometry = PxPlaneGeometry();
			return _CreateShape(&Geometry);
		}
			break;
		case E_MESH_TYPE::HEIGHTFIELD:
		{
			// TODO
			PxGeometry &Geometry = PxHeightFieldGeometry();
			return _CreateShape(&Geometry);
		}
			break;
		case E_MESH_TYPE::CAPSULE:
		{
			PxGeometry &Geometry = PxCapsuleGeometry(1.0f, 1.0f);
			return _CreateShape(&Geometry);
		}
			break;
		case E_MESH_TYPE::BOX:
		{
			PxGeometry &Geometry = PxBoxGeometry(RawMesh.m_pRawModel->m_vScale);
			return _CreateShape(&Geometry);
		}
			break;
		};

		return nullptr;
	}
};