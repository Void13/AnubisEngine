#include "EnginePCH.h"

#include "SolidPrimitiveRenderer.h"
#include "ShadersManager.h"

CTechnique				g_TechSolidRenderer;

ID3D11RasterizerState	*g_pSolidRastState = nullptr;
ID3D11BlendState		*g_pSolidBlendState = nullptr;
ID3D11DepthStencilState	*g_pSolidDSState = nullptr;

SAMPLERID				g_LinearSampler;

void CSolidRenderer::OpenRenderer()
{
	if (!g_pSolidRastState)
	{
		InitRenderer();
	}

	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	pImmediateContext->OMSetBlendState(g_pSolidBlendState, nullptr, 0xffffffff);
	pImmediateContext->OMSetDepthStencilState(g_pSolidDSState, 0);
	pImmediateContext->RSSetState(g_pSolidRastState);

	CShaders::SetSampler(g_LinearSampler, 0, SHM_PS);
	g_TechSolidRenderer.SetShaders();
}

void CSolidRenderer::CloseRenderer()
{
	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	pImmediateContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	pImmediateContext->OMSetDepthStencilState(NULL, 0);
	pImmediateContext->RSSetState(NULL);
}

void CSolidRenderer::InitRenderer()
{
	HRESULT hr;

	if (!g_pSolidRastState)
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};

		g_TechSolidRenderer.CreateInputLayout(layout, _ARRAYSIZE(layout));

		g_TechSolidRenderer.CreateShaders(L"data\\shaders\\SolidPrimitiveRenderer.hlsl",
			"Render", "Render", nullptr, SHM_VS | SHM_PS);

		D3D11_SAMPLER_DESC SampDesc;
		ZeroMemory(&SampDesc, sizeof(SampDesc));
		SampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SampDesc.MipLODBias = 0.0f;
		SampDesc.MaxAnisotropy = 16;
		SampDesc.BorderColor[0] = SampDesc.BorderColor[1] = SampDesc.BorderColor[2] = SampDesc.BorderColor[3] = 0.0f;
		SampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		SampDesc.MinLOD = 0;
		SampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		CShaders::CreateSampler(SampDesc, &g_LinearSampler);

		ID3D11Device *pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();

		D3D11_RASTERIZER_DESC RastDesc;
		ZeroMemory(&RastDesc, sizeof(D3D11_RASTERIZER_DESC));
		RastDesc.AntialiasedLineEnable = true;
		RastDesc.MultisampleEnable = true;
		RastDesc.SlopeScaledDepthBias = 2.0f;
		RastDesc.FrontCounterClockwise = false;
		RastDesc.CullMode = D3D11_CULL_NONE;
		RastDesc.FillMode = D3D11_FILL_SOLID;
		hr = pD3DDevice->CreateRasterizerState(&RastDesc, &g_pSolidRastState);
		if (hr != S_OK)
		{
			CHError er = new CError_CreatingRastStateFailed(AT);

			return;
		}

		D3D11_BLEND_DESC bsDesc;
		ZeroMemory(&bsDesc, sizeof(D3D11_BLEND_DESC));
		bsDesc.AlphaToCoverageEnable = true;
		bsDesc.IndependentBlendEnable = true;

		bsDesc.RenderTarget[0].BlendEnable = true;
		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = pD3DDevice->CreateBlendState(&bsDesc, &g_pSolidBlendState);
		if (hr != S_OK)
		{
			CHError er = new CError_CreatingBlenderStateFailed(AT);

			return;
		}

		D3D11_DEPTH_STENCIL_DESC dsdesc;
		ZeroMemory(&dsdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

		dsdesc.DepthEnable = true;
		dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsdesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsdesc.StencilEnable = false;
		dsdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		dsdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		dsdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		hr = pD3DDevice->CreateDepthStencilState(&dsdesc, &g_pSolidDSState);
		if (hr != S_OK)
		{
			CHError er = new CError_CreatingDepthStencilStateFailed(AT);

			return;
		}
	}
}

void CSolidRenderer::DestroyRenderer()
{
	g_TechSolidRenderer.Destroy();

	SAFE_RELEASE(g_pSolidRastState);
	SAFE_RELEASE(g_pSolidBlendState);
	SAFE_RELEASE(g_pSolidDSState);
}