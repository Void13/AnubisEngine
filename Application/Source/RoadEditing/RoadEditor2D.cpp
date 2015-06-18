#include "RoadEditor2D.h"

void CTrajectory::Init(CRoadGraph const &RoadGraph, float fVehicleWidth)
{
	// init trajectory

	// start point

	//m_vControlPoints.push_back(RoadGraph.GetRoadGraph()[nToSkip].m_vPos);

	CRoadPoint PrePreLastPoint = RoadGraph.GetRoadGraph()[1];
	CRoadPoint PreLastPoint = RoadGraph.GetRoadGraph()[1];
	CRoadPoint LastPoint = RoadGraph.GetRoadGraph()[1];

	int nSkip = 2;

	for (CRoadPoint const &iCP : RoadGraph.GetRoadGraph())
	{
		if (nSkip == 3)
		{
			nSkip = 0;
		}
		else
		{
			nSkip++;
			continue;
		}

		// через раз делать
		CCatmullRom spl;
		spl.SetPoints(PrePreLastPoint.m_vPos, PreLastPoint.m_vPos, LastPoint.m_vPos, iCP.m_vPos);

		CCircleBy3Points circle(PreLastPoint.m_vPos, LastPoint.m_vPos, iCP.m_vPos);

		XMVECTOR xvCP = XMLoadFloat3(&spl(0.8f));
		XMVECTOR xvLeftToRight = XMLoadFloat3(&LastPoint.m_vRightPoint) - XMLoadFloat3(&LastPoint.m_vLeftPoint);
		float fWidth = XMVectorGetX(XMVector3Length(xvLeftToRight));
		xvLeftToRight = XMVector3Normalize(xvLeftToRight);

		// больше радиус - меньше расст. от центра
		float fDistFromCenter = 8.0f * fWidth / circle.GetRadius();
		fDistFromCenter = MathHelper::Clamp(fDistFromCenter, 0.0f, fWidth * 0.5f - fVehicleWidth);

		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&circle.GetCenter()) - XMLoadFloat3(&LastPoint.m_vLeftPoint))) <
			XMVectorGetX(XMVector3Length(XMLoadFloat3(&circle.GetCenter()) - XMLoadFloat3(&LastPoint.m_vRightPoint))))
		{
			// ближе к левой, склоняем к левой
			xvCP -= xvLeftToRight * fDistFromCenter;
		}
		else
		{
			xvCP += xvLeftToRight * fDistFromCenter;
		}

		XMFLOAT3 vCP;
		XMStoreFloat3(&vCP, xvCP);

		AddStaticObject(vCP, 0x00FFFFFF, E_ACTOR_TYPE::SPHERE, E_RIGID_BODY_FLAG::STATIC);

		m_vControlPoints.push_back(vCP);

		PrePreLastPoint = PreLastPoint;
		PreLastPoint = LastPoint;
		LastPoint = iCP;
	}

	m_vControlPoints.erase(m_vControlPoints.begin());

	m_vControlPoints.push_back(RoadGraph.GetRoadGraph().back().m_vPos);

	m_nCurrentPrevControlPoint = 1;
};

void CTrajectory::OnUpdate(XMFLOAT3 const &vVehiclePos)
{
	/*
	float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vVehiclePos) - XMLoadFloat3(&m_vControlPoints[m_nCurrentPrevControlPoint])));
	if (fDist < 5.0f)
	{
		m_nCurrentPrevControlPoint++;
		m_nCurrentPrevControlPoint %= m_vControlPoints.size();
	}
	*/
	/*
	size_t nMinDistControlPoint = m_nCurrentPrevControlPoint;
	float fMinDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vVehiclePos) - XMLoadFloat3(&m_vControlPoints[nMinDistControlPoint])));

	// ищем ближайшую контрольную точку
	for (size_t i = 0; i < m_vControlPoints.size(); i++)
	{
		float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vVehiclePos) - XMLoadFloat3(&m_vControlPoints[i])));

		if (fDist < fMinDistance &&
			abs((int)(m_nCurrentPrevControlPoint - i)) < 6 &&
			i != m_nCurrentPrevControlPoint)
		{
			nMinDistControlPoint = i;
			fMinDistance = fDist;
		}
	}

	m_nCurrentPrevControlPoint = nMinDistControlPoint;
	*/
};


CRoadEditor2D::CRoadEditor2D()
{
};

CRoadEditor2D::~CRoadEditor2D()
{
}

CEGUI::FrameWindow	*g_pInfoWindow12 = nullptr;
CEGUI::FrameWindow	*g_pInfoWindow22 = nullptr;

