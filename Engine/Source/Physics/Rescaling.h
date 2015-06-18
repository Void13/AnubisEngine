#pragma once

#include "PhysXLoader.h"

namespace PhysXRescaling
{
	/// Scales the given shape as precisely as possible.
	void Scale(physx::PxShape &shape, const physx::PxVec3 &scaling);

	void Scale(physx::PxGeometry &geometry, const physx::PxVec3 &scaling);

	/// Scales the given shape as precisely as possible.
	void Scale(physx::PxGeometry &geometry, physx::PxTransform &location, const physx::PxVec3 &scaling);
	/// Scales the given shape as precisely as possible.
	void Scale(physx::PxBoxGeometry &geometry, physx::PxTransform &location, const physx::PxVec3 &scaling);
	/// Scales the given shape as precisely as possible.
	void Scale(physx::PxSphereGeometry &geometry, physx::PxTransform &location, const physx::PxVec3 &scaling);
	/// Scales the given shape as precisely as possible.
	void Scale(physx::PxPlaneGeometry &geometry, physx::PxTransform &location, const physx::PxVec3 &scaling);
	/// Scales the given shape as precisely as possible.
	void Scale(physx::PxCapsuleGeometry &geometry, physx::PxTransform &location, const physx::PxVec3 &scaling);
	/// Scales the given shape as precisely as possible.
	void Scale(physx::PxConvexMeshGeometry &geometry, physx::PxTransform &location, const physx::PxVec3 &scaling);
	/// Scales the given shape as precisely as possible.
	void Scale(physx::PxTriangleMeshGeometry &geometry, physx::PxTransform &location, const physx::PxVec3 &scaling);
};