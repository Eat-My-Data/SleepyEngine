#include "RenderTarget.h"
#include "../../Macros/GraphicsThrowMacros.h"
#include "DepthStencil.h"
#include "../../Renderer/Surface.h"
#include <stdexcept>
#include <array>
#include <wrl.h>
#include "../../Libraries/cnpy/cnpy.h"

namespace wrl = Microsoft::WRL;

namespace Bind
{
	RenderTarget::RenderTarget( Graphics& gfx, UINT width, UINT height )
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

	RenderTarget::RenderTarget( Graphics& gfx, ID3D11Texture2D* pTexture, std::optional<UINT> face )
	{
		// get information from texture about dimensions
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc( &textureDesc );
		width = textureDesc.Width;
		height = textureDesc.Height;

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;

		if ( face.has_value() )
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.ArraySize = 1;
			rtvDesc.Texture2DArray.FirstArraySlice = *face;
			rtvDesc.Texture2DArray.MipSlice = 0;
		}
		else
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		}

		GetDevice( gfx )->CreateRenderTargetView(
			pTexture, &rtvDesc, &pTargetView
		);
	}

	std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> RenderTarget::MakeStaging( Graphics& gfx ) const
	{
		INFOMAN( gfx );

		// get info about the stencil view
		D3D11_RENDER_TARGET_VIEW_DESC srcViewDesc{};
		pTargetView->GetDesc( &srcViewDesc );
		// creating a temp texture compatible with the source, but with CPU read access
		wrl::ComPtr<ID3D11Resource> pResSource;
		pTargetView->GetResource( &pResSource );
		wrl::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As( &pTexSource );
		D3D11_TEXTURE2D_DESC srcTextureDesc{};
		pTexSource->GetDesc( &srcTextureDesc );
		D3D11_TEXTURE2D_DESC tmpTextureDesc = srcTextureDesc;
		tmpTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tmpTextureDesc.Usage = D3D11_USAGE_STAGING;
		tmpTextureDesc.BindFlags = 0;
		tmpTextureDesc.MiscFlags = 0;
		tmpTextureDesc.ArraySize = 1;
		wrl::ComPtr<ID3D11Texture2D> pTexTemp;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D(
			&tmpTextureDesc, nullptr, &pTexTemp
		) );

		// copy texture contents
		if ( srcViewDesc.ViewDimension == D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DARRAY )
		{
			// source is actually inside a cubemap texture, use view info to find the correct slice and copy subresource
			GFX_THROW_INFO_ONLY( GetContext( gfx )->CopySubresourceRegion( pTexTemp.Get(), 0, 0, 0, 0, pTexSource.Get(), srcViewDesc.Texture2DArray.FirstArraySlice, nullptr ) );
		}
		else
		{
			GFX_THROW_INFO_ONLY( GetContext( gfx )->CopyResource( pTexTemp.Get(), pTexSource.Get() ) );
		}

		return { std::move( pTexTemp ),srcTextureDesc };
	}

	Surface Bind::RenderTarget::ToSurface( Graphics& gfx ) const
	{
		INFOMAN( gfx );

		auto [pTexTemp, desc] = MakeStaging( gfx );

		if ( desc.Format != DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM )
		{
			throw std::runtime_error( "tosurface in RenderTarget on bad dxgi format" );
		}

		// create Surface and copy from temp texture to it
		const auto width = GetWidth();
		const auto height = GetHeight();
		Surface s{ width,height };
		D3D11_MAPPED_SUBRESOURCE msr = {};
		GFX_THROW_INFO( GetContext( gfx )->Map( pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr ) );
		auto pSrcBytes = static_cast<const char*>( msr.pData );
		for ( unsigned int y = 0; y < height; y++ )
		{
			auto pSrcRow = reinterpret_cast<const Surface::Color*>( pSrcBytes + msr.RowPitch * size_t( y ) );
			for ( unsigned int x = 0; x < width; x++ )
			{
				s.PutPixel( x, y, *( pSrcRow + x ) );
			}
		}
		GFX_THROW_INFO_ONLY( GetContext( gfx )->Unmap( pTexTemp.Get(), 0 ) );

		return s;
	}

	void Bind::RenderTarget::Dumpy( Graphics& gfx, const std::string& path ) const
	{
		INFOMAN( gfx );

		auto [pTexTemp, srcTextureDesc] = MakeStaging( gfx );

		// mapping texture and preparing vector
		const auto width = GetWidth();
		const auto height = GetHeight();
		std::vector<float> arr;
		arr.reserve( width * height );
		D3D11_MAPPED_SUBRESOURCE msr = {};
		GFX_THROW_INFO( GetContext( gfx )->Map( pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr ) );
		auto pSrcBytes = static_cast<const char*>( msr.pData );

		UINT nElements = 0;

		// flatten texture elements		
		if ( srcTextureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT )
		{
			nElements = 1;
			for ( unsigned int y = 0; y < height; y++ )
			{
				auto pSrcRow = reinterpret_cast<const float*>( pSrcBytes + msr.RowPitch * size_t( y ) );
				for ( unsigned int x = 0; x < width; x++ )
				{
					arr.push_back( pSrcRow[x] );
				}
			}
		}
		else if ( srcTextureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT )
		{
			nElements = 2;
			struct Element
			{
				float r;
				float g;
			};
			for ( unsigned int y = 0; y < height; y++ )
			{
				auto pSrcRow = reinterpret_cast<const Element*>( pSrcBytes + msr.RowPitch * size_t( y ) );
				for ( unsigned int x = 0; x < width; x++ )
				{
					arr.push_back( pSrcRow[x].r );
					arr.push_back( pSrcRow[x].g );
				}
			}
		}
		else
		{
			GFX_THROW_INFO_ONLY( GetContext( gfx )->Unmap( pTexTemp.Get(), 0 ) );
			throw std::runtime_error{ "Bad format in RenderTarget for dumpy" };
		}
		GFX_THROW_INFO_ONLY( GetContext( gfx )->Unmap( pTexTemp.Get(), 0 ) );

		// dump to numpy array
		cnpy::npy_save( path, arr.data(), { height,width,nElements } );
	}

	void RenderTarget::BindAsBuffer( Graphics& gfx ) noexcept
	{
		ID3D11DepthStencilView* const null = nullptr;
		BindAsBuffer( gfx, null );
	}

	void RenderTarget::BindAsBuffer( Graphics& gfx, BufferResource* depthStencil ) noexcept
	{
		assert( dynamic_cast<DepthStencil*>( depthStencil ) != nullptr );
		BindAsBuffer( gfx, static_cast<DepthStencil*>( depthStencil ) );
	}

	void RenderTarget::BindAsBuffer( Graphics& gfx, DepthStencil* depthStencil ) noexcept
	{
		BindAsBuffer( gfx, depthStencil ? depthStencil->pDepthStencilView.Get() : nullptr );
	}

	void RenderTarget::BindAsBuffer( Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView ) noexcept
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

	void RenderTarget::Clear( Graphics& gfx, const std::array<float, 4>& color ) noexcept
	{
		GetContext( gfx )->ClearRenderTargetView( pTargetView, color.data() );
	}

	void RenderTarget::Clear( Graphics& gfx ) noexcept
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


	ShaderInputRenderTarget::ShaderInputRenderTarget( Graphics& gfx, UINT width, UINT height, UINT slot )
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

	void ShaderInputRenderTarget::Bind( Graphics& gfx ) noexcept
	{
		GetContext( gfx )->PSSetShaderResources( slot, 1, &pShaderResourceView );
	}


	void OutputOnlyRenderTarget::Bind( Graphics& gfx ) noexcept
	{
		assert( "Cannot bind OuputOnlyRenderTarget as shader input" && false );
	}

	OutputOnlyRenderTarget::OutputOnlyRenderTarget( Graphics& gfx, ID3D11Texture2D* pTexture, std::optional<UINT> face )
		:
		RenderTarget( gfx, pTexture, face )
	{}

	GBufferRenderTargets::GBufferRenderTargets( Graphics& gfx, UINT width, UINT height, UINT slot )
		:
		RenderTarget( gfx, width, height ),
		slot( slot )
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		for ( int i = 0; i < bufferCount; i++ )
		{
			HRESULT result = GetDevice( gfx )->CreateTexture2D( &textureDesc, NULL, &m_pTextures[i] );
			if ( FAILED( result ) )
			{
				throw std::exception();
			}
		}

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

		for ( int i = 0; i < bufferCount; i++ )
		{
			HRESULT result = GetDevice( gfx )->CreateRenderTargetView( m_pTextures[i], &renderTargetViewDesc, &m_pGBuffers[i] );
			if ( FAILED( result ) )
			{
				throw std::exception();
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		for ( int i = 0; i < bufferCount; i++ )
		{
			HRESULT result = GetDevice( gfx )->CreateShaderResourceView( m_pTextures[i], &shaderResourceViewDesc, &m_pShaderResources[i] );
			if ( FAILED( result ) )
			{
				throw std::exception();
			}
		}
	}

	void GBufferRenderTargets::BindAsBuffer( Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView ) noexcept
	{
		GetContext( gfx )->OMSetRenderTargets( 3, &m_pGBuffers[0], pDepthStencilView );

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

	void GBufferRenderTargets::Bind( Graphics& gfx ) noexcept
	{
		GetContext( gfx )->PSSetShaderResources( slot, 3, &m_pShaderResources[0] );
	}

	void GBufferRenderTargets::Clear( Graphics& gfx, const std::array<float, 4>& color ) noexcept
	{
		GetContext( gfx )->ClearRenderTargetView( m_pGBuffers[0], color.data() );
		GetContext( gfx )->ClearRenderTargetView( m_pGBuffers[1], color.data() );
		GetContext( gfx )->ClearRenderTargetView( m_pGBuffers[2], color.data() );
	}

	void GBufferRenderTargets::Clear( Graphics& gfx ) noexcept
	{
		Clear( gfx, { 0.0f,0.0f,0.0f,0.0f } );
	}
}