void CRoadEditor2D::OnInit()
{
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_KEYS_INPUT, &CRoadEditor2D::OnKeysInput, this);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_MOUSE_INPUT, &CRoadEditor2D::OnMouseInput, this);
	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::ON_UPDATE, &CRoadEditor2D::OnUpdate, this);

	CInputManager::GetInstance()->LinkEventToKey(E_INPUT_EVENTS::CHANGE_PROJECTION, { { E_CHANNELS::MOUSE_RIGHT } });
	CInputManager::GetInstance()->LinkEventToKey(E_INPUT_EVENTS::DRAW_ROAD, { { E_CHANNELS::MOUSE_LEFT } });

	m_eState = E_STATE::INIT;

	auto pGuiContext = &CEGUI::System::getSingletonPtr()->getDefaultGUIContext();
	auto root = pGuiContext->getRootWindow();
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

	CEGUI::FrameWindow* wnd = (CEGUI::FrameWindow*)winMgr.createWindow("AlfiskoSkin/Button", "Finish!");
	root->addChild(wnd);

	wnd->setVisible(true);

	wnd->setText("Start editing");
	wnd->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CRoadEditor2D::OnFinishedEditing, this));
	wnd->setPosition(CEGUI::UVector2(cegui_reldim(0.03f), cegui_reldim(0.91f)));
	wnd->setSize(CEGUI::USize(cegui_reldim(0.15f), cegui_reldim(0.05f)));
	m_pButtonWindow = wnd;

	g_pInfoWindow12 = (CEGUI::FrameWindow*)winMgr.createWindow("AlfiskoSkin/Label", "InfoWindow22");
	root->addChild(g_pInfoWindow12);
	g_pInfoWindow12->setAlwaysOnTop(true);
	g_pInfoWindow12->setAlpha(0.8f);
	//g_pInfoWindow12->setProperty("BackgroundEnabled", "true");
	g_pInfoWindow12->setPosition(CEGUI::UVector2(cegui_reldim(0.75f), cegui_reldim(0.10f)));
	g_pInfoWindow12->setSize(CEGUI::USize(cegui_reldim(0.25f), cegui_reldim(0.20f)));

	g_pInfoWindow22 = (CEGUI::FrameWindow*)winMgr.createWindow("AlfiskoSkin/Label", "InfoWindow32");
	root->addChild(g_pInfoWindow22);
	g_pInfoWindow22->setAlwaysOnTop(true);
	g_pInfoWindow22->setAlpha(0.8f);
	g_pInfoWindow22->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim(0.10f)));
	g_pInfoWindow22->setSize(CEGUI::USize(cegui_reldim(0.25f), cegui_reldim(0.99f)));

	if (IsFileExists("data\\Road1.trc"))
	{
		m_RoadGraph.LoadGraph("data\\Road1.trc");

		InitRoad();
	}

	m_Trajectory.Init(m_RoadGraph, 3.0f);

	m_xvLastVehPos = XMLoadFloat3(&GetVehicleActor()->GetPosition());

	m_GeneticAlg.Init();

	if (IsFileExists("data\\Road1.trc"))
	{
		ReinitVehicle();
	}

	m_nCurrentDriver = 0;
}

void CRoadEditor2D::ReinitVehicle()
{
	m_Trajectory.m_nCurrentPrevControlPoint = 1;

	XMFLOAT3 vPos = m_RoadGraph.GetRoadGraph()[3].m_vPos;
	vPos.y += 10.0f;
	GetVehicleActor()->SetPosition(vPos);
	GetVehicleActor()->SetRotation(0.0f, XMConvertToRadians(0.0f), 0.0f);
	GetVehicleActor()->GetVehicle()->mDriveDynData.setToRestState();
	GetVehicleActor()->GetVehicle()->getRigidDynamicActor()->setLinearVelocity(PxVec3(PxZERO()));
	GetVehicleActor()->GetVehicle()->getRigidDynamicActor()->setAngularVelocity(PxVec3(PxZERO()));

	m_xvLastVehPos = XMLoadFloat3(&GetVehicleActor()->GetPosition());

	m_bIsNewDrive = true;
}

