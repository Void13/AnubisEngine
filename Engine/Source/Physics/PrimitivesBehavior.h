#pragma once

#include "BasePhysicsBehavior.h"

class CPrimitivePhysics final : public IPhysicalBehavior
{
public:
	virtual ~CPrimitivePhysics()
	{
	};

	virtual void VInit(IActor *pActor) override;
};