#pragma once

#include "PCH.h"

enum class E_STATE
{
	NOT_INIT = 0,
	INIT,
	CAN_DRAW,
	CANNOT_DRAW,
	FINISH_EDITING,
};

#include "RoadGraph.h"
#include "../GeneticAlgorithm.h"

class CTrajectory
{
public:
	void Init(CRoadGraph const &RoadGraph, float fVehicleWidth);
	void OnUpdate(XMFLOAT3 const &vVehiclePos);

	std::vector<XMFLOAT3>	m_vControlPoints;
	size_t					m_nCurrentPrevControlPoint = 0;
private:
};

class CRoadEditor2D
{
public:
	CRoadEditor2D();
	~CRoadEditor2D();

	void OnInit();
	void OnUpdate();
private:
	void RoadPushOneSlice(XMVECTOR const &xvLeftRoadBorder, XMVECTOR const &xvRightRoadBorder);
	void OnMouseInput(E_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDeltaPos, XMINT2 const &_vPos);
	void OnKeysInput(E_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown);
	bool OnFinishedEditing(const CEGUI::EventArgs& args);
	
	void InitRoad();
	void ReinitVehicle();

	CEGUI::FrameWindow		*m_pButtonWindow = nullptr;

	CGeneticAlgorithm		m_GeneticAlg;
	size_t					m_nCurrentDriver = 0;
	double					m_fStartCircleTime = 0.0f;
	bool					m_bIsNewDrive = true;

	CRoadGraph				m_RoadGraph;
	IActor					*m_pRoad = nullptr;

	XMFLOAT3				m_vStartRoad = XMFLOAT3(0.0f, 10.0f, 0.0f);
	XMFLOAT3				m_vEndRoad = XMFLOAT3(40.0f, 10.0f, 0.0f);
	const int				m_nSlicesX = 10;
	float					m_fRoadWidth = 5.0f;

	XMFLOAT3				m_vPreLastMouseWorldPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3				m_vLastMouseWorldPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3				m_vCurrentMouseWorldPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMINT2					m_vMousePos;

	E_STATE					m_eState = E_STATE::NOT_INIT;

	CTrajectory				m_Trajectory;

	XMVECTOR				m_xvLastVehPos = XMVectorZero();
	double					m_fLastUpdateCalling = 0.0;
	float					m_fNeedSteerAngle = 0.0f;
	float					m_fCurrentSteerAngle = 0.0f;
};