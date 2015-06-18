#pragma once

#include "PhysXLoader.h"
#include "GraphicsRenderers\\BaseMeshRenderer.h"

namespace ShapeCreator
{
	PxShape *CreateShape(CRawMesh const &RawMesh);
};