#pragma once
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "GraphicsResource.h"


class DepthStencil : public GraphicsResource
{
	friend class RenderTarget;
	friend class GraphicsDeviceInterface;
public:
	DepthStencil( GraphicsDeviceInterface& gfx, UINT width, UINT height );
	void BindAsDepthStencil( GraphicsDeviceInterface& gfx ) const noexcept;
	void Clear( GraphicsDeviceInterface& gfx ) const noexcept;
private:
	ID3D11DepthStencilView* pDepthStencilView;
};