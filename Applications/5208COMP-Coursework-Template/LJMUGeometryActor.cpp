#include "LJMUGeometryActor.h"
#include "../../Source/Rendering/SamplerStateConfigDX11.h" // ?? Need to add this for SamplerStateConfig to work?

void LJMUGeometryActor::UseLitTexturedMaterial(ResourcePtr texture)
{
	m_pLitTexturedMaterial->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
	GetBody()->SetMaterial(m_pLitTexturedMaterial);
}

MaterialPtr LJMUGeometryActor::createLitTexturedMaterial()
{
	RendererDX11* pRenderer = RendererDX11::Get();
	MaterialPtr material = MaterialPtr(new MaterialDX11());

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader(pRenderer->LoadShader(VERTEX_SHADER,
		std::wstring(L"LJMULitTexture.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_4_0")));

	pEffect->SetPixelShader(pRenderer->LoadShader(PIXEL_SHADER,
		std::wstring(L"LJMULitTexture.hlsl"),
		std::wstring(L"PSMAIN"),
		std::wstring(L"ps_4_0")));

	SamplerStateConfigDX11 SamplerConfig;
	SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerConfig.MaxAnisotropy = 16;

	int TextureSampler = RendererDX11::Get()->CreateSamplerState(&SamplerConfig);
	material->Parameters.SetSamplerParameter(L"TextureSampler", TextureSampler);

	// Enable the material to render the given view type, and set it's effect
	material->Params[VT_PERSPECTIVE].bRender = true;
	material->Params[VT_PERSPECTIVE].pEffect = pEffect;

	return material;
}

LJMUGeometryActor::LJMUGeometryActor()
{
	m_pLitTexturedMaterial = createLitTexturedMaterial();
	GetBody()->SetMaterial(m_pLitTexturedMaterial);
}

