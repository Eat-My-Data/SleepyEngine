#include "DepthStencil.h"

DepthStencil::DepthStencil( GraphicsDeviceInterface& gfx, UINT width, UINT height )
{
	// create depth stensil texture
	ID3D11Texture2D* pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GetDevice( gfx )->CreateTexture2D( &descDepth, nullptr, &pDepthStencil );

	// create target view of depth stensil texture
	GetDevice( gfx )->CreateDepthStencilView(
		pDepthStencil, nullptr, &pDepthStencilView
	);
}

void DepthStencil::BindAsDepthStencil( GraphicsDeviceInterface& gfx ) const noexcept
{
	GetContext( gfx )->OMSetRenderTargets( 0, nullptr, pDepthStencilView );
}

void DepthStencil::Clear( GraphicsDeviceInterface& gfx ) const noexcept
{
	GetContext( gfx )->ClearDepthStencilView( pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
}