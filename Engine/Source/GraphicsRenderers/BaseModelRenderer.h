#pragma once

#include "EnginePCH.h"
#include "VertexStructs.h"
#include "BaseGraphicsRenderer.h"
#include "Material.h"
#include "RawMesh.h"

class ICamera;
class IBaseMeshRenderer;
struct CRawMesh;
class IActor;

class IBaseModelRenderer
{
public:
	IBaseModelRenderer()
	{
	};

	virtual ~IBaseModelRenderer()
	{
	};

	E_RENDERMETHOD GetRenderMethod() const
	{
		return m_eRenderMethod;
	};

	void SetRenderMethod(E_RENDERMETHOD eRenderMethod)
	{
		m_eRenderMethod = eRenderMethod;
	};

	std::vector<IBaseMeshRenderer *> const &GetMeshes() const
	{
		return m_Meshes;
	};

	std::string const &GetName() const
	{
		return m_sModelName;
	};

	void SetName(std::string const &sName)
	{
		m_sModelName = sName;
	};

	std::string const &GetCenterMeshName() const
	{
		return m_sCenterMesh;
	};

	void SetCenterMeshName(std::string const &sCenterMeshName)
	{
		m_sCenterMesh = sCenterMeshName;
	};

	void SetLODDistance(float fLODDistance)
	{
		m_fLODDistance = fLODDistance;
	};

	float GetLODDistance() const
	{
		return m_fLODDistance;
	};

	void SetRigidBodyFlag(E_RIGID_BODY_FLAG eBodyFlag)
	{
		m_eBodyFlag = eBodyFlag;
	};

	E_RIGID_BODY_FLAG GetRigidBodyFlag() const
	{
		return m_eBodyFlag;
	};

	IBaseMeshRenderer *GetMeshByMeshName(std::string const &sMeshName) const;

	MaterialsVector GetMaterials() const
	{
		return m_Materials;
	};

	void AddMaterial(CMaterial *pMaterial)
	{
		m_Materials.push_back(pMaterial);
	};

	void SetTexturesFolder(std::string const &sTexturesFolder)
	{
		m_sTexturesFolder = sTexturesFolder;
	};

	std::string const &GetTexturesFolder() const
	{
		return m_sTexturesFolder;
	};

	bool IsUniqueForActor() const
	{
		return m_bIsUnique;
	};

	void SetUniquness(bool bUnique)
	{
		m_bIsUnique = bUnique;
	};

	virtual void VAddMesh(CRawMesh const *pRawMesh, CRawMesh const *pPhysicsRawMesh) = 0;

	virtual void VRender(std::vector<IActor *> const &Actors, ICamera const *const _pCamera) = 0;
protected:
	virtual void VCreateInstanceBuffer(DWORD dwNumInstances) = 0;
protected:
	std::string							m_sModelName;
	std::string							m_sTexturesFolder;

	std::vector<IBaseMeshRenderer *>	m_Meshes;
	std::unordered_map<std::string, IBaseMeshRenderer *> m_MeshNameToMesh;
	std::string							m_sCenterMesh;

	// будет ли у разных актёров одна модель, или для разных актёров копии этой модели
	// true - один актёр - одна модель
	// false - много актёров - одна модель
	bool								m_bIsUnique = false;

	MaterialsVector						m_Materials;

	E_RIGID_BODY_FLAG					m_eBodyFlag = E_RIGID_BODY_FLAG::STATIC;
	E_RENDERMETHOD						m_eRenderMethod = E_RENDERMETHOD::SOLID;

	DWORD								m_dwNumInstances = 0;
	DWORD								m_dwAllocatedInstances = 0;

	float								m_fLODDistance = 150.0f;
};