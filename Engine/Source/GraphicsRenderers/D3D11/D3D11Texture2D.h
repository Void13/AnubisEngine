#pragma once

#include "..\\BaseTexture2D.h"

class ID3D11Texture2D;
class ID3D11ShaderResourceView;
class ID3D11UnorderedAccessView;

class CD3D11Texture2D final : public ITexture2D
{
public:
	virtual ~CD3D11Texture2D();

	virtual void VInitTexture(CTextureInitInfo const &InitInfo) override;
	virtual void VBindTexture(size_t dwSlot) override;
private:
	virtual void VMapTexture(void *&pData) override;
	virtual void VUnmapTexture() override;
private:
	ID3D11Texture2D					*m_pTex2D = nullptr;

	// attached to shader(except compute shader)
	ID3D11ShaderResourceView		*m_pSRV = nullptr;

	// attached to compute shader
	ID3D11UnorderedAccessView		*m_pUAV = nullptr;
};