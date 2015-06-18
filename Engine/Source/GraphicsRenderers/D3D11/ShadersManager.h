#ifndef H__SHADERSMANAGER
#define H__SHADERSMANAGER

#define AUTSHADERSFUNC

#include "EnginePCH.h"

#include "GAPIControllers\\D3D11Controller.h"

typedef char SHADER_MASK;

#define SHM_CS 0x20
#define SHM_VS 0x10
#define SHM_HS 0x8
#define SHM_DS 0x4
#define SHM_GS 0x2
#define SHM_PS 0x1

class CInputLayout
{
public:
	CInputLayout()
	{
	}

	~CInputLayout()
	{
		SAFE_RELEASE(pInputLayout);
		SAFE_DELETE_ARRAY(pILDesc);
	}

	D3D11_INPUT_ELEMENT_DESC	*pILDesc = nullptr;
	ID3D11InputLayout			*pInputLayout = nullptr;
	int							nNumElems;
};

class CSamplerState
{
public:
	CSamplerState()
	{
	}
	~CSamplerState()
	{
		SAFE_DELETE(pSamplerDesc);
		SAFE_RELEASE(pSamplerState);
	}
	D3D11_SAMPLER_DESC			*pSamplerDesc = nullptr;
	ID3D11SamplerState			*pSamplerState = nullptr;
};


class CConstantBuffer
{
public:
	CConstantBuffer()
	{
	}
	~CConstantBuffer()
	{
		SAFE_DELETE_ARRAY(pData);
		SAFE_RELEASE(pConstantBuffer);
	}

	ID3D11Buffer				*pConstantBuffer = nullptr;
	void						*pData = nullptr;
	DWORD						dwSize = 0;
};

typedef DWORD CONSTBUFID;
typedef DWORD SAMPLERID;

class CShaders : public CSingleton<CShaders>
{
	friend class CSingleton<CShaders>;
	friend class CTechnique;
public:
	CShaders();
	~CShaders();

	static void Clear();
	static void Init();
	static CHError AddConstantBuffer(DWORD _dwSize, OUT CONSTBUFID &CBID_Out);
	static void SetConstantBuffer(CONSTBUFID CB_ID, DWORD _dwSlot, SHADER_MASK SHM_Mask);
	static CConstantBuffer *const GetCB(CONSTBUFID CB_ID);
	static void UpdateCB(CONSTBUFID CB_ID);
	static CHError CreateSampler(const D3D11_SAMPLER_DESC &_SamplerDesc, OUT DWORD *dwID);
	static void SetSampler(SAMPLERID SID_ID, DWORD _dwSlot, SHADER_MASK SHM_Mask);
protected:
	std::vector<CConstantBuffer *>			m_ConstantBuffers;
	std::vector<CSamplerState *>			m_SamplerStates;
};

class CTechnique
{
public:
	CTechnique();
	~CTechnique();

	CHError CreateGeometrySO(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines,
		const UINT *pBufferStrides, DWORD dwNumStrides, D3D11_SO_DECLARATION_ENTRY *pSODeclaraion, DWORD dwNumSODeclaraions);
	CHError CreateShaders(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, SHADER_MASK SHM_Mask);
	void CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *ILDescs, DWORD dwNumElems);
	void SetShaders() const;
	void Destroy();
protected:
	ID3D11VertexShader		*m_pVertexShader = nullptr;
	ID3D11PixelShader		*m_pPixelShader = nullptr;
	ID3D11HullShader		*m_pHullShader = nullptr;
	ID3D11DomainShader		*m_pDomainShader = nullptr;
	ID3D11GeometryShader	*m_pGeometryShader = nullptr;
	ID3D11ComputeShader		*m_pComputeShader = nullptr;

	CInputLayout			*m_pInputLayout = nullptr;
};

#endif