#include "EnginePCH.h"

#include "D3D11GraphicsRenderer.h"
#include "OSAPIControllers\\WindowsOSController.h"
#include "SettingsHandler.h"
#include "MainLoopHandler.h"

#include "GAPIControllers\\D3D11Controller.h"

#include "CEGUI\\RendererModules\\Direct3D11\\Renderer.h"

#include "SolidPrimitiveRenderer.h"
#include "WireframePrimitiveRenderer.h"

#include "PrimitiveObjects.h"

#include "Camera.h"

#ifdef _DEBUG
	#pragma comment( lib, "CEGUI\\CEGUIDirect3D11Renderer-0_d.lib" )
#else
	#pragma comment( lib, "CEGUI\\CEGUIDirect3D11Renderer-0.lib" )
#endif

//CTechnique		g_TechRenderQuad;

CD3D11GraphicsRenderer::~CD3D11GraphicsRenderer()
{
	CWireframeRenderer::DestroyWireframeRenderer();
	CSolidRenderer::DestroyRenderer();

	CEGUI::Direct3D11Renderer::destroySystem();
};

CHError CD3D11GraphicsRenderer::VInit()
{
	CHError er = CDistributedObjectCreator::GetGAPIController()->VInit();
	if (!er.IsOK())
	{
		return er;
	}

	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.Width = (float)CSettingsHandler::GetInstance()->GetBackBufferWidth();
	m_Viewport.Height = (float)CSettingsHandler::GetInstance()->GetBackBufferHeight();

	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();
	pImmediateContext->RSSetViewports(1, &m_Viewport);

	CShaders::Init();

	//g_TechRenderQuad.CreateShaders(L"data\\shaders\\Renderer.hlsl", 
	//	"RENDER_QUAD", "RENDER_QUAD", nullptr, SHM_VS | SHM_PS);

	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::RESET_SYSTEMS, &CD3D11GraphicsRenderer::ResetSystem, this);

	m_pCEGUIRenderer = &CEGUI::Direct3D11Renderer::bootstrapSystem(
		CD3D11Controller::GetInstance()->GetDevice(),
		CD3D11Controller::GetInstance()->GetContext());

	m_pCEGUISystem = CEGUI::System::getSingletonPtr();

	CShaders::AddConstantBuffer(sizeof(CBPerMesh), m_cbidPerMesh);

	LOG_INFO("CD3D11GraphicsRenderer: initializated");

	return new CError_OK(AT);
}

void CD3D11GraphicsRenderer::ResetSystem()
{
	CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Sizef(
		(float)CSettingsHandler::GetInstance()->GetBackBufferWidth(),
		(float)CSettingsHandler::GetInstance()->GetBackBufferHeight()));
}

void CD3D11GraphicsRenderer::RenderWireframeObjects()
{
	CWireframeRenderer::OpenWireframeRenderer();

	auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();

	auto cbidPerMesh = GetCBIDPerMesh();
	CShaders::SetConstantBuffer(cbidPerMesh, 0, SHM_VS | SHM_GS | SHM_PS);
	CBPerMesh *const pCBMesh = (CBPerMesh *)CShaders::GetCB(cbidPerMesh)->pData;
	pCBMesh->mViewProj = XMMatrixTranspose(pCamera->GetViewProjMatrix());
	pCBMesh->fFarPlane = pCamera->GetFarPlane();

	for (auto iPair : *m_pWireframeQueue)
	{
		iPair.first->VRender(iPair.second, pCamera);
	}

	CWireframeRenderer::CloseWireframeRenderer();
}

void CD3D11GraphicsRenderer::RenderSolidObjects()
{
	CSolidRenderer::OpenRenderer();

	auto pCamera = CMainLoopHandler::GetInstance()->GetPrimaryCamera();

	auto cbidPerMesh = GetCBIDPerMesh();
	CShaders::SetConstantBuffer(cbidPerMesh, 0, SHM_VS | SHM_GS | SHM_PS);
	CBPerMesh *const pCBMesh = (CBPerMesh *)CShaders::GetCB(cbidPerMesh)->pData;
	pCBMesh->mViewProj = XMMatrixTranspose(pCamera->GetViewProjMatrix());
	pCBMesh->fFarPlane = pCamera->GetFarPlane();

	for (auto iPair : *m_pSolidQueue)
	{
		iPair.first->VRender(iPair.second, pCamera);
	}

	CSolidRenderer::CloseRenderer();
}

void CD3D11GraphicsRenderer::VOnFrameRender()
{
	CD3D11Controller::GetInstance()->VStartFrame();
	
	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();
	auto pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();

	auto pRTV = CD3D11Controller::GetInstance()->GetBBRTV();
	pImmediateContext->OMSetRenderTargets(1, &pRTV, CD3D11Controller::GetInstance()->GetBBDSV());
	pImmediateContext->RSSetViewports(1, &m_Viewport);
	pImmediateContext->RSSetState(nullptr);
	pImmediateContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	pImmediateContext->IASetInputLayout(nullptr);
	pImmediateContext->OMSetDepthStencilState(nullptr, 0);

	/*
	g_TechRenderQuad.SetShaders();
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pImmediateContext->Draw(3, 0);
	*/

	if (m_pSolidQueue)
	{
		RenderSolidObjects();
		m_pSolidQueue = nullptr;
	}

	if (m_pWireframeQueue)
	{
		RenderWireframeObjects();
		m_pWireframeQueue = nullptr;
	}

	m_pCEGUISystem->renderAllGUIContexts();

	CD3D11Controller::GetInstance()->VEndFrame();
}