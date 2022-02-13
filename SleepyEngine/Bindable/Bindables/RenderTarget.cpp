#include "RenderTarget.h"
#include "DepthStencil.h"
#include "../../ResourceManager/Surface.h"
#include <array>
#include <wrl.h>

namespace Bind
{
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
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // never do we not want to bind offscreen RTs as inputs
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		ID3D11Texture2D* pTexture;
		GetDevice( gfx )->CreateTexture2D(
			&textureDesc, nullptr, &pTexture
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

	RenderTarget::RenderTarget( GraphicsDeviceInterface& gfx, ID3D11Texture2D* pTexture )
	{
		// get information from texture about dimensions
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc( &textureDesc );
		width = textureDesc.Width;
		height = textureDesc.Height;

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		GetDevice( gfx )->CreateRenderTargetView(
			pTexture, &rtvDesc, &pTargetView
		);
	}

	void RenderTarget::BindAsBuffer( GraphicsDeviceInterface& gfx ) noexcept
	{
		ID3D11DepthStencilView* const null = nullptr;
		BindAsBuffer( gfx, null );
	}

	void RenderTarget::BindAsBuffer( GraphicsDeviceInterface& gfx, BufferResource* depthStencil ) noexcept
	{
		assert( dynamic_cast<DepthStencil*>( depthStencil ) != nullptr );
		BindAsBuffer( gfx, static_cast<DepthStencil*>( depthStencil ) );
	}

	void RenderTarget::BindAsBuffer( GraphicsDeviceInterface& gfx, DepthStencil* depthStencil ) noexcept
	{
		BindAsBuffer( gfx, depthStencil ? depthStencil->pDepthStencilView : nullptr );
	}

	void RenderTarget::BindAsBuffer( GraphicsDeviceInterface& gfx, ID3D11DepthStencilView* pDepthStencilView ) noexcept
	{
		GetContext( gfx )->OMSetRenderTargets( 1, &pTargetView, pDepthStencilView );

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

	void RenderTarget::Clear( GraphicsDeviceInterface& gfx, const std::array<float, 4>& color ) noexcept
	{
		GetContext( gfx )->ClearRenderTargetView( pTargetView, color.data() );
	}

	void RenderTarget::Clear( GraphicsDeviceInterface& gfx ) noexcept
	{
		Clear( gfx, { 0.0f,0.0f,0.0f,0.0f } );
	}

	UINT RenderTarget::GetWidth() const noexcept
	{
		return width;
	}

	UINT RenderTarget::GetHeight() const noexcept
	{
		return height;
	}


	ShaderInputRenderTarget::ShaderInputRenderTarget( GraphicsDeviceInterface& gfx, UINT width, UINT height, UINT slot )
		:
		RenderTarget( gfx, width, height ),
		slot( slot )
	{
		ID3D11Resource* pRes;
		pTargetView->GetResource( &pRes );

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GetDevice( gfx )->CreateShaderResourceView(
			pRes, &srvDesc, &pShaderResourceView
		);
	}

	void ShaderInputRenderTarget::Bind( GraphicsDeviceInterface& gfx ) noexcept
	{
		GetContext( gfx )->PSSetShaderResources( slot, 1, &pShaderResourceView );
	}


	void OutputOnlyRenderTarget::Bind( GraphicsDeviceInterface& gfx ) noexcept
	{
		assert( "Cannot bind OuputOnlyRenderTarget as shader input" && false );
	}

	Surface Bind::ShaderInputRenderTarget::ToSurface( GraphicsDeviceInterface& gfx ) const
	{
		namespace wrl = Microsoft::WRL;

		// creating a temp texture compatible with the source, but with CPU read access
		wrl::ComPtr<ID3D11Resource> pResSource;
		pShaderResourceView->GetResource( &pResSource );
		wrl::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As( &pTexSource );
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexSource->GetDesc( &textureDesc );
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		wrl::ComPtr<ID3D11Texture2D> pTexTemp;
		GetDevice( gfx )->CreateTexture2D(
			&textureDesc, nullptr, &pTexTemp
		);

		// copy texture contents
		GetContext( gfx )->CopyResource( pTexTemp.Get(), pTexSource.Get() );

		// create Surface and copy from temp texture to it
		const auto width = GetWidth();
		const auto height = GetHeight();
		Surface s{ width,height };
		D3D11_MAPPED_SUBRESOURCE msr = {};
		GetContext( gfx )->Map( pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr );
		auto pSrcBytes = static_cast<const char*>( msr.pData );
		for ( unsigned int y = 0; y < height; y++ )
		{
			auto pSrcRow = reinterpret_cast<const Surface::Color*>( pSrcBytes + msr.RowPitch * size_t( y ) );
			for ( unsigned int x = 0; x < width; x++ )
			{
				s.PutPixel( x, y, *( pSrcRow + x ) );
			}
		}
		GetContext( gfx )->Unmap( pTexTemp.Get(), 0 );

		return s;
	}


	OutputOnlyRenderTarget::OutputOnlyRenderTarget( GraphicsDeviceInterface& gfx, ID3D11Texture2D* pTexture )
		:
		RenderTarget( gfx, pTexture )
	{}
}