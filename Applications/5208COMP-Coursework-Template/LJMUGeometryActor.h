#pragma once
#include "Application.h"
#include <GeometryActor.h>
#include <SamplerStateConfigDX11.h>
#include <RasterizerStateConfigDX11.h>

using namespace Glyph3;

class LJMUGeometryActor : public GeometryActor
{
public:
	// CONSTRUCTORS --------------
	LJMUGeometryActor(); // Standard Empty Constructor which builds the object
	void UseLitTexturedMaterial(ResourcePtr texture);
	void UseSkyboxMaterial(ResourcePtr texture);
	MaterialPtr m_pLitTexturedMaterial;
	MaterialPtr m_pSkyboxMaterial;

protected:
	MaterialPtr LJMUGeometryActor::createLitTexturedMaterial();
	MaterialPtr LJMUGeometryActor::createSkyboxMaterial();

};

