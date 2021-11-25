#pragma once
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "GraphicsResource.h"

class DepthStencil;

class RenderTarget : public GraphicsResource
{
public:
	RenderTarget( GraphicsDeviceInterface& gfx, UINT width, UINT height );
	void BindAsTexture( GraphicsDeviceInterface& gfx, UINT slot ) const noexcept;
	void BindAsTarget( GraphicsDeviceInterface& gfx ) const noexcept;
	void BindAsTarget( GraphicsDeviceInterface& gfx, const DepthStencil& depthStencil ) const noexcept;
private:
	ID3D11ShaderResourceView* pTextureView;
	ID3D11RenderTargetView* pTargetView;
};