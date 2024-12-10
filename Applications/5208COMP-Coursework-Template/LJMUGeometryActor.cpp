#include "LJMUGeometryActor.h"
//#include "../../Source/Rendering/SamplerStateConfigDX11.h" 

void LJMUGeometryActor::UseLitTexturedMaterial(ResourcePtr texture)
{
	m_pLitTexturedMaterial->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
	GetBody()->SetMaterial(m_pLitTexturedMaterial);
}

void LJMUGeometryActor::UseSkyboxMaterial(ResourcePtr texture)
{
	m_pSkyboxMaterial->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
	GetBody()->SetMaterial(m_pSkyboxMaterial);
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

MaterialPtr LJMUGeometryActor::createSkyboxMaterial()
{
	RendererDX11* pRenderer = RendererDX11::Get();
	MaterialPtr Material = MaterialPtr(new MaterialDX11);

	//create and fill the effect

	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader(pRenderer->LoadShader(VERTEX_SHADER,
		std::wstring(L"LJMUTextureMap.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_4_0")));


	pEffect->SetPixelShader(pRenderer->LoadShader(PIXEL_SHADER,
		std::wstring(L"LJMUTextureMap.hlsl"),
		std::wstring(L"PSMAIN"),
		std::wstring(L"ps_4_0")));


	SamplerStateConfigDX11 SamplerConfig;
	SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerConfig.MaxAnisotropy = 16;

	int TextureSampler = RendererDX11::Get()->CreateSamplerState(&SamplerConfig);
	Material->Parameters.SetSamplerParameter(L"TextureSampler", TextureSampler);

	RasterizerStateConfigDX11 rsConfig;
	rsConfig.CullMode = D3D11_CULL_NONE;
	rsConfig.FillMode = D3D11_FILL_SOLID;

	int rasterizerState = pRenderer->CreateRasterizerState(&rsConfig);
	pEffect->m_iRasterizerState = rasterizerState;

	//Enable the material to render the given view and set its effect
	Material->Params[VT_PERSPECTIVE].bRender = true;
	Material->Params[VT_PERSPECTIVE].pEffect = pEffect;

	return Material;
}


LJMUGeometryActor::LJMUGeometryActor()
{
	m_pLitTexturedMaterial = createLitTexturedMaterial();
	GetBody()->SetMaterial(m_pLitTexturedMaterial);
	m_pSkyboxMaterial = createSkyboxMaterial();
	GetBody()->SetMaterial(m_pSkyboxMaterial);
}