void CRoadEditor2D::OnUpdate()
{
	if (m_eState == E_STATE::CAN_DRAW || m_eState == E_STATE::CANNOT_DRAW)
	{
		auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();
		pCamera->SetFreeze(true);
	}

	if (m_bIsNewDrive)
	{
		m_fStartCircleTime = CMainLoopHandler::GetInstance()->GetTime();

		m_bIsNewDrive = false;
	}

	// пересчёт текущего угла поворота
	// 
	m_fCurrentSteerAngle += (m_fNeedSteerAngle - m_fCurrentSteerAngle) * (float)CMainLoopHandler::GetInstance()->GetLogicElapsedTime() * 0.01f;
	m_fCurrentSteerAngle = MathHelper::Clamp(m_fCurrentSteerAngle, -1.0f, 1.0f);
	// если разница между нужным и текущим маленькая - делаем текущий как нужный.
	if (fabs(m_fCurrentSteerAngle - m_fNeedSteerAngle) < 0.1f)
	{
		m_fCurrentSteerAngle = m_fNeedSteerAngle;
	}

	GetVehicleActor()->SetSteering(m_fCurrentSteerAngle);

	// нужно раз в н секунд делать
	if (CMainLoopHandler::GetInstance()->GetTime() < m_fLastUpdateCalling + 0.005)
		return;

	m_fLastUpdateCalling = CMainLoopHandler::GetInstance()->GetTime();
	
	XMFLOAT3 vPrePreLastCP = m_Trajectory.m_vControlPoints[MathHelper::Clamp((int)m_Trajectory.m_nCurrentPrevControlPoint - 2, 0, (int)m_Trajectory.m_vControlPoints.size() - 1)];
	XMFLOAT3 vPreLastCP = m_Trajectory.m_vControlPoints[MathHelper::Clamp((int)m_Trajectory.m_nCurrentPrevControlPoint - 1, 0, (int)m_Trajectory.m_vControlPoints.size() - 1)];
	XMFLOAT3 vLastCP = m_Trajectory.m_vControlPoints[MathHelper::Clamp((int)m_Trajectory.m_nCurrentPrevControlPoint - 0, 0, (int)m_Trajectory.m_vControlPoints.size() - 1)];
	XMFLOAT3 vNextCP = m_Trajectory.m_vControlPoints[MathHelper::Clamp((int)m_Trajectory.m_nCurrentPrevControlPoint + 1, 0, (int)m_Trajectory.m_vControlPoints.size() - 1)];
	/*
	const float fAheadBrakingK = 1.0f;
	const float fBrakingKFromVel = 0.5f;
	const float fBrakingKBorder = 0.1f;
	const float fBrakingKRadius = 0.001f;

	const float fSteeringK = 10.0f;
	const float fSteeringToCenterK = 0.1f;

	const float fAccel = 0.5f;
	const float fVelocityK = 0.06f;
	*/

	const float fAheadBrakingK =		m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_DriverTraits[EDriverBehaviorType::AHEAD_BRAKING_K];
	const float fBrakingKFromVel =		m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_DriverTraits[EDriverBehaviorType::BRAKING_K_FROM_VEL];
	const float fBrakingKBorder =		m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_DriverTraits[EDriverBehaviorType::BRAKING_K_BORDER];
	const float fBrakingKRadius =		m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_DriverTraits[EDriverBehaviorType::BRAKING_K_RADIUS];

	const float fSteeringK =			m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_DriverTraits[EDriverBehaviorType::STEERING_K];
	const float fSteeringToCenterK =	m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_DriverTraits[EDriverBehaviorType::STEERING_TO_CENTER_K];

	const float fAccel =				m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_DriverTraits[EDriverBehaviorType::ACCELERATION];
	const float fVelocityK =			m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_DriverTraits[EDriverBehaviorType::VELOCITY_K];

	// время логики в секундах. обычно где-то 0.06
	float fVelocity = XMVectorGetX(XMVector3Length(m_xvLastVehPos - XMLoadFloat3(&GetVehicleActor()->GetPosition()))) * (float)CMainLoopHandler::GetInstance()->GetLogicElapsedTime() * fVelocityK;
	m_xvLastVehPos = XMLoadFloat3(&GetVehicleActor()->GetPosition());

	// точка на прямой
	XMFLOAT3 vProjPoint = GetClosestPointLine(&vLastCP, &vPreLastCP, &GetVehicleActor()->GetPosition());

	// теперь нужно узнать расстояние от прошлой точки до позиции
	float fDistPreLastToProj = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vProjPoint) - XMLoadFloat3(&vPreLastCP)));

	CCatmullRom PosSpline;
	PosSpline.SetPoints(vPrePreLastCP, vPreLastCP, vLastCP, vNextCP);

	// dist from prev CP to curCP
	XMVECTOR xvPrevCPToCP = XMLoadFloat3(&vLastCP) - XMLoadFloat3(&vPreLastCP);
	float fDistPrevCPToCP = XMVectorGetX(XMVector3Length(xvPrevCPToCP));

	float fSplineKoef = fDistPreLastToProj / fDistPrevCPToCP;

	if (fSplineKoef >= 0.98f)
		m_Trajectory.m_nCurrentPrevControlPoint++;

	// собираем сумму левых и правых поворотов
	// на расстоянии fDistCheck
	float fDistCheck = 50.0f * fVelocity;

	float fAccToSave = fDistCheck;
	{
		GetVehicleActor()->SetAccel(fAccel);
	}

	float fBraking = 0.0f;

	{
		// чем больше скорость - тем дальше надо заглядывать
		// fVelocity примерно в 0.3
		CCircleBy3Points circle(PosSpline(0.2f + fVelocity * fAheadBrakingK), PosSpline(0.4f + fVelocity * fAheadBrakingK), PosSpline(0.6f + fVelocity * fAheadBrakingK));

		// чем больше радиус - тем не нужнее тормоз. но если скорость высокая - то нужен.
		float fAcceleration = circle.GetRadius() * fBrakingKRadius / fVelocity;

		if (fAcceleration < fBrakingKBorder && fVelocity > 0.2f)
		{
			// чем меньше fAccToSave, тем сильнее нужно тормозить
			// fAccToSave = 0.1, fBraking = 0.1
			// fAccToSave = 0.001, fBraking = 1.0
			// fBraking = -9 * fAccToSave + 1
			fBraking = -9.5f * fAcceleration + 1.0f;
			fBraking *= fVelocity * fBrakingKFromVel;
			fBraking = MathHelper::Clamp(fBraking, 0.0f, 1.0f);
		}

		GetVehicleActor()->SetBraking(fBraking);
	}

	XMVECTOR xvVehDir = XMLoadFloat3(&GetVehicleActor()->GetObjectByName("Wheel0")->GetPosition()) - XMLoadFloat3(&GetVehicleActor()->GetObjectByName("Wheel2")->GetPosition());
	xvVehDir = XMVector3Normalize(xvVehDir);

	float fVehAngle = (atan2(xvVehDir.vector4_f32[2], xvVehDir.vector4_f32[0]) + XM_PI) / (2 * XM_PI);

	XMVECTOR xvFirstSplPoint = XMLoadFloat3(&PosSpline(fSplineKoef));
	XMVECTOR xvSecondSplPoint = XMLoadFloat3(&PosSpline(fSplineKoef + 0.08f));
	XMVECTOR xvFirstToSecond = xvSecondSplPoint - xvFirstSplPoint;
	xvFirstToSecond = XMVector3Normalize(xvFirstToSecond);

	// угол траектории. а тректория задаётся сплайном
	float fTrajAngle = (atan2(xvFirstToSecond.vector4_f32[2], xvFirstToSecond.vector4_f32[0]) + XM_PI) / (2 * XM_PI);

	// 0 to pi, -pi to 0

	// это угол между колёсами и тректорией
	float fSteerAngle = (fVehAngle - fTrajAngle) * fSteeringK;

	// и ещё нужно добавить стремление к центру траектории. чем дальше от траектории - тем хуже
	// расстояние от машины до траектории
	XMVECTOR xvVehToTraj = xvFirstSplPoint - XMLoadFloat3(&GetVehicleActor()->GetPosition());
	xvVehToTraj = XMVectorSetY(xvVehToTraj, 0.0f);
	float fDistVehToTraj = XMVectorGetX(XMVector3Length(xvVehToTraj));

	// если слева - то минус 1, справа - 1
	float fRightOrLeftToTraj = 1.0f;

	XMVECTOR xvRoadNormal = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), xvFirstSplPoint - xvSecondSplPoint);
	xvRoadNormal = XMVector3Normalize(xvRoadNormal);

	XMVECTOR xvRightPlane = XMPlaneFromPointNormal(xvFirstSplPoint, xvRoadNormal);
	float fCircleCenterToRightPlaneDist = XMVectorGetX(XMPlaneDotCoord(xvRightPlane, XMLoadFloat3(&GetVehicleActor()->GetPosition())));
	if (fCircleCenterToRightPlaneDist >= 0)
		fRightOrLeftToTraj = -1.0f;

	float fToCenterKoef = fDistVehToTraj * fRightOrLeftToTraj * fSteeringToCenterK;

	fSteerAngle = fSteerAngle - fToCenterKoef;
	m_fNeedSteerAngle = fSteerAngle;

	char fps_textbuff[256];
	sprintf(fps_textbuff, "CircleTime: %0.3f\nDriver: DB%d", 
		(CMainLoopHandler::GetInstance()->GetTime() - m_fStartCircleTime) * 0.001f, 
		m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_nID);
	//sprintf(fps_textbuff, "Accel: %0.3f\nBrake: %0.3f\nVelocity: %0.3f", fAccToSave, fBraking, fVelocity);
	//sprintf(fps_textbuff, "CurSteer: %0.3f\nNeedSteer: %0.3f\nMinus: %0.3f", m_fCurrentSteerAngle, m_fNeedSteerAngle, m_fNeedSteerAngle - m_fCurrentSteerAngle);

	g_pInfoWindow12->setText(fps_textbuff);

	static float fBestLoop = 999.0f;
	static size_t nNumLoops = 0;
	static size_t nNumFailedLoops = 0;

	XMFLOAT3 const &vLastPoint = m_RoadGraph.GetRoadGraph().back().m_vPos;

	if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&GetVehicleActor()->GetPosition()) - XMLoadFloat3(&vLastPoint))) < 5.0f ||
		GetVehicleActor()->GetPosition().y < 0.0f || 
		(CMainLoopHandler::GetInstance()->GetTime() - m_fStartCircleTime) * 0.001f > 60.0f)
	{
		// FINISH!
		// нужно всё обнулить, сделать эволюцию и телепортировать машину в начало

		// если упал - то макс. если нет - то реальное время
		if (GetVehicleActor()->GetPosition().y < 0.0f ||
			(CMainLoopHandler::GetInstance()->GetTime() - m_fStartCircleTime) * 0.001f > 60.0f)
		{
			m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_fCircleTime = FLT_MAX;
			nNumFailedLoops++;
		}
		else
			m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_fCircleTime = (float)(CMainLoopHandler::GetInstance()->GetTime() - m_fStartCircleTime) * 0.001f;
		
		if (m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_fCircleTime < fBestLoop)
			fBestLoop = m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_fCircleTime;

		//m_GeneticAlg.m_DrivingBehaviours[m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_nBehaviorID].m_fFitness = m_GeneticAlg.m_Drivers[m_nCurrentDriver].m_fCircleTime;

		m_nCurrentDriver++;

		ReinitVehicle();

		if (m_nCurrentDriver >= m_GeneticAlg.m_Drivers.size())
		{
			m_nCurrentDriver = 0;
			m_GeneticAlg.OnUpdate();
		}

		nNumLoops++;
	}

	char InfoWindow2Text[2048];
	sprintf(InfoWindow2Text, "Drivers Table:\n");

	for (auto const &DB : m_GeneticAlg.m_Drivers)
	{
		sprintf(InfoWindow2Text, "%s%s: %0.3f\n", InfoWindow2Text, DB.m_sDriverName.c_str(), 
			(DB.m_fCircleTime > 999.0f) ? 999.999f : DB.m_fCircleTime);
	}

	sprintf(InfoWindow2Text, "%sBest Loop: %0.3f\n\\[%02d/%02d]", InfoWindow2Text, fBestLoop, nNumFailedLoops, nNumLoops);

	g_pInfoWindow22->setText(InfoWindow2Text);
}

