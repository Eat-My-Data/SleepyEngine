#include "RenderTarget.h"
#include "DepthStencil.h"

RenderTarget::RenderTarget( GraphicsDeviceInterface& gfx, UINT width, UINT height )
	:
	width( width ),
	height( height )
{
	// create texture resource
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	ID3D11Texture2D* pTexture;
	GetDevice( gfx )->CreateTexture2D(
		&textureDesc, nullptr, &pTexture
	);

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GetDevice( gfx )->CreateShaderResourceView(
		pTexture, &srvDesc, &pTextureView
	);

	// create the target view on the texture
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
	GetDevice( gfx )->CreateRenderTargetView(
		pTexture, &rtvDesc, &pTargetView
	);
}

void RenderTarget::BindAsTexture( GraphicsDeviceInterface& gfx, UINT slot ) const noexcept
{
	GetContext( gfx )->PSSetShaderResources( slot, 1, &pTextureView );
}

void RenderTarget::BindAsTarget( GraphicsDeviceInterface& gfx ) const noexcept
{
	GetContext( gfx )->OMSetRenderTargets( 1, &pTargetView, nullptr );

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	GetContext( gfx )->RSSetViewports( 1u, &vp );
}

void RenderTarget::BindAsTarget( GraphicsDeviceInterface& gfx, const DepthStencil& depthStencil ) const noexcept
{
	GetContext( gfx )->OMSetRenderTargets( 1, &pTargetView, depthStencil.pDepthStencilView );

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	GetContext( gfx )->RSSetViewports( 1u, &vp );
}

void RenderTarget::Clear( GraphicsDeviceInterface& gfx, const std::array<float, 4>& color ) const noexcept
{
	GetContext( gfx )->ClearRenderTargetView( pTargetView, color.data() );
}

void RenderTarget::Clear( GraphicsDeviceInterface& gfx ) const noexcept
{
	Clear( gfx, { 0.0f,0.0f,0.0f,0.0f } );
}