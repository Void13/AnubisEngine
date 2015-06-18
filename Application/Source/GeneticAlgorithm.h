#pragma once

#include "PCH.h"

enum EDriverBehaviorType
{
	AHEAD_BRAKING_K = 0,
	BRAKING_K_FROM_VEL,
	BRAKING_K_BORDER,
	BRAKING_K_RADIUS,

	STEERING_K,
	STEERING_TO_CENTER_K,

	ACCELERATION,
	VELOCITY_K,

	COUNT
};

class CDrivingBehavior
{
public:
	CDrivingBehavior(INI<> &ini, std::string const &sSection);
	CDrivingBehavior(size_t nID);
	CDrivingBehavior();

	void Save(INI<> &ini) const;

	static float CheckToBounds(EDriverBehaviorType eType, float fValue);

	// чем меньше - тем лучше!
	float				m_fFitness = FLT_MAX;
	bool				m_bIsParticipiant = false;

	std::unordered_map<EDriverBehaviorType, float> 
						m_DriverTraits;

	size_t				m_nID;
};

class CDriver
{
public:
	std::string			m_sDriverName;
	float				m_fCircleTime;
	size_t				m_nBehaviorID;
};

class CGeneticAlgorithm
{
public:
	~CGeneticAlgorithm();

	void Init();

	void RecalcFitness();
	size_t ChooseParent() const;
	CDrivingBehavior DoChild();
	void OnUpdate();

	std::vector<CDriver>			m_Drivers;
	std::vector<CDrivingBehavior>	m_DrivingBehaviours;

	size_t m_nNumDrivingBehaviors = 10;

	const float						m_nElitismRate = 0.10f;
	const float						m_nMutationRate = 0.25f;
};