void CRoadEditor2D::OnKeysInput(E_INPUT_EVENTS const _eInputEvent, bool const _bKeyDown)
{
	static bool bOrth = false;

	switch (_eInputEvent.event)
	{
	case E_INPUT_EVENTS::CHANGE_PROJECTION:
		if (_bKeyDown)
		{
			auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();

			if (bOrth)
			{
				pCamera->SetOrthogonalMatrix(1000.0f, 1000.0f, 1.0f, 100000.0f);
				//pCamera->SetPosition(XMFLOAT3(1.0f, 100.0f, 0.0f));
				//pCamera->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));
			}
			else
			{
				float fAspectRatio = (float)CSettingsHandler::GetInstance()->GetBackBufferWidth() / (float)CSettingsHandler::GetInstance()->GetBackBufferHeight();
				pCamera->SetPerspectiveMatrix(pCamera->GetFOV(), fAspectRatio, 1.0f, pCamera->GetFarPlane());
			}

			bOrth = !bOrth;
		}
		break;

	case E_INPUT_EVENTS::DRAW_ROAD:
		if (m_eState == E_STATE::CANNOT_DRAW && _bKeyDown)
		{
			m_eState = E_STATE::CAN_DRAW;
		}
		else if (m_eState == E_STATE::CAN_DRAW && !_bKeyDown)
		{
			m_eState = E_STATE::CANNOT_DRAW;
		}

		break;
	};
}

