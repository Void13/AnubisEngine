#ifndef H__BASEGRAPHICSRENDERER
#define H__BASEGRAPHICSRENDERER

#include "EnginePCH.h"
#include "GAPIControllers\\BaseGAPIController.h"
#include "VertexStructs.h"

class IActor;
class IBaseModelRenderer;

enum class E_RENDERMETHOD
{
	SOLID = 0,
	WIREFRAMED,

	WHEEL,
	TERRAIN

	// etc
};

class IBaseGraphicsRenderer
{
public:
	typedef std::unordered_map<IBaseModelRenderer *, std::vector<IActor *>> RenderQueue;
public:
	IBaseGraphicsRenderer()
	{
	};

	virtual ~IBaseGraphicsRenderer()
	{
	};

	virtual CHError VInit() = 0;
	virtual void VOnFrameRender() = 0;

	void SetWireframeQueue(RenderQueue const &WireframeQueue)
	{
		if (!WireframeQueue.empty())
		{
			m_pWireframeQueue = &WireframeQueue;
		}
	};

	void SetSolidQueue(RenderQueue const &SolidQueue)
	{
		if (!SolidQueue.empty())
		{
			m_pSolidQueue = &SolidQueue;
		}
	};

	void SetDebugQueue(RenderQueue const &DebugQueue)
	{
		if (!DebugQueue.empty())
		{
			m_pDebugQueue = &DebugQueue;
		}
	};

	void SetTransparencyQueue(RenderQueue const &TransparencyQueue)
	{
		if (!TransparencyQueue.empty())
		{
			m_pTransparencyQueue = &TransparencyQueue;
		}
	};
protected:
	RenderQueue			const *m_pWireframeQueue = nullptr;
	RenderQueue			const *m_pSolidQueue = nullptr;
	RenderQueue			const *m_pDebugQueue = nullptr;
	RenderQueue			const *m_pTransparencyQueue = nullptr;
};

#endif