#ifndef H__D3D11CONTROLLER
#define H__D3D11CONTROLLER

#include "BaseGAPIController.h"

#include <d3d11.h>

//#include <DXOldSDK\\D3DX11.h>

#pragma comment( lib, "d3d11.lib" )
//#pragma comment( lib, "DXOldSDK\\d3dx11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

class IBaseOSAPIController;

class CD3D11Controller final : 
	public IBaseGAPIController,
	public CSingleton<CD3D11Controller>
{
	friend class CSingleton<CD3D11Controller>;
public:
	virtual ~CD3D11Controller();

	virtual CHError VInit() override;
	virtual void VStartFrame() override;
	virtual void VEndFrame() override;
	virtual void VResetDevice() override;

	ID3D11Device *GetDevice() const
	{
		return m_pD3DDevice;
	};
	ID3D11DeviceContext *GetContext() const
	{
		return m_pImmediateContext;
	};
	ID3D11RenderTargetView *GetBBRTV() const
	{
		return m_pBackBufferRTV;
	};
	ID3D11DepthStencilView *GetBBDSV() const
	{
		return m_pBackBufferDSV;
	};
private:
	IDXGISwapChain							*m_pSwapChain = nullptr;
	ID3D11Device							*m_pD3DDevice = nullptr;
	ID3D11DeviceContext						*m_pImmediateContext = nullptr;

	ID3D11RenderTargetView					*m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView					*m_pBackBufferDSV = nullptr;

	bool									m_bIsResetRequsted = false;

	std::unique_ptr<DXGI_SWAP_CHAIN_DESC>	m_pSwapChainDesc = nullptr;
};

#endif