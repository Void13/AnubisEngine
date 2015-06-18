#include "EnginePCH.h"
#include "BaseMeshRenderer.h"

#include "MeshSimplifier.h"

void Simplify(size_t nNumTargetTriangles, double fArgessiveness)
{
	return;
	/*
	CMeshSimplifier MeshSimpl;

	for (auto iV : m_Vertices)
	{
		CMeshSimplifier::Vertex v;
		v.v = iV;

		MeshSimpl.vertices.push_back(v);
	}

	size_t nNumTriangles = 0;
	for (auto iSplit : m_MaterialSplits)
	{
		for (size_t i = 0; i < iSplit.Indices.size(); i += 3)
		{
			CMeshSimplifier::Triangle t;

			t.v[0] = iSplit.Indices[i + 0];
			t.v[1] = iSplit.Indices[i + 1];
			t.v[2] = iSplit.Indices[i + 2];

			MeshSimpl.triangles.push_back(t);

			nNumTriangles++;
		}
	}

	LOG_INFO("Simplify(before): Vertices: %d, Indices: %d", MeshSimpl.vertices.size(), MeshSimpl.triangles.size() * 3);

	MeshSimpl.simplify_mesh((int)(nNumTriangles * 0.5f));

	LOG_INFO("Simplify(after): Vertices: %d, Indices: %d", MeshSimpl.vertices.size(), MeshSimpl.triangles.size() * 3);

	for (size_t i = 0; i < MeshSimpl.vertices.size(); i++)
	{
		m_Vertices[i] = MeshSimpl.vertices[i].v;
	}
	m_Vertices.resize(MeshSimpl.vertices.size());

	int nOffset = 0;
	for (size_t i = 0; i < MeshSimpl.triangles.size(); i++)
	{
		if (!MeshSimpl.triangles[i].deleted)
		{
			m_MaterialSplits[0].Indices[nOffset + 0] = m_MaterialSplits[0].Indices[i * 3 + 0];
			m_MaterialSplits[0].Indices[nOffset + 1] = m_MaterialSplits[0].Indices[i * 3 + 1];
			m_MaterialSplits[0].Indices[nOffset + 2] = m_MaterialSplits[0].Indices[i * 3 + 2];
			
			m_MaterialSplits[0].Indices[nOffset + 0] = MeshSimpl.triangles[i].v[0];
			m_MaterialSplits[0].Indices[nOffset + 1] = MeshSimpl.triangles[i].v[1];
			m_MaterialSplits[0].Indices[nOffset + 2] = MeshSimpl.triangles[i].v[2];

			nOffset += 3;
		}
	}
	m_MaterialSplits[0].Indices.resize(nOffset);*/
}