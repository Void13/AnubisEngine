#include "EnginePCH.h"
#include "ShadersManager.h"
#include "D3D11GraphicsRenderer.h"

#include <D3Dcompiler.h>

#pragma comment( lib, "D3dcompiler.lib" )


CShaders::CShaders()
{
}

CShaders::~CShaders()
{
	Clear();
}

void CShaders::Clear()
{
	auto pInstance = GetInstance();
	for (auto iState : pInstance->m_SamplerStates)
	{
		SAFE_DELETE(iState);
	}
	pInstance->m_SamplerStates.clear();

	for (auto iBuffer : pInstance->m_ConstantBuffers)
	{
		SAFE_DELETE(iBuffer);
	}
	GetInstance()->m_ConstantBuffers.clear();
}

void CShaders::Init()
{
}

CHError CShaders::AddConstantBuffer(DWORD _dwSize, OUT CONSTBUFID &CBID_Out)
{
	HRESULT hr;

	ID3D11Device *pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();
	ID3D11Buffer *pD3DCBuffer;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = _dwSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	hr = pD3DDevice->CreateBuffer(&bufferDesc, NULL, &pD3DCBuffer);
	if (hr != S_OK)
	{
		return new CError_CreatingContantBufferFailed(AT);
	}

	CConstantBuffer *pCBuffer = new CConstantBuffer;
	pCBuffer->pConstantBuffer = pD3DCBuffer;
	pCBuffer->pData = new char[_dwSize];
	pCBuffer->dwSize = _dwSize;

	GetInstance()->m_ConstantBuffers.push_back(pCBuffer);
	CBID_Out = GetInstance()->m_ConstantBuffers.size() - 1;

	LOG_INFO("Added constant buffer with size %d", _dwSize);

	return new CError_OK(AT);
}

void CShaders::SetConstantBuffer(CONSTBUFID CB_ID, DWORD _dwSlot, SHADER_MASK SHM_Mask)
{
	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	if (SHM_Mask & SHM_VS)
		pImmediateContext->VSSetConstantBuffers(_dwSlot, 1, &GetInstance()->m_ConstantBuffers.at(CB_ID)->pConstantBuffer);
	if (SHM_Mask & SHM_PS)
		pImmediateContext->PSSetConstantBuffers(_dwSlot, 1, &GetInstance()->m_ConstantBuffers.at(CB_ID)->pConstantBuffer);
	if (SHM_Mask & SHM_GS)
		pImmediateContext->GSSetConstantBuffers(_dwSlot, 1, &GetInstance()->m_ConstantBuffers.at(CB_ID)->pConstantBuffer);
	if (SHM_Mask & SHM_CS)
		pImmediateContext->CSSetConstantBuffers(_dwSlot, 1, &GetInstance()->m_ConstantBuffers.at(CB_ID)->pConstantBuffer);
	if (SHM_Mask & SHM_DS)
		pImmediateContext->DSSetConstantBuffers(_dwSlot, 1, &GetInstance()->m_ConstantBuffers.at(CB_ID)->pConstantBuffer);
	if (SHM_Mask & SHM_HS)
		pImmediateContext->HSSetConstantBuffers(_dwSlot, 1, &GetInstance()->m_ConstantBuffers.at(CB_ID)->pConstantBuffer);
}

CConstantBuffer *const CShaders::GetCB(CONSTBUFID CB_ID)
{
	if (CB_ID != -1)
		return GetInstance()->m_ConstantBuffers.at(CB_ID);

	return nullptr;
}

void CShaders::UpdateCB(CONSTBUFID CB_ID)
{
	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();
	/*D3D11_MAPPED_SUBRESOURCE mappedResource;
	pImmediateContext->Map(pD3DCBPerBatch, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &CBPerBatch, sizeof(CCBPerBatch));
	pImmediateContext->Unmap(pD3DCBPerBatch, 0);*/
	pImmediateContext->UpdateSubresource(
		GetInstance()->m_ConstantBuffers.at(CB_ID)->pConstantBuffer, 0, NULL, 
		GetInstance()->m_ConstantBuffers.at(CB_ID)->pData, 0, 0);
}



