#pragma once
#include "Application.h"
#include <GeometryActor.h>

using namespace Glyph3;

class LJMUGeometryActor : public GeometryActor
{
public:
	// CONSTRUCTORS --------------
	LJMUGeometryActor(); // Standard Empty Constructor which builds the object
	void UseLitTexturedMaterial(ResourcePtr texture);
	MaterialPtr m_pLitTexturedMaterial;

protected:
	MaterialPtr LJMUGeometryActor::createLitTexturedMaterial();

};

