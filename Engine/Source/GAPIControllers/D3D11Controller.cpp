#include "EnginePCH.h"

#include "D3D11Controller.h"
#include "OSAPIControllers\\WindowsOSController.h"
#include "SettingsHandler.h"

CD3D11Controller::~CD3D11Controller()
{
	SAFE_RELEASE(m_pBackBufferRTV);
	SAFE_RELEASE(m_pBackBufferDSV);
	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pD3DDevice);
};

CHError CD3D11Controller::VInit()
{
	m_pSwapChainDesc.reset(new DXGI_SWAP_CHAIN_DESC);
	ZeroMemory(m_pSwapChainDesc.get(), sizeof(DXGI_SWAP_CHAIN_DESC));

	m_pSwapChainDesc->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_pSwapChainDesc->BufferDesc.RefreshRate.Numerator = 0;
	m_pSwapChainDesc->BufferDesc.RefreshRate.Denominator = 1;
	m_pSwapChainDesc->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pSwapChainDesc->BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	m_pSwapChainDesc->BufferDesc.Width = CSettingsHandler::GetInstance()->GetBackBufferWidth();
	m_pSwapChainDesc->BufferDesc.Height = CSettingsHandler::GetInstance()->GetBackBufferHeight();
	m_pSwapChainDesc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_pSwapChainDesc->SampleDesc.Count = 1;
	m_pSwapChainDesc->SampleDesc.Quality = 0;
	m_pSwapChainDesc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_pSwapChainDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	m_pSwapChainDesc->OutputWindow = CWindowsOSController::GetInstance()->GetHWND();
	m_pSwapChainDesc->Windowed = CSettingsHandler::GetInstance()->IsWindowed();
	m_pSwapChainDesc->BufferCount = 2;

	D3D_FEATURE_LEVEL FeautureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL *pFeautureLevel = nullptr;

	HRESULT hr;
	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0,
		D3D11_CREATE_DEVICE_DEBUG, FeautureLevels, 1, D3D11_SDK_VERSION,
		&m_pD3DDevice, pFeautureLevel, &m_pImmediateContext);
	if (hr != S_OK)
	{
		return new CError_DeviceCreationFailed(AT);
	}

	DWORD dwCount = -1;
	DWORD dwQuality = -1;

	for (DWORD iSampleCount = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; iSampleCount > 0; iSampleCount--)
	{
		UINT dwTempQuality = 0;

		hr = m_pD3DDevice->CheckMultisampleQualityLevels(m_pSwapChainDesc->BufferDesc.Format,
			iSampleCount, &dwTempQuality);

		if (hr != S_OK)
		{
			return new CError_DeviceCreationFailed(AT);
		}

		if (dwTempQuality > 0)
		{
			dwTempQuality--;
		}

		if (dwTempQuality > 0)
		{
			dwCount = iSampleCount;
			dwTempQuality = dwTempQuality;

			break;
		}
	}

	if (dwCount == -1 || dwQuality == -1)
	{
		dwCount = 1;
		dwQuality = 0;
	}

	m_pSwapChainDesc->SampleDesc.Count = dwCount;
	m_pSwapChainDesc->SampleDesc.Quality = dwQuality;

	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pD3DDevice);

	DWORD dwFlags = 0;

#ifdef _DEBUG
	dwFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0,
		dwFlags, FeautureLevels, 1, D3D11_SDK_VERSION,
		m_pSwapChainDesc.get(), &m_pSwapChain, &m_pD3DDevice, pFeautureLevel, &m_pImmediateContext);
	if (hr != S_OK)
	{
		return new CError_DeviceCreationFailed(AT);
	}

	IDXGIDevice1 *pDXGIDevice = nullptr;
	m_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice1), (void **)&pDXGIDevice);
	pDXGIDevice->SetMaximumFrameLatency(1);

	CEventManager::GetInstance()->RegCallback(E_RESERVED_EVENTS::RESET_DEVICE, &CD3D11Controller::VResetDevice, this);

	CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::RESET_DEVICE);

	LOG_INFO("CD3D11Controller: device has been created");

	return new CError_OK(AT);
}