CHError CShaders::CreateSampler(const D3D11_SAMPLER_DESC &_SamplerDesc, OUT DWORD *dwID)
{
	HRESULT hr;

	ID3D11Device *pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();

	ID3D11SamplerState *pSampler;
	hr = (pD3DDevice->CreateSamplerState(&_SamplerDesc, &pSampler));
	if (hr != S_OK)
	{
		return new CError_CreatingSamplerStateFailed(AT);
	}

	CSamplerState *pSamplerState = new CSamplerState;
	pSamplerState->pSamplerDesc = new D3D11_SAMPLER_DESC;
	memcpy(pSamplerState->pSamplerDesc, &_SamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	pSamplerState->pSamplerState = pSampler;

	GetInstance()->m_SamplerStates.push_back(pSamplerState);

	*dwID = GetInstance()->m_SamplerStates.size() - 1;

	LOG_INFO("Added sampler state with id %d", dwID);

	return S_OK;
}

void CShaders::SetSampler(SAMPLERID SID_ID, DWORD _dwSlot, SHADER_MASK SHM_Mask)
{
	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	if (SHM_Mask & SHM_VS)
		pImmediateContext->VSSetSamplers(_dwSlot, 1, &GetInstance()->m_SamplerStates.at(SID_ID)->pSamplerState);
	if (SHM_Mask & SHM_PS)
		pImmediateContext->PSSetSamplers(_dwSlot, 1, &GetInstance()->m_SamplerStates.at(SID_ID)->pSamplerState);
	if (SHM_Mask & SHM_GS)
		pImmediateContext->GSSetSamplers(_dwSlot, 1, &GetInstance()->m_SamplerStates.at(SID_ID)->pSamplerState);
	if (SHM_Mask & SHM_CS)
		pImmediateContext->CSSetSamplers(_dwSlot, 1, &GetInstance()->m_SamplerStates.at(SID_ID)->pSamplerState);
	if (SHM_Mask & SHM_DS)
		pImmediateContext->DSSetSamplers(_dwSlot, 1, &GetInstance()->m_SamplerStates.at(SID_ID)->pSamplerState);
	if (SHM_Mask & SHM_HS)
		pImmediateContext->HSSetSamplers(_dwSlot, 1, &GetInstance()->m_SamplerStates.at(SID_ID)->pSamplerState);
}



// --------------------------------------------------------------------------------------------
//
// --------------------------------------------------------------------------------------------

CTechnique::CTechnique()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pHullShader = nullptr;
	m_pDomainShader = nullptr;
	m_pGeometryShader = nullptr;
	m_pComputeShader = nullptr;
	m_pInputLayout = new CInputLayout;
}

CTechnique::~CTechnique()
{
	Destroy();
}

void CTechnique::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *ILDescs, DWORD dwNumElems)
{
	m_pInputLayout->nNumElems = dwNumElems;
	m_pInputLayout->pILDesc = new D3D11_INPUT_ELEMENT_DESC[dwNumElems];
	memcpy(m_pInputLayout->pILDesc, ILDescs, dwNumElems * sizeof(D3D11_INPUT_ELEMENT_DESC));
	m_pInputLayout->pInputLayout = NULL;

	LOG_INFO("Created input layout");
}

void CTechnique::SetShaders() const
{
	auto pImmediateContext = CD3D11Controller::GetInstance()->GetContext();

	pImmediateContext->IASetInputLayout(m_pInputLayout->pInputLayout);

	pImmediateContext->VSSetShader(m_pVertexShader, NULL, NULL);
	pImmediateContext->PSSetShader(m_pPixelShader, NULL, NULL);
	pImmediateContext->GSSetShader(m_pGeometryShader, NULL, NULL);
	pImmediateContext->CSSetShader(m_pComputeShader, NULL, NULL);
	pImmediateContext->DSSetShader(m_pDomainShader, NULL, NULL);
	pImmediateContext->HSSetShader(m_pHullShader, NULL, NULL);
}

