#include "EnginePCH.h"

#include "WireframePrimitiveRenderer.h"
#include "ShadersManager.h"

CTechnique				g_TechWireRenderer;

ID3D11RasterizerState	*g_pWireRastState = nullptr;
ID3D11BlendState		*g_pWireBlendState = nullptr;
ID3D11DepthStencilState	*g_pWireDSState = nullptr;

void CWireframeRenderer::OpenWireframeRenderer()
{
	if (!g_pWireRastState)
	{
		InitWireframeRenderer();
	}

	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	pImmediateContext->OMSetBlendState(g_pWireBlendState, nullptr, 0xffffffff);
	pImmediateContext->OMSetDepthStencilState(g_pWireDSState, 0);
	pImmediateContext->RSSetState(g_pWireRastState);

	g_TechWireRenderer.SetShaders();
}

void CWireframeRenderer::CloseWireframeRenderer()
{
	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	pImmediateContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	pImmediateContext->OMSetDepthStencilState(NULL, 0);
	pImmediateContext->RSSetState(NULL);
}

void CWireframeRenderer::InitWireframeRenderer()
{
	HRESULT hr;

	if (!g_pWireRastState)
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

		g_TechWireRenderer.CreateInputLayout(layout, _ARRAYSIZE(layout));

		g_TechWireRenderer.CreateShaders(L"data\\shaders\\WireframePrimitiveRenderer.hlsl", 
			"WIRE_Render", "WIRE_Render", nullptr, SHM_VS | SHM_PS | SHM_GS);

		ID3D11Device *pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();

		D3D11_RASTERIZER_DESC RastDesc;
		ZeroMemory(&RastDesc, sizeof(D3D11_RASTERIZER_DESC));
		RastDesc.AntialiasedLineEnable = true;
		RastDesc.MultisampleEnable = true;
		RastDesc.SlopeScaledDepthBias = 2.0f;
		RastDesc.FrontCounterClockwise = false;
		RastDesc.CullMode = D3D11_CULL_NONE;
		RastDesc.FillMode = D3D11_FILL_SOLID;
		hr = pD3DDevice->CreateRasterizerState(&RastDesc, &g_pWireRastState);
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
		bsDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

		hr = pD3DDevice->CreateBlendState(&bsDesc, &g_pWireBlendState);
		if (hr != S_OK)
		{
			CHError er = new CError_CreatingBlenderStateFailed(AT);

			return;
		}

		D3D11_DEPTH_STENCIL_DESC dsdesc;
		ZeroMemory(&dsdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

		dsdesc.DepthEnable = true;
		// D3D11_DEPTH_WRITE_MASK_ZERO
		dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsdesc.StencilEnable = false;
		dsdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsdesc.StencilWriteMask = 0;

		dsdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		dsdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		hr = pD3DDevice->CreateDepthStencilState(&dsdesc, &g_pWireDSState);
		if (hr != S_OK)
		{
			CHError er = new CError_CreatingDepthStencilStateFailed(AT);

			return;
		}
	}
}

void CWireframeRenderer::DestroyWireframeRenderer()
{
	g_TechWireRenderer.Destroy();

	SAFE_RELEASE(g_pWireRastState);
	SAFE_RELEASE(g_pWireBlendState);
	SAFE_RELEASE(g_pWireDSState);
}