void CD3D11Controller::VResetDevice()
{
	HRESULT hr;

	m_pSwapChainDesc->BufferDesc.Width = CSettingsHandler::GetInstance()->GetBackBufferWidth();
	m_pSwapChainDesc->BufferDesc.Height = CSettingsHandler::GetInstance()->GetBackBufferHeight();
	m_pSwapChainDesc->Windowed = CSettingsHandler::GetInstance()->IsWindowed();

	DXGI_MODE_DESC ModeDesc = m_pSwapChainDesc->BufferDesc;

	ModeDesc.Width = CSettingsHandler::GetInstance()->GetWindowWidth();
	ModeDesc.Height = CSettingsHandler::GetInstance()->GetWindowHeight();

	hr = m_pSwapChain->ResizeTarget(&ModeDesc);
	if (hr != S_OK)
	{
		CHError er = new CError_ResetDeviceFailed(AT);
		return;
	}

	hr = m_pSwapChain->SetFullscreenState(!m_pSwapChainDesc->Windowed, nullptr);
	if (hr != S_OK)
	{
		CHError er = new CError_ResetDeviceFailed(AT);
		return;
	}
	
	ModeDesc.RefreshRate.Numerator = 0;
	ModeDesc.RefreshRate.Denominator = 1;

	hr = m_pSwapChain->ResizeTarget(&ModeDesc);
	if (hr != S_OK)
	{
		CHError er = new CError_ResetDeviceFailed(AT);
		return;
	}

	SAFE_RELEASE(m_pBackBufferRTV);
	
	hr = m_pSwapChain->ResizeBuffers(1, 
		m_pSwapChainDesc->BufferDesc.Width,
		m_pSwapChainDesc->BufferDesc.Height,
		m_pSwapChainDesc->BufferDesc.Format,
		0);
	if (hr != S_OK)
	{
		CHError er = new CError_ResetDeviceFailed(AT);
		return;
	}


	ID3D11Texture2D *pBackBuffer = nullptr;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBuffer);
	if (hr != S_OK)
	{
		CHError er = new CError_ResetDeviceFailed(AT);
		return;
	}

	hr = m_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pBackBufferRTV);
	if (hr != S_OK)
	{
		CHError er = new CError_ResetDeviceFailed(AT);
		return;
	}

	SAFE_RELEASE(pBackBuffer);

	
	SAFE_RELEASE(m_pBackBufferDSV);
	
	ID3D11Texture2D *pDSVTexture = nullptr;

	D3D11_TEXTURE2D_DESC DSVTexDesc;
	ZeroMemory(&DSVTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
	DSVTexDesc.Width = m_pSwapChainDesc->BufferDesc.Width;
	DSVTexDesc.Height = m_pSwapChainDesc->BufferDesc.Height;
	DSVTexDesc.MipLevels = 1;
	DSVTexDesc.ArraySize = 1;
	DSVTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVTexDesc.SampleDesc.Count = 1;
	DSVTexDesc.SampleDesc.Quality = 0;
	DSVTexDesc.Usage = D3D11_USAGE_DEFAULT;
	DSVTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSVTexDesc.CPUAccessFlags = 0;
	DSVTexDesc.MiscFlags = 0;
	hr = m_pD3DDevice->CreateTexture2D(&DSVTexDesc, nullptr, &pDSVTexture);
	if (hr != S_OK)
	{
		CHError er = new CError_ResetDeviceFailed(AT);
		return;
	}

	hr = m_pD3DDevice->CreateDepthStencilView(pDSVTexture, nullptr, &m_pBackBufferDSV);
	if (hr != S_OK)
	{
		CHError er = new CError_ResetDeviceFailed(AT);
		return;
	}

	SAFE_RELEASE(pDSVTexture);

	CEventManager::GetInstance()->FastCallEvent(E_RESERVED_EVENTS::RESET_SYSTEMS);

	LOG_INFO("CD3D11Controller: device has been resetted");
}

void CD3D11Controller::VStartFrame()
{
	static ID3D11ShaderResourceView *pSRVNull[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	static ID3D11RenderTargetView *pRTVNull[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	static ID3D11DepthStencilView *pDSVNull = nullptr;
	static float ClearColor[4] = { 0.2f, 0.2f, 0.8f, 1.0f };

	// clear buffers before starting drawing smth
	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTV, ClearColor);
	m_pImmediateContext->ClearDepthStencilView(m_pBackBufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CD3D11Controller::VEndFrame()
{
	// DXGI_PRESENT_DO_NOT_SEQUENCE
	if (m_pSwapChain->Present(0, 0) != S_OK)
	{
		if (m_bIsResetRequsted)
		{
			CHError er = new CError_SwapBuffersFailed(AT);
			return;
		}
		else
		{
			LOG_WARN("CD3D11Controller: present failed!!");

			m_bIsResetRequsted = true;

			VResetDevice();
		}
	}
}