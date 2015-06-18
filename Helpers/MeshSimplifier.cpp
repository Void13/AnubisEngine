#include "EnginePCH.h"

#include "MeshSimplifier.h"

#include "GraphicsRenderers\\VertexStructs.h"
#include "GraphicsRenderers\\BaseMeshRenderer.h"

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/local_optimization.h>
#include <VCG/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>
#include <VCG/complex/algorithms/create/platonic.h>

using namespace vcg;

class MyVertex;
class MyEdge;
class MyFace;

struct MyUsedTypes : public UsedTypes<Use<MyVertex>::AsVertexType, Use<MyEdge>::AsEdgeType, Use<MyFace>::AsFaceType>
{
};

class MyVertex : public Vertex<MyUsedTypes,
	vertex::VFAdj,
	vertex::Coord3f,
	vertex::Normal3f,
	vertex::TexCoord2f,
	vertex::Mark,
	vertex::BitFlags>
{
public:
	vcg::math::Quadric<double> &Qd()
	{
		return q;
	};
private:
	math::Quadric<double> q;
};

class MyEdge : public Edge<MyUsedTypes>
{
};

typedef BasicVertexPair<MyVertex> VertexPair;

class MyFace : public Face<MyUsedTypes,
	face::VFAdj,
	face::VertexRef,
	face::BitFlags>
{
};

class MyMesh : public tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace>>
{
};

class MyTriEdgeCollapse :
	public TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse, QInfoStandard<MyVertex>>
{
public:
	typedef TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse, QInfoStandard<MyVertex>> TECQ;
	typedef MyMesh::VertexType::EdgeType EdgeType;

	inline MyTriEdgeCollapse(const VertexPair &p, int i, BaseParameterClass *pp) : TECQ(p, i, pp)
	{
	};
};

bool SimplifyMesh(CRawMesh *pRawMesh, int nMaximumVertices, int nMaximumTriangles)
{
	if (nMaximumVertices < 50 || nMaximumTriangles < 20)
		return true;

	MyMesh mesh;
	
	auto vi = Allocator<MyMesh>::AddVertices(mesh, pRawMesh->m_Vertices.size());
	for (size_t i = 0; i < pRawMesh->m_Vertices.size(); i++)
	{
		auto iVert = pRawMesh->m_Vertices[i];

		(*vi).P() = Point3f(iVert.vPosition.x, iVert.vPosition.y, iVert.vPosition.z);
		(*vi).N() = Point3f(iVert.vNormal.x, iVert.vNormal.y, iVert.vNormal.z);
		(*vi).T() = TexCoord2f(iVert.vTexCoords.x, iVert.vTexCoords.y);

		//vi->Flags() = 40;

		vi++;
	}
	
	Allocator<MyMesh>::AddFaces(mesh, pRawMesh->m_Indices.size() / 3);

	for (int i = 0; i < mesh.fn; i++)
	{
		mesh.face[i].Alloc(3);

		// 262144 1048576
		// faux0, faux2

		for (int j = 0; j < 3; j++)
		{
			mesh.face[i].ClearF(j);

			mesh.face[i].V(j) = &(mesh.vert[pRawMesh->m_Indices[i * 3 + j]]);
		}
	}

	TriEdgeCollapseQuadricParameter qparams;
	qparams.QualityThr = 0.3;
	qparams.SafeHeapUpdate = true;
	//qparams.FastPreserveBoundary = true;
	qparams.PreserveBoundary = true;
	qparams.PreserveTopology = false;

	int dup = Clean<MyMesh>::RemoveDuplicateVertex(mesh);

	UpdateBounding<MyMesh>::Box(mesh);

	LocalOptimization<MyMesh> DeciSession(mesh, &qparams);
	DeciSession.Init<MyTriEdgeCollapse>();

	// если кол-во вершин установлено
	if (nMaximumVertices < mesh.vn - 5 && nMaximumVertices > 0)
	{
		DeciSession.SetTargetVertices(nMaximumVertices);
	}

	if (nMaximumTriangles < mesh.fn && nMaximumTriangles > 0)
	{
		DeciSession.SetTargetSimplices(nMaximumTriangles);
	}

	DeciSession.SetTimeBudget(0.5f);

	float fTargetError = std::numeric_limits<float>::max();
	if (fTargetError < std::numeric_limits<float>::max())
		DeciSession.SetTargetMetric(fTargetError);

	LOG_INFO("Simplify mesh %s %d to %d", pRawMesh->m_sName.c_str(), mesh.fn, nMaximumTriangles);

	while (DeciSession.DoOptimization() &&
		mesh.fn > nMaximumTriangles &&
		mesh.vn > nMaximumVertices &&
		DeciSession.currMetric < fTargetError)
	{
		LOG_INFO("Current mesh FN: %7i, Heap: %9i, Error: %9g",
			mesh.fn, DeciSession.h.size(), DeciSession.currMetric);
	}

	LOG_INFO("Simplify mesh %s: FN %d to %d, VN: %d to %d", 
		pRawMesh->m_sName.c_str(), pRawMesh->m_Indices.size() / 3, mesh.fn, pRawMesh->m_Vertices.size(), mesh.vn);

	pRawMesh->m_Vertices.clear();
	pRawMesh->m_Indices.clear();

	SimpleTempData<MyMesh::VertContainer, int> indices(mesh.vert);

	int nVertex = 0;
	for (auto it = mesh.vert.begin(); it != mesh.vert.end(); it++)
	{
		auto iVertex = *it;

		if (!iVertex.IsD())
		{
			indices[it] = nVertex++;

			CVertex vert;
			vert.vPosition = XMFLOAT3(iVertex.P().X(), iVertex.P().Y(), iVertex.P().Z());
			vert.vNormal = XMFLOAT3(iVertex.N().X(), iVertex.N().Y(), iVertex.N().Z());
			vert.vTexCoords = XMFLOAT2(iVertex.T().U(), iVertex.T().V());

			pRawMesh->m_Vertices.push_back(vert);
		}
	}

	for (auto iFace : mesh.face)
	{
		if (!iFace.IsD())
		{
			pRawMesh->m_Indices.push_back(indices[iFace.V(0)]);
			pRawMesh->m_Indices.push_back(indices[iFace.V(1)]);
			pRawMesh->m_Indices.push_back(indices[iFace.V(2)]);
		}
	}

	return true;
}