#include "EnginePCH.h"
#include "BaseModelRenderer.h"
#include "BaseMeshRenderer.h"

IBaseMeshRenderer *IBaseModelRenderer::GetMeshByMeshName(std::string const &sMeshName) const
{
	auto it = m_MeshNameToMesh.find(sMeshName);
	if (it == m_MeshNameToMesh.end())
	{
		return nullptr;
	}

	return it->second;
};