void CTechnique::Destroy()
{
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pHullShader);
	SAFE_RELEASE(m_pDomainShader);
	SAFE_RELEASE(m_pGeometryShader);
	SAFE_RELEASE(m_pComputeShader);

	SAFE_DELETE(m_pInputLayout);
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, const D3D_SHADER_MACRO* pDefines = NULL)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( NDEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;

	hr = D3DCompileFromFile(szFileName, pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel, NULL, NULL, ppBlobOut, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob)
			pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

#define AUTSHADERS_SAVE_TO_FILE 0

void WriteReadShader(ID3DBlob **ppSBlob, const wchar_t *const sTargetFileName)
{
	FILE *pFile;

#ifdef AUTSHADERS_SAVE_TO_FILE
	_wfopen_s(&pFile, sTargetFileName, L"wb");
	fwrite((*ppSBlob)->GetBufferPointer(), (*ppSBlob)->GetBufferSize(), 1, pFile);
	fclose(pFile);
#endif

	int nSize = 0;
	_wfopen_s(&pFile, sTargetFileName, L"rb");

	fseek(pFile, 0, SEEK_END);
	nSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	*ppSBlob = NULL;
	D3DCreateBlob(nSize, ppSBlob);

	fread_s((*ppSBlob)->GetBufferPointer(), nSize, nSize, 1, pFile);

	fclose(pFile);
}

CHError CTechnique::CreateGeometrySO(LPCWSTR szFileName, LPCSTR szEntryPoint, 
	LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, const UINT *pBufferStrides, 
	DWORD dwNumStrides, D3D11_SO_DECLARATION_ENTRY *pSODeclaraion, DWORD dwNumSODeclaraions)
{
	HRESULT hr;

	ID3DBlob* pSBlob = NULL;
	ID3D11GeometryShader *GStmp = NULL;
	ID3D11Device *pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();

	static wchar_t wszTex[64];
	mbstowcs(wszTex, szShaderName, 63);
	std::wstring sPath, sFileName;
	StringHelper::SplitFileName(std::wstring(szFileName), L"\\", sPath, sFileName);

	std::wstring sSourceFile = sPath + L"\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
	FILE *pFile;
	_wfopen_s(&pFile, sSourceFile.c_str(), L"rb");

	if (!pFile)
	{
		return new CError_UnableToFindShader(AT);
	}
	fclose(pFile);

	std::string sEntryPoint = szEntryPoint;
	sEntryPoint = "GS_" + sEntryPoint;

	hr = CompileShaderFromFile(sSourceFile.c_str(), sEntryPoint.c_str(), "gs_5_0", &pSBlob, pDefines);
	if (FAILED(hr))
	{
		CHError er = new CError_UnableToCompileShader(AT);

		return new CError_SOCreatingFailed(AT);
	}
#endif

	std::wstring sTargetFileName = sPath + L"\\" + wszTex + std::wstring(L".soo");
	WriteReadShader(&pSBlob, sTargetFileName.c_str());

	hr = pD3DDevice->CreateGeometryShaderWithStreamOutput(pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(),
		pSODeclaraion, dwNumSODeclaraions,
		pBufferStrides, dwNumStrides, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &GStmp);
	
	if (FAILED(hr))
	{
		return new CError_SOCreatingFailed(AT);
	}

	m_pGeometryShader = GStmp;

	LOG_INFO("Created geometry SO. Shader: %s, %s", StringHelper::ws2s(sFileName).c_str(), sEntryPoint.c_str());

	return S_OK;
}

CHError CTechnique::CreateShaders(LPCWSTR szFileName, LPCSTR szEntryPoint,
	LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, SHADER_MASK SHM_Mask)
{
	HRESULT hr;

	ID3D11Device *pD3DDevice = CD3D11Controller::GetInstance()->GetDevice();

	if (SHM_Mask & SHM_VS)
	{
		ID3DBlob* pSBlob = NULL;
		ID3D11VertexShader *VStmp = NULL;

		wchar_t wszTex[64];
		mbstowcs(wszTex, szShaderName, 63);
		std::wstring sPath, sFileName;
		StringHelper::SplitFileName(std::wstring(szFileName), L"\\", sPath, sFileName);

		std::wstring sSourceFile = sPath + L"\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
		FILE *pFile;
		_wfopen_s(&pFile, sSourceFile.c_str(), L"rb");

		if (!pFile)
		{
			return new CError_UnableToFindShader(AT);
		}
		fclose(pFile);

		std::string sEntryPoint = szEntryPoint;
		sEntryPoint = "VS_" + sEntryPoint;

		hr = CompileShaderFromFile(sSourceFile.c_str(), sEntryPoint.c_str(), "vs_5_0", 
			&pSBlob, pDefines);
		if (FAILED(hr))
		{
			return new CError_UnableToCompileShader(AT);
		}
#endif

		std::wstring sTargetFileName = sPath + L"\\" + wszTex + std::wstring(L".vso");
		WriteReadShader(&pSBlob, sTargetFileName.c_str());

		hr = pD3DDevice->CreateVertexShader(pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &VStmp);
		if (FAILED(hr))
		{
			return new CError_VertexShaderCreatingFailed(AT);
		}

		if (m_pInputLayout->pILDesc)
		{
			hr = pD3DDevice->CreateInputLayout(m_pInputLayout->pILDesc, m_pInputLayout->nNumElems, 
				pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), &m_pInputLayout->pInputLayout);
			if (FAILED(hr))
			{
				return new CError_InputLayoutCreationFailed(AT);
			}
		}

		SAFE_RELEASE(pSBlob);

		LOG_INFO("Created vertex shader: %s, %s", StringHelper::ws2s(sFileName).c_str(), sEntryPoint.c_str());

		m_pVertexShader = VStmp;
	}

	if (SHM_Mask & SHM_PS)
	{
		ID3DBlob* pSBlob = NULL;
		ID3D11PixelShader *PStmp = NULL;

		static wchar_t wszTex[64];
		mbstowcs(wszTex, szShaderName, 63);
		std::wstring sPath, sFileName;
		StringHelper::SplitFileName(std::wstring(szFileName), L"\\", sPath, sFileName);

		std::wstring sSourceFile = sPath + L"\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
		FILE *pFile;
		_wfopen_s(&pFile, sSourceFile.c_str(), L"rb");

		if (!pFile)
		{
			return new CError_UnableToFindShader(AT);
		}
		fclose(pFile);

		std::string sEntryPoint = szEntryPoint;
		sEntryPoint = "PS_" + sEntryPoint;

		hr = CompileShaderFromFile(sSourceFile.c_str(), sEntryPoint.c_str(), "ps_5_0", &pSBlob, pDefines);
		if (FAILED(hr))
		{
			return new CError_UnableToCompileShader(AT);
		}
#endif

		std::wstring sTargetFileName = sPath + L"\\" + wszTex + std::wstring(L".pso");
		WriteReadShader(&pSBlob, sTargetFileName.c_str());

		hr = pD3DDevice->CreatePixelShader(pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &PStmp);
		SAFE_RELEASE(pSBlob);
		if (FAILED(hr))
		{
			return new CError_PixelShaderCreatingFailed(AT);
		}

		LOG_INFO("Created pixel shader: %s, %s", StringHelper::ws2s(sFileName).c_str(), sEntryPoint.c_str());

		m_pPixelShader = PStmp;
	}

	if (SHM_Mask & SHM_GS)
	{
		ID3DBlob* pSBlob = NULL;
		ID3D11GeometryShader *GStmp = NULL;

		static wchar_t wszTex[64];
		mbstowcs(wszTex, szShaderName, 63);
		std::wstring sPath, sFileName;
		StringHelper::SplitFileName(std::wstring(szFileName), L"\\", sPath, sFileName);

		std::wstring sSourceFile = sPath + L"\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
		FILE *pFile;
		_wfopen_s(&pFile, sSourceFile.c_str(), L"rb");

		if (!pFile)
		{
			return new CError_UnableToFindShader(AT);
		}
		fclose(pFile);

		std::string sEntryPoint = szEntryPoint;
		sEntryPoint = "GS_" + sEntryPoint;

		hr = CompileShaderFromFile(sSourceFile.c_str(), sEntryPoint.c_str(), "gs_5_0", &pSBlob, pDefines);
		if (FAILED(hr))
		{
			return new CError_UnableToCompileShader(AT);
		}
#endif

		std::wstring sTargetFileName = sPath + L"\\" + wszTex + std::wstring(L".gso");
		WriteReadShader(&pSBlob, sTargetFileName.c_str());

		hr = pD3DDevice->CreateGeometryShader(pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &GStmp);
		if (FAILED(hr))
		{
			return new CError_GeometryShaderCreatingFailed(AT);
		}

		LOG_INFO("Created geometry shader: %s, %s", StringHelper::ws2s(sFileName).c_str(), sEntryPoint.c_str());

		SAFE_RELEASE(pSBlob);
		m_pGeometryShader = GStmp;
	}

	if (SHM_Mask & SHM_HS)
	{
		ID3DBlob* pSBlob = NULL;
		ID3D11HullShader *HStmp = NULL;

		static wchar_t wszTex[64];
		mbstowcs(wszTex, szShaderName, 63);
		std::wstring sPath, sFileName;
		StringHelper::SplitFileName(std::wstring(szFileName), L"\\", sPath, sFileName);

		std::wstring sSourceFile = sPath + L"\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
		FILE *pFile;
		_wfopen_s(&pFile, sSourceFile.c_str(), L"rb");

		if (!pFile)
		{
			return new CError_UnableToFindShader(AT);
		}
		fclose(pFile);

		std::string sEntryPoint = szEntryPoint;
		sEntryPoint = "HS_" + sEntryPoint;

		hr = CompileShaderFromFile(sSourceFile.c_str(), sEntryPoint.c_str(), "hs_5_0", &pSBlob, pDefines);
		if (FAILED(hr))
		{
			return new CError_UnableToCompileShader(AT);
		}
#endif

		std::wstring sTargetFileName = sPath + L"\\" + wszTex + std::wstring(L".hso");
		WriteReadShader(&pSBlob, sTargetFileName.c_str());

		hr = pD3DDevice->CreateHullShader(pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &HStmp);
		SAFE_RELEASE(pSBlob);
		if (FAILED(hr))
		{
			return new CError_HullShaderCreatingFailed(AT);
		}

		LOG_INFO("Created hull shader: %s, %s", StringHelper::ws2s(sFileName).c_str(), sEntryPoint.c_str());

		m_pHullShader = HStmp;
	}

	if (SHM_Mask & SHM_DS)
	{
		ID3DBlob* pSBlob = NULL;
		ID3D11DomainShader *DStmp = NULL;

		static wchar_t wszTex[64];
		mbstowcs(wszTex, szShaderName, 63);
		std::wstring sPath, sFileName;
		StringHelper::SplitFileName(std::wstring(szFileName), L"\\", sPath, sFileName);

		std::wstring sSourceFile = sPath + L"\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
		FILE *pFile;
		_wfopen_s(&pFile, sSourceFile.c_str(), L"rb");

		if (!pFile)
		{
			return new CError_UnableToFindShader(AT);
		}
		fclose(pFile);

		std::string sEntryPoint = szEntryPoint;
		sEntryPoint = "DS_" + sEntryPoint;

		hr = CompileShaderFromFile(sSourceFile.c_str(), sEntryPoint.c_str(), "ds_5_0", &pSBlob, pDefines);
		if (FAILED(hr))
		{
			return new CError_UnableToCompileShader(AT);
		}
#endif

		std::wstring sTargetFileName = sPath + L"\\" + wszTex + std::wstring(L".dso");
		WriteReadShader(&pSBlob, sTargetFileName.c_str());

		hr = pD3DDevice->CreateDomainShader(pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &DStmp);
		SAFE_RELEASE(pSBlob);
		if (FAILED(hr))
		{
			return new CError_DomainShaderCreatingFailed(AT);
		}

		LOG_INFO("Created domain shader: %s, %s", StringHelper::ws2s(sFileName).c_str(), sEntryPoint.c_str());

		m_pDomainShader = DStmp;
	}

	if (SHM_Mask & SHM_CS)
	{
		ID3DBlob* pSBlob = NULL;
		ID3D11ComputeShader *CStmp = NULL;

		static wchar_t wszTex[64];
		mbstowcs(wszTex, szShaderName, 63);
		std::wstring sPath, sFileName;
		StringHelper::SplitFileName(std::wstring(szFileName), L"\\", sPath, sFileName);

		std::wstring sSourceFile = sPath + L"\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
		FILE *pFile;
		_wfopen_s(&pFile, sSourceFile.c_str(), L"rb");

		if (!pFile)
		{
			return new CError_UnableToFindShader(AT);
		}
		fclose(pFile);

		std::string sEntryPoint = szEntryPoint;
		sEntryPoint = "CS_" + sEntryPoint;

		hr = CompileShaderFromFile(sSourceFile.c_str(), sEntryPoint.c_str(), "cs_5_0", &pSBlob, pDefines);
		if (FAILED(hr))
		{
			return new CError_UnableToCompileShader(AT);
		}
#endif

		std::wstring sTargetFileName = sPath + L"\\" + wszTex + std::wstring(L".cso");
		WriteReadShader(&pSBlob, sTargetFileName.c_str());

		hr = pD3DDevice->CreateComputeShader(pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &CStmp);
		SAFE_RELEASE(pSBlob);
		if (FAILED(hr))
		{
			return new CError_ComputeShaderCreatingFailed(AT);
		}

		LOG_INFO("Created compute shader: %s, %s", StringHelper::ws2s(sFileName).c_str(), sEntryPoint.c_str());

		m_pComputeShader = CStmp;
	}

	return S_OK;
}