#pragma once

#include "EnginePCH.h"
#include "BaseActor.h"
#include "Object.h"
#include "Physics//PrimitivesBehavior.h"

class CActor_Simple : public IActor
{
public:
	CActor_Simple(std::string const &sActorFile, std::string const &sModelFile = "") :
		IActor(sActorFile, sModelFile)
	{
	};

	CActor_Simple(IBaseModelRenderer *pModelRenderer) :
		IActor(E_ACTOR_TYPE::SIMPLE, pModelRenderer)
	{
	};

	IPhysicalBehavior *VGetPhysicsBehavior() override
	{
		if (!m_pPhysicsBehavior)
		{
			m_pPhysicsBehavior = new CPrimitivePhysics();
		}

		return m_pPhysicsBehavior;
	};

	virtual ~CActor_Simple()
	{
	};
};