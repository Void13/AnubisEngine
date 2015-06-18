#pragma once

#include "EnginePCH.h"

typedef std::vector<CVertex> GPUObjectVertices;
typedef std::vector<CIndex> GPUObjectIndices;

enum class E_RIGID_BODY_FLAG
{
	STATIC = 0,
	DYNAMIC,
	KINEMATIC
};

enum class E_MESH_TOPOLOGY
{
	POINTLIST = 0,
	TRIANGLELIST,
	TRIANGLESTRIP
};

enum class E_MESH_TYPE
{
	TRIANGLEMESH = 0,
	SPHERE,
	PLANE,
	HEIGHTFIELD,
	CAPSULE,
	BOX
};

struct CRawModel;

struct CRawMesh
{
public:
	CRawMesh(CRawModel *pRawModel, E_MESH_TYPE eMeshType, E_MESH_TOPOLOGY _Topology, E_RIGID_BODY_FLAG eBodyFlag) :
		m_pRawModel(pRawModel), m_Topology(_Topology), m_eMeshType(eMeshType), m_eBodyFlag(eBodyFlag)
	{
	};

	E_MESH_TOPOLOGY GetTopology() const
	{
		return m_Topology;
	};

	E_MESH_TYPE GetMeshType() const
	{
		return m_eMeshType;
	};

	CAABB				m_AABB;

	std::string			m_sName;

	E_RIGID_BODY_FLAG	m_eBodyFlag;

	GPUObjectVertices	m_Vertices;
	GPUObjectIndices	m_Indices;

	CMaterial			*m_pMaterial = nullptr;

	CRawModel			const *const m_pRawModel = nullptr;

	XMMATRIX			m_mLocalWorld = XMMatrixIdentity();
private:
	E_MESH_TOPOLOGY		m_Topology;

	// чтобы создавать нужный PhysXShape
	E_MESH_TYPE			m_eMeshType;
};

struct CRawModel
{
public:
	~CRawModel()
	{
		for (auto pRawMesh : m_RawMeshes)
		{
			SAFE_DELETE(pRawMesh);
		}
	};

	XMFLOAT3			m_vScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	std::vector<CMaterial *> m_Materials;
	float				m_fLODDistance = 200.0f;
	std::string			m_sModelName;
	std::string			m_sCenterMesh;
	E_RIGID_BODY_FLAG	m_eRidigBodyFlag = E_RIGID_BODY_FLAG::STATIC;

	// будет ли у разных актёров одна модель, или для разных актёров копии этой модели
	// true - один актёр - одна модель
	// false - много актёров - одна модель
	bool				m_bIsUnique = false;

	std::vector<CRawMesh *> m_RawMeshes;
};

typedef std::vector<CRawMesh *> RAW_MESHES;