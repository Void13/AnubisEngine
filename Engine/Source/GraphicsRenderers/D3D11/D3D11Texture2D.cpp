#include "EnginePCH.h"

#include "D3D11Texture2D.h"
#include "D3D11GraphicsRenderer.h"

#include <D3DX11.h>
#pragma comment( lib, "d3dx11.lib" )

void CD3D11Texture2D::VInitTexture(CTextureInitInfo const &InitInfo)
{
	m_sName = InitInfo.sName;

	auto pDevice = CD3D11Controller::GetInstance()->GetDevice();

	D3DX11_IMAGE_LOAD_INFO ImLoadInfo;
	ImLoadInfo.Filter = D3DX11_FILTER_LINEAR;
	ImLoadInfo.MipFilter = D3DX11_FILTER_LINEAR;
	ImLoadInfo.Usage = D3D11_USAGE_IMMUTABLE;
	ImLoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ImLoadInfo.CpuAccessFlags = 0;
	ImLoadInfo.MiscFlags = 0;
	// DXGI_FORMAT_R8G8B8A8_UNORM
	ImLoadInfo.Format = DXGI_FORMAT_BC1_UNORM;

	if (D3DX11CreateShaderResourceViewFromMemory(pDevice, InitInfo.pData, InitInfo.dwDataSize, &ImLoadInfo, nullptr, &m_pSRV, nullptr) != S_OK)
	{
		CHError er = new CError_D3D11ResourceFailed(AT);
		return;
	}

	ID3D11Resource *pRes;
	m_pSRV->GetResource(&pRes);
	pRes->QueryInterface(&m_pTex2D);

	D3D11_TEXTURE2D_DESC desc;
	m_pTex2D->GetDesc(&desc);
	m_nWidth = desc.Width;
	m_nHeight = desc.Height;

	SAFE_RELEASE(pRes);

	/*
	D3D11_TEXTURE2D_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Desc.Width = InitInfo.nWidth;
	Desc.Height = InitInfo.nHeight;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_BC1_UNORM;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.MiscFlags = 0;
	Desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = InitInfo.pData;
	InitData.SysMemPitch = 16 * (InitInfo.nWidth / 8);

	auto pDevice = CD3D11Controller::GetInstance()->GetDevice();
	if (pDevice->CreateTexture2D(&Desc, &InitData, &m_pTex2D) != S_OK)
	{
		CHError er = new CError_D3D11ResourceFailed(AT);
		return;
	}

	if (pDevice->CreateShaderResourceView(m_pTex2D, nullptr, &m_pSRV) != S_OK)
	{
		CHError er = new CError_D3D11ResourceFailed(AT);
		return;
	}
	*/
	//auto pContext = CD3D11Controller::GetInstance()->GetContext();
	//pContext->GenerateMips(m_pSRV);
};

void CD3D11Texture2D::VMapTexture(void *&pData)
{
	auto pContext = CD3D11Controller::GetInstance()->GetContext();

	D3D11_MAPPED_SUBRESOURCE MapRes;
	if (pContext->Map(m_pTex2D, D3D11CalcSubresource(0, 0, 1),
		D3D11_MAP_WRITE_DISCARD, 0, &MapRes) != S_OK)
	{
		CHError er = new CError_ResourceMapFailed(AT);
		return;
	}

	pData = MapRes.pData;
};

void CD3D11Texture2D::VUnmapTexture()
{
	auto pContext = CD3D11Controller::GetInstance()->GetContext();
	pContext->Unmap(m_pTex2D, D3D11CalcSubresource(0, 0, 1));
};

void CD3D11Texture2D::VBindTexture(size_t dwSlot)
{
	CD3D11Controller::GetInstance()->GetContext()->PSSetShaderResources(dwSlot, 1, &m_pSRV);
};

CD3D11Texture2D::~CD3D11Texture2D()
{
	SAFE_RELEASE(m_pTex2D);
	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pUAV);
}