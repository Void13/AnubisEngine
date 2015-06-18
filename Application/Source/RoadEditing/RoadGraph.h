#pragma once

#include "EnginePCH.h"
#include "PCH.h"

// точка дороги, которая описывает центральную позицию участка дороги.
struct CRoadPoint
{
	XMFLOAT3	m_vLeftPoint;
	XMFLOAT3	m_vRightPoint;

	XMFLOAT3	m_vPos;
};

/*

	RoadGraph format:
	int nNumCenterPoints;
	[nNumCenterPoints]
		mainpoint.x y z : leftpoint.x y z : rightpoint.x y z

*/

class CRoadGraph
{
public:
	typedef std::vector<CRoadPoint> ROAD_GRAPH;

	void SaveGraph(std::string const &sFile) const
	{
		FILE *pFile;
		fopen_s(&pFile, sFile.c_str(), "w");
		
		fprintf_s(pFile, "%d\n", m_RoadGraph.size());

		for (CRoadPoint const &p : m_RoadGraph)
		{
			fprintf_s(pFile, "%0.2f %0.2f %0.2f : %0.2f %0.2f %0.2f : %0.2f %0.2f %0.2f\n", 
				p.m_vPos.x, p.m_vPos.y, p.m_vPos.z,
				p.m_vLeftPoint.x, p.m_vLeftPoint.y, p.m_vLeftPoint.z,
				p.m_vRightPoint.x, p.m_vRightPoint.y, p.m_vRightPoint.z);
		}

		fclose(pFile);
	};

	void LoadGraph(std::string const &sFile)
	{
		FILE *pFile;
		fopen_s(&pFile, sFile.c_str(), "r");
		if (!pFile)
		{
			CHError er = new CError_FileReading(AT);
			return;
		}

		size_t nRoadGraphSize = 0;
		if (fscanf_s(pFile, "%d", &nRoadGraphSize) != 1)
		{
			fclose(pFile);

			CHError er = new CError_FileReading(AT);
			return;
		}

		for (size_t i = 0; i < nRoadGraphSize; i++)
		{
			CRoadPoint p;

			if (fscanf_s(pFile, "%f %f %f : %f %f %f : %f %f %f\n",
				&p.m_vPos.x, &p.m_vPos.y, &p.m_vPos.z,
				&p.m_vLeftPoint.x, &p.m_vLeftPoint.y, &p.m_vLeftPoint.z,
				&p.m_vRightPoint.x, &p.m_vRightPoint.y, &p.m_vRightPoint.z) != 9)
			{
				fclose(pFile);

				CHError er = new CError_FileReading(AT);
				return;
			}

			m_RoadGraph.push_back(p);
		}

		fclose(pFile);
	};

	void AddMainPoint(XMFLOAT3 const &vLeft, XMFLOAT3 const &vRight)
	{
		XMVECTOR xvMainPoint = (XMLoadFloat3(&vLeft) + XMLoadFloat3(&vRight)) * 0.5f;

		CRoadPoint MainPoint;
		XMStoreFloat3(&MainPoint.m_vPos, xvMainPoint);
		MainPoint.m_vLeftPoint = vLeft;
		MainPoint.m_vRightPoint = vRight;

		m_RoadGraph.push_back(MainPoint);
	};

	void AddMainPoint(XMVECTOR const &xvLeft, XMVECTOR const &xvRight)
	{
		XMVECTOR xvMainPoint = (xvLeft + xvRight) * 0.5f;

		CRoadPoint MainPoint;
		XMStoreFloat3(&MainPoint.m_vPos, xvMainPoint);
		XMStoreFloat3(&MainPoint.m_vLeftPoint, xvLeft);
		XMStoreFloat3(&MainPoint.m_vRightPoint, xvRight);

		m_RoadGraph.push_back(MainPoint);
	};

	CRoadPoint &FindMainPoint(XMVECTOR const &xvPoint, float fSearchRadius)
	{
		for (auto &MP : m_RoadGraph)
		{
			if (XMVector3NearEqual(XMLoadFloat3(&MP.m_vPos), xvPoint, XMVectorReplicate(fSearchRadius)))
			{
				return MP;
			}
		}
	};

	XMFLOAT3 &FindControlPoint(XMVECTOR const &xvPoint, float fSearchRadius)
	{
		for (auto &MP : m_RoadGraph)
		{
			if (XMVector3NearEqual(XMLoadFloat3(&MP.m_vLeftPoint), xvPoint, XMVectorReplicate(fSearchRadius)))
			{
				return MP.m_vLeftPoint;
			}
			
			if (XMVector3NearEqual(XMLoadFloat3(&MP.m_vRightPoint), xvPoint, XMVectorReplicate(fSearchRadius)))
			{
				return MP.m_vRightPoint;
			}
		}
	};

	ROAD_GRAPH &GetRoadGraph()
	{
		return m_RoadGraph;
	};

	ROAD_GRAPH const &GetRoadGraph() const
	{
		return m_RoadGraph;
	};
private:
	ROAD_GRAPH	m_RoadGraph;
};