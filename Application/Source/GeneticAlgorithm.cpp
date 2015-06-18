#include "PCH.h"

#include "GeneticAlgorithm.h"

size_t g_nLastID = 0;

const std::unordered_map<EDriverBehaviorType, std::pair<float, float>> g_Bounds(
{
	{ AHEAD_BRAKING_K,		{ 0.1f, 2.0f } },
	{ BRAKING_K_FROM_VEL,	{ 0.1f, 2.0f } },
	{ BRAKING_K_BORDER,		{ 0.05f, 0.2f } },
	{ BRAKING_K_RADIUS,		{ 0.0002f, 0.002f } },

	{ STEERING_K,			{ 6.0f, 14.0f } },
	{ STEERING_TO_CENTER_K, { 0.01f, 0.6f } },

	{ ACCELERATION,			{ 0.2f, 1.0f } },
	{ VELOCITY_K,			{ 0.01f, 0.1f } }
});

const std::unordered_map<EDriverBehaviorType, std::string> g_DBToString(
{
	{ AHEAD_BRAKING_K,		"AheadBrakingK" },
	{ BRAKING_K_FROM_VEL,	"BrakingKFromVel" },
	{ BRAKING_K_BORDER,		"BrakingKBorder" },
	{ BRAKING_K_RADIUS,		"BrakingKRadius" },

	{ STEERING_K,			"SteeringK" },
	{ STEERING_TO_CENTER_K, "SteeringToCenterK" },

	{ ACCELERATION,			"Acceleration" },
	{ VELOCITY_K,			"VelocityK" }
});

const std::unordered_map<std::string, EDriverBehaviorType> g_StringToDB(
{
	{ "AheadBrakingK",		AHEAD_BRAKING_K },
	{ "BrakingKFromVel",	BRAKING_K_FROM_VEL },
	{ "BrakingKBorder",		BRAKING_K_BORDER },
	{ "BrakingKRadius",		BRAKING_K_RADIUS },

	{ "SteeringK",			STEERING_K },
	{ "SteeringToCenterK",	STEERING_TO_CENTER_K },

	{ "Acceleration",		ACCELERATION },
	{ "VelocityK",			VELOCITY_K }
});

float CDrivingBehavior::CheckToBounds(EDriverBehaviorType eType, float fValue)
{
	return MathHelper::Clamp(fValue, g_Bounds.find(eType)->second.first, g_Bounds.find(eType)->second.second);
}

CDrivingBehavior::CDrivingBehavior(INI<> &ini, std::string const &sSection)
{
	std::string sTemp;

	for (size_t i = 0; i < (size_t)EDriverBehaviorType::COUNT; i++)
	{
		EDriverBehaviorType iType = (EDriverBehaviorType)i;

		if ((sTemp = ini.get(sSection, g_DBToString.find(iType)->second, INI_ERROR)) != INI_ERROR)
			m_DriverTraits[iType] = StringHelper::StringToNumber<float>(sTemp);
		else
			m_DriverTraits[iType] = MathHelper::GetRandom(g_Bounds.find(iType)->second.first, g_Bounds.find(iType)->second.second);
	}

	m_nID = g_nLastID;
	g_nLastID++;

	m_fFitness = MathHelper::GetRandom(40.0f, 60.0f);
};

CDrivingBehavior::CDrivingBehavior(size_t nID)
{
	for (size_t i = 0; i < (size_t)EDriverBehaviorType::COUNT; i++)
	{
		EDriverBehaviorType iType = (EDriverBehaviorType)i;
		m_DriverTraits[iType] = MathHelper::GetRandom(g_Bounds.find(iType)->second.first, g_Bounds.find(iType)->second.second);
	}

	m_nID = g_nLastID;
	g_nLastID++;

	m_fFitness = MathHelper::GetRandom(40.0f, 60.0f);
}

CDrivingBehavior::CDrivingBehavior()
{
	m_nID = g_nLastID;
	g_nLastID++;
}

void CDrivingBehavior::Save(INI<> &ini) const
{
	ini.create("DB" + StringHelper::NumberToString(m_nID));

	for (size_t i = 0; i < (size_t)EDriverBehaviorType::COUNT; i++)
	{
		EDriverBehaviorType iType = (EDriverBehaviorType)i;

		ini.set(g_DBToString.find(iType)->second, StringHelper::NumberToString(m_DriverTraits.find(iType)->second));
	}
}

CGeneticAlgorithm::~CGeneticAlgorithm()
{
	INI<> ini("data/DriverBehaviors.ini", true);
	ini.clear();

	for (auto const &DB : m_DrivingBehaviours)
	{
		DB.Save(ini);
	}

	ini.save();
}

void CGeneticAlgorithm::Init()
{
	{
		INI<> ini("data/DriverBehaviors.ini", true);

		for (auto &sect : ini.sections)
		{
			m_DrivingBehaviours.emplace_back(CDrivingBehavior(ini, sect.first));
		}

		for (size_t i = ini.sections.size(); i < m_nNumDrivingBehaviors; i++)
		{
			m_DrivingBehaviours.emplace_back(CDrivingBehavior(30));
		}
	}

	const std::string DriverNames[5] =
	{
		"Vanya",
		"Misha",
		"Maks",
		"Igor",
		"Snedr"
	};

	size_t nNumDrivers = SizeOfArray(DriverNames);
	if (nNumDrivers > m_nNumDrivingBehaviors)
		nNumDrivers = m_nNumDrivingBehaviors;

	for (size_t i = 0; i < nNumDrivers; i++)
	{
		CDriver Driver;

		Driver.m_fCircleTime = 1.0f;
		Driver.m_nBehaviorID = rand() % m_DrivingBehaviours.size();
		Driver.m_sDriverName = DriverNames[i % SizeOfArray(DriverNames)];

		m_Drivers.push_back(Driver);
	}
};

