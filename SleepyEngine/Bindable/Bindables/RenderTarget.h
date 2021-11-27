#pragma once
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "GraphicsResource.h"
#include <array>

class DepthStencil;

class RenderTarget : public GraphicsResource
{
public:
	RenderTarget( GraphicsDeviceInterface& gfx, UINT width, UINT height );
	void BindAsTexture( GraphicsDeviceInterface& gfx, UINT slot ) const noexcept;
	void BindAsTarget( GraphicsDeviceInterface& gfx ) const noexcept;
	void BindAsTarget( GraphicsDeviceInterface& gfx, const DepthStencil& depthStencil ) const noexcept;
	void Clear( GraphicsDeviceInterface& gfx, const std::array<float, 4>& color ) const noexcept;
	void Clear( GraphicsDeviceInterface& gfx ) const noexcept;
private:
	ID3D11ShaderResourceView* pTextureView;
	ID3D11RenderTargetView* pTargetView;
};