void CRoadEditor2D::RoadPushOneSlice(XMVECTOR const &xvLeftRoadBorder, XMVECTOR const &xvRightRoadBorder)
{
	m_RoadGraph.AddMainPoint(xvLeftRoadBorder, xvRightRoadBorder);
}

void CRoadEditor2D::InitRoad()
{
	CRawModel *pRoadModel = nullptr;
	CRawMesh *pRoadMesh = nullptr;

	pRoadModel = new CRawModel;
	pRoadModel->m_eRidigBodyFlag = E_RIGID_BODY_FLAG::STATIC;
	pRoadModel->m_fLODDistance = 200.0f;
	pRoadModel->m_sModelName = "RoadObject";

	pRoadMesh = new CRawMesh(pRoadModel, E_MESH_TYPE::TRIANGLEMESH, E_MESH_TOPOLOGY::TRIANGLELIST, E_RIGID_BODY_FLAG::STATIC);
	pRoadModel->m_RawMeshes.push_back(pRoadMesh);

	int nSlicesZ = 0;

	// for all main points
	for (auto MainPoint : m_RoadGraph.GetRoadGraph())
	{
		XMVECTOR xvLeftToRight =
			(XMLoadFloat3(&MainPoint.m_vRightPoint) - XMLoadFloat3(&MainPoint.m_vLeftPoint)) / ((float)m_nSlicesX - 3);

		int nTextureBlockLength = 20;

		{
			CVertex Vertex;
			//Vertex.vPosition = MainPoint.m_vLeftPoint;
			//Vertex.vPosition.y += 3.0f;
			XMStoreFloat3(&Vertex.vPosition, XMLoadFloat3(&MainPoint.m_vLeftPoint) - xvLeftToRight * 5.0f);
			Vertex.vTexCoords.x = (float)(nSlicesZ % nTextureBlockLength) / nTextureBlockLength;
			Vertex.vTexCoords.y = (1.0f);
			XMStoreFloat3(&Vertex.vNormal, XMVector3Normalize(xvLeftToRight));

			pRoadMesh->m_Vertices.push_back(Vertex);
		}

		for (int j = 0; j < m_nSlicesX - 2; j++)
		{
			CVertex Vertex;

			XMStoreFloat3(&Vertex.vPosition, XMLoadFloat3(&MainPoint.m_vLeftPoint) + xvLeftToRight * (float)j);

			Vertex.vTexCoords.x = (float)(nSlicesZ % nTextureBlockLength) / nTextureBlockLength;
			Vertex.vTexCoords.y = (1.0f - (float)j / (m_nSlicesX - 3));

			Vertex.vNormal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			pRoadMesh->m_Vertices.push_back(Vertex);
		}

		{
			CVertex Vertex;
			//Vertex.vPosition = MainPoint.m_vRightPoint;
			//Vertex.vPosition.y += 3.0f;
			XMStoreFloat3(&Vertex.vPosition, XMLoadFloat3(&MainPoint.m_vRightPoint) + xvLeftToRight * 5.0f);
			Vertex.vTexCoords.x = (float)(nSlicesZ % nTextureBlockLength) / nTextureBlockLength;
			Vertex.vTexCoords.y = (1.0f);
			XMStoreFloat3(&Vertex.vNormal, XMVector3Normalize(-xvLeftToRight));

			pRoadMesh->m_Vertices.push_back(Vertex);
		}

		nSlicesZ++;
	}

	for (int i = 0; i < nSlicesZ - 1; i++)
	{
		for (int j = 0; j < m_nSlicesX - 1; j++)
		{
			pRoadMesh->m_Indices.push_back((m_nSlicesX) * (i + 0) + (j + 0));
			pRoadMesh->m_Indices.push_back((m_nSlicesX) * (i + 1) + (j + 0));
			pRoadMesh->m_Indices.push_back((m_nSlicesX) * (i + 0) + (j + 1));
			pRoadMesh->m_Indices.push_back((m_nSlicesX) * (i + 0) + (j + 1));
			pRoadMesh->m_Indices.push_back((m_nSlicesX) * (i + 1) + (j + 0));
			pRoadMesh->m_Indices.push_back((m_nSlicesX) * (i + 1) + (j + 1));
		}
	}

	IBaseModelRenderer *pActorSimpleRenderer = CDistributedObjectCreator::GetInstance()->CreateModelRenderer();
	pActorSimpleRenderer->SetRenderMethod(E_RENDERMETHOD::WIREFRAMED);
	pActorSimpleRenderer->SetRigidBodyFlag(pRoadModel->m_eRidigBodyFlag);
	pActorSimpleRenderer->SetLODDistance(pRoadModel->m_fLODDistance);
	//m_pRoadModel->m_Materials

	pActorSimpleRenderer->VAddMesh(pRoadMesh, pRoadMesh);

	SAFE_DELETE(pRoadModel);

	auto pActor = new CActor_Simple(pActorSimpleRenderer);
	pActor->VInit();

	AddActor(pActor);
	GetActorsStatic().push_back(pActor);
}

