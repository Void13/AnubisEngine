#ifndef H__D3D11GRAPHICSREND
#define H__D3D11GRAPHICSREND

#include "EnginePCH.h"
#include "ShadersManager.h"
#include "GraphicsRenderers\\BaseGraphicsRenderer.h"

struct CBPerMesh
{
	XMMATRIX	mViewProj;

	CBMaterial	Material;

	float		fFarPlane;
	int			bIsUseTexture;
	float		fOther[2];
};

class CD3D11GraphicsRenderer final : 
	public IBaseGraphicsRenderer,
	public CSingleton<CD3D11GraphicsRenderer>
{
	friend class CSingleton<CD3D11GraphicsRenderer>;
public:
	virtual ~CD3D11GraphicsRenderer();
	virtual CHError VInit() override;
	virtual void VOnFrameRender() override;

	CONSTBUFID GetCBIDPerMesh() const
	{
		return m_cbidPerMesh;
	};

	D3D11_VIEWPORT const &GetViewport() const
	{
		return m_Viewport;
	};
private:
	void RenderWireframeObjects();
	void RenderSolidObjects();
	void ResetSystem();

	CEGUI::Renderer		*m_pCEGUIRenderer = nullptr;
	CEGUI::System		*m_pCEGUISystem = nullptr;

	CONSTBUFID			m_cbidPerMesh = 0;
	D3D11_VIEWPORT		m_Viewport;
};

#endif