void CGeneticAlgorithm::RecalcFitness()
{
	float fMinCircleTime = FLT_MAX;
	for (auto const &dr : m_Drivers)
	{
		if (dr.m_fCircleTime < fMinCircleTime)
		{
			fMinCircleTime = dr.m_fCircleTime;
		}
	}

	for (auto &dr : m_Drivers)
	{
		// одинаковый фитнес для разных водителей с одинаковым поведением
		m_DrivingBehaviours[dr.m_nBehaviorID].m_fFitness = dr.m_fCircleTime - fMinCircleTime;

		m_DrivingBehaviours[dr.m_nBehaviorID].m_bIsParticipiant = true;
	}
};

float g_NumVsKoefData[] =
{
	0, 1.0f,
	3, 0.98f,
	8, 0.78f,
	15, 0.65f,
	20, 0.4f,
	50, 0.25f,
	110, 0.12f,
	200, 0.061f,
	500, 0.028f,
	800, 0.015f,
	1100, 0.011f,
	1500, 0.0095f,
	2000, 0.006f,

	1000000.0f, 0.0f,
};

CLerpTable g_NumVsKoefTable(g_NumVsKoefData, sizeof(g_NumVsKoefData) / (4 * 2));

size_t CGeneticAlgorithm::ChooseParent() const
{
	size_t nMax = m_DrivingBehaviours.size();
	size_t nChoosed = 0;
	float fKoef = g_NumVsKoefTable.GetYVal((float)nMax);

	while (nChoosed < nMax - 1)
	{
		float fChance = (float)rand() / RAND_MAX;

		if (fChance < fKoef)
		{
			break;
		}

		nChoosed++;
	}

	return nChoosed;
}

CDrivingBehavior CGeneticAlgorithm::DoChild()
{
	size_t nFirstParent = ChooseParent();
	size_t nSecondParent = ChooseParent();

	while (nSecondParent == nFirstParent)
	{
		nSecondParent = ChooseParent();
	}

	CDrivingBehavior Child;

	for (size_t t = 0; t < (size_t)EDriverBehaviorType::COUNT; t++)
	{
		Child.m_DriverTraits[(EDriverBehaviorType)(EDriverBehaviorType::AHEAD_BRAKING_K + t)] =
			(m_DrivingBehaviours[nFirstParent].m_DriverTraits[(EDriverBehaviorType)t] + m_DrivingBehaviours[nSecondParent].m_DriverTraits[(EDriverBehaviorType)t]) * 0.5f;
	}

	Child.m_fFitness = MathHelper::GetRandom(40.0f, 60.0f);
	Child.m_bIsParticipiant = false;

	return Child;
};

// вызов апдейта только после того, как каждый водитель проедет по одному кругу!!
void CGeneticAlgorithm::OnUpdate()
{
	RecalcFitness();

	// нужно отсортировать по возрастанию
	std::sort(m_DrivingBehaviours.begin(), m_DrivingBehaviours.end(),
		[](CDrivingBehavior const &d1, CDrivingBehavior const &d2)
	{
		return d1.m_fFitness < d2.m_fFitness;
	});

	size_t nNumParticipiants = 0;

	for (size_t i = 0; i < m_DrivingBehaviours.size(); i++)
	{
		if (m_DrivingBehaviours[i].m_bIsParticipiant)
		{
			nNumParticipiants++;
		}
	}

	size_t nElitSize = (size_t)(m_DrivingBehaviours.size() * m_nElitismRate);
	if (nElitSize == 0)
		nElitSize = 1;

	size_t nNumChildren = nNumParticipiants - nElitSize + rand() % nElitSize - rand() % nElitSize;
	if (nNumChildren < nElitSize + 2)
		nNumChildren = nElitSize + 2;

	// нельзя мутировать тех, кто не участвовал в гонках
	std::vector<CDrivingBehavior> Children;
	for (size_t i = 0; i < nNumChildren && Children.size() < m_nNumDrivingBehaviors; i++)
	{
		CDrivingBehavior Child = DoChild();

		Children.push_back(Child);
	}

	for (size_t i = 0; i < m_DrivingBehaviours.size(); i++)
	{
		if ((i < nElitSize || !m_DrivingBehaviours[i].m_bIsParticipiant) && Children.size() < m_nNumDrivingBehaviors)
			Children.emplace_back(m_DrivingBehaviours[i]);
	}

	m_DrivingBehaviours.clear();
	m_DrivingBehaviours = Children;

	// мутация для всех
	for (auto &DB : m_DrivingBehaviours)
	{
		if ((float)rand() / RAND_MAX < m_nMutationRate)
		{
			size_t nTrait = rand() % DB.m_DriverTraits.size();

			DB.m_DriverTraits[(EDriverBehaviorType)nTrait] = MathHelper::GetRandom(g_Bounds.find((EDriverBehaviorType)nTrait)->second.first, g_Bounds.find((EDriverBehaviorType)nTrait)->second.second);
		}
	}

	// тут нужно раздать новые поведения водителям

	for (auto &iDriver : m_Drivers)
	{
		//if (iDriver.m_nBehaviorID > m_DrivingBehaviours.size())
		{
			iDriver.m_nBehaviorID = rand() % m_DrivingBehaviours.size();
		}
	}
};