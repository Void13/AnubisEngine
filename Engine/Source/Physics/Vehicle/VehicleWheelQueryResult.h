#pragma once

#include "EnginePCH.h"
#include "../PhysXLoader.h"

//Data structure for quick setup of wheel query data structures.
class CVehicleWheelQueryResults
{
public:
	//Allocate a buffer of wheel query results for up to maxNumWheels.
	static CVehicleWheelQueryResults* allocate(const PxU32 maxNumWheels);

	//Free allocated buffer.
	void free();

	PxWheelQueryResult* addVehicle(const PxU32 numWheels);

private:

	//One result for each wheel.
	PxWheelQueryResult* mWheelQueryResults;

	//Maximum number of wheels.
	PxU32 mMaxNumWheels;

	//Number of wheels 
	PxU32 mNumWheels;


	CVehicleWheelQueryResults()
		: mWheelQueryResults(NULL), mMaxNumWheels(0), mNumWheels(0)
	{
		init();
	}

	~CVehicleWheelQueryResults()
	{
	}

	void init()
	{
		mWheelQueryResults = NULL;
		mMaxNumWheels = 0;
		mNumWheels = 0;
	}
};