bool CRoadEditor2D::OnFinishedEditing(const CEGUI::EventArgs& args)
{
	if (m_eState == E_STATE::INIT)
	{
		m_RoadGraph.GetRoadGraph().clear();

		m_vStartRoad = GetVehicleActor()->GetPosition();
		m_vStartRoad.y = 0.0f;
		m_vEndRoad = m_vStartRoad;
		m_vEndRoad.x += 200.0f;

		auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();
		XMFLOAT3 vCameraPos = m_vStartRoad;
		vCameraPos.y += 300.0f;
		vCameraPos.z += 1.0f;
		vCameraPos.x += 100.0f;
		pCamera->SetPosition(vCameraPos);
		vCameraPos.y = 0.0f;
		pCamera->SetLookAt(vCameraPos);

		// start editing
		AddStaticObject(m_vStartRoad, 0x00FFFFFF, E_ACTOR_TYPE::SPHERE, E_RIGID_BODY_FLAG::STATIC);
		AddStaticObject(m_vEndRoad, 0xB7102FFF, E_ACTOR_TYPE::SPHERE, E_RIGID_BODY_FLAG::STATIC);

		// направляем дорогу ёпта
		XMVECTOR xvStartToEnd = XMLoadFloat3(&m_vEndRoad) - XMLoadFloat3(&m_vStartRoad);
		xvStartToEnd = XMVector3Normalize(xvStartToEnd);

		XMStoreFloat3(&m_vPreLastMouseWorldPos, XMLoadFloat3(&m_vStartRoad) - xvStartToEnd * 2.0f);
		XMStoreFloat3(&m_vLastMouseWorldPos, XMLoadFloat3(&m_vStartRoad) - xvStartToEnd * 1.0f);

		XMVECTOR xvRightNormal = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 
			XMLoadFloat3(&m_vPreLastMouseWorldPos) - XMLoadFloat3(&m_vLastMouseWorldPos));
		xvRightNormal = XMVector3Normalize(xvRightNormal);

		{
			XMVECTOR xvRightRoadBorder = XMLoadFloat3(&m_vPreLastMouseWorldPos) + xvRightNormal * m_fRoadWidth;
			XMVECTOR xvLeftRoadBorder = XMLoadFloat3(&m_vPreLastMouseWorldPos) - xvRightNormal * m_fRoadWidth;

			RoadPushOneSlice(xvRightRoadBorder, xvLeftRoadBorder);
		}

		{
			XMVECTOR xvRightRoadBorder = XMLoadFloat3(&m_vLastMouseWorldPos) + xvRightNormal * m_fRoadWidth;
			XMVECTOR xvLeftRoadBorder = XMLoadFloat3(&m_vLastMouseWorldPos) - xvRightNormal * m_fRoadWidth;

			RoadPushOneSlice(xvRightRoadBorder, xvLeftRoadBorder);
		}

		{
			XMVECTOR xvRightRoadBorder = XMLoadFloat3(&m_vStartRoad) + xvRightNormal * m_fRoadWidth;
			XMVECTOR xvLeftRoadBorder = XMLoadFloat3(&m_vStartRoad) - xvRightNormal * m_fRoadWidth;

			RoadPushOneSlice(xvRightRoadBorder, xvLeftRoadBorder);
		}

		AddStaticObject(m_vLastMouseWorldPos, 0x00FFFFFF, E_ACTOR_TYPE::SPHERE, E_RIGID_BODY_FLAG::STATIC);
		AddStaticObject(m_vPreLastMouseWorldPos, 0x00FFFFFF, E_ACTOR_TYPE::SPHERE, E_RIGID_BODY_FLAG::STATIC);

		m_pButtonWindow->setText("Finish editing");

		m_eState = E_STATE::CANNOT_DRAW;
	}
	// finished editing
	else if (m_eState == E_STATE::CAN_DRAW || m_eState == E_STATE::CANNOT_DRAW)
	{
		{
			XMVECTOR xvRightNormal = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
				XMLoadFloat3(&m_vLastMouseWorldPos) - XMLoadFloat3(&m_vEndRoad));
			xvRightNormal = XMVector3Normalize(xvRightNormal);

			XMVECTOR xvRightRoadBorder = XMLoadFloat3(&m_vEndRoad) + xvRightNormal * m_fRoadWidth;
			XMVECTOR xvLeftRoadBorder = XMLoadFloat3(&m_vEndRoad) - xvRightNormal * m_fRoadWidth;

			RoadPushOneSlice(xvRightRoadBorder, xvLeftRoadBorder);
		}

		InitRoad();

		CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
		winMgr.destroyWindow(m_pButtonWindow);

		m_RoadGraph.SaveGraph("data\\Road1.trc");

		m_eState = E_STATE::FINISH_EDITING;
	}

	return true;
}


void CRoadEditor2D::OnMouseInput(E_INPUT_EVENTS const _eInputEvent, XMINT3 const &_vDeltaPos, XMINT2 const &_vPos)
{
	m_vMousePos = _vPos;

	if (m_eState == E_STATE::CAN_DRAW)
	{
		{
			XMVECTOR xvRayDir;
			XMVECTOR xvRayOrigin;
			ScreenToWorld(CMainLoopHandler::GetInstance()->GetPrimaryCamera(), m_vMousePos, xvRayDir, xvRayOrigin);

			// do projection to plane
			XMVECTORF32 xvPlanePoint = { 0, m_vStartRoad.y, 0, 0 };
			XMVECTORF32 xvPlaneNormal = { 0, 1, 0, 0 };
			XMVECTOR xvPlane = XMPlaneFromPointNormal(xvPlanePoint, xvPlaneNormal);

			XMVECTOR xvResultPoint = XMPlaneIntersectLine(xvPlane, xvRayOrigin, xvRayOrigin + xvRayDir);
			XMStoreFloat3(&m_vCurrentMouseWorldPos, xvResultPoint);
		}
		XMVECTOR xvCurPos = XMLoadFloat3(&m_vCurrentMouseWorldPos);

		CCircleBy3Points circle(m_vPreLastMouseWorldPos, m_vLastMouseWorldPos, m_vCurrentMouseWorldPos);

		XMVECTOR xvCircleCenter = XMLoadFloat3(&circle.GetCenter());

		float fBorder = circle.GetRadius() * 0.2f;
		fBorder = MathHelper::Clamp(fBorder, 0.8f, 3.0f);

		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vLastMouseWorldPos) - xvCurPos)) > fBorder)
		{
			XMVECTOR xvCurPosToCenter = xvCircleCenter - xvCurPos;
			xvCurPosToCenter = XMVector3Normalize(xvCurPosToCenter);

			XMVECTOR xvRightRoadBorder = xvCurPos + xvCurPosToCenter * m_fRoadWidth;
			XMVECTOR xvLeftRoadBorder = xvCurPos - xvCurPosToCenter * m_fRoadWidth;

			{
				// vector up
				XMVECTOR xvRoadNormal = XMVector3Cross(xvCurPos - XMLoadFloat3(&m_vPreLastMouseWorldPos), 
					xvCurPos - XMLoadFloat3(&m_vLastMouseWorldPos));
				xvRoadNormal = XMVectorSetY(xvRoadNormal, 1.0f);
				xvRoadNormal = XMVector3Normalize(xvRoadNormal);
				// vector right
				xvRoadNormal = XMVector3Cross(xvRoadNormal, xvCurPos - XMLoadFloat3(&m_vPreLastMouseWorldPos));
				xvRoadNormal = XMVector3Normalize(xvRoadNormal);

				XMVECTOR xvRightPlane = XMPlaneFromPointNormal(xvCurPos, xvRoadNormal);
				float fCircleCenterToRightPlaneDist = XMVectorGetX(XMPlaneDotCoord(xvRightPlane, xvRightRoadBorder));
				if (fCircleCenterToRightPlaneDist < 0)
				{
					std::swap(xvRightRoadBorder, xvLeftRoadBorder);
				}
			}

			AddStaticObject(xvLeftRoadBorder, 0xdb00c2FF, E_ACTOR_TYPE::SPHERE, E_RIGID_BODY_FLAG::STATIC); // pink
			AddStaticObject(xvRightRoadBorder, 0x0006dbFF, E_ACTOR_TYPE::SPHERE, E_RIGID_BODY_FLAG::STATIC); // dark blue

			//AddObjectAt(xvCircleCenter, 0x17db00FF); // green
			//AddObjectAt(xvCurPos, 0xdbda00FF); // yellow

			RoadPushOneSlice(xvLeftRoadBorder, xvRightRoadBorder);

			m_vPreLastMouseWorldPos = m_vLastMouseWorldPos;
			m_vLastMouseWorldPos = m_vCurrentMouseWorldPos;
		}
	}
}