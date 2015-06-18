#pragma once

#include "BasePhysicsBehavior.h"
#include "SimpleActor.h"

class CCameraPhysics final : public IPhysicalBehavior
{
public:
	virtual ~CCameraPhysics()
	{
	};

	void VInit(std::shared_ptr<IActor> pActor)
	{
		// we know, that this actor is camera
		CActor_Simple *pCamera = (CActor_Simple *)pActor.get();

		// do smth
	};
private:
};