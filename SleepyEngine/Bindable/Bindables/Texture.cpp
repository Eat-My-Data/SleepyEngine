#include "Texture.h"
#include "../../Renderer/Surface.h"
#include "../../Macros/GraphicsThrowMacros.h"
#include "../BindableCodex.h"

namespace Bind
{
	Texture::Texture( Graphics& gdi, const std::string& path, UINT slot )
		:
		slot( slot )
	{

		// load surface
		const auto s = Surface::FromFile( path );
		hasAlpha = s.AlphaLoaded();

		// create texture resource
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		ID3D11Texture2D* pTexture;
		GetDevice( gdi )->CreateTexture2D(
			&textureDesc, nullptr, &pTexture
		);

		// write image data into top mip level
		GetContext( gdi )->UpdateSubresource(
			pTexture, 0u, nullptr, s.GetBufferPtrConst(), s.GetWidth() * sizeof( Surface::Color ), 0u
		);

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		 GetDevice( gdi )->CreateShaderResourceView(
			pTexture, &srvDesc, &pTextureView
		);

		// generate the mip chain using the gpu rendering pipeline
		GetContext( gdi )->GenerateMips( pTextureView );
	}
	void Texture::Bind( Graphics& gdi ) noexcept
	{
		GetContext( gdi )->PSSetShaderResources( slot, 1u, &pTextureView );
	}
	std::shared_ptr<Texture> Texture::Resolve( Graphics& gdi, const std::string& path, UINT slot )
	{
		return Codex::Resolve<Texture>( gdi, path, slot );
	}
	std::string Texture::GenerateUID( const std::string& path, UINT slot )
	{
		using namespace std::string_literals;
		return typeid( Texture ).name() + "#"s + path + "#" + std::to_string( slot );
	}
	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID( path, slot );
	}
	bool Texture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}
#undef max
	UINT Texture::CalculateNumberOfMipLevels( UINT width, UINT height ) noexcept
	{
		const float xSteps = std::ceil( log2( (float)width ) );
		const float ySteps = std::ceil( log2( (float)height ) );
		return (UINT)std::max( xSteps, ySteps );
	}

	DepthTexture::DepthTexture( Graphics& gfx, UINT size, UINT slot )
		:
		slot( slot )
	{
		INFOMAN( gfx );

		// texture descriptor
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = size;
		textureDesc.Height = size;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		// create the texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D(
			&textureDesc, nullptr, &pTexture
		) );


		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GetDevice( gfx )->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);

		depthBuffer = std::make_shared<OutputOnlyDepthStencil>( gfx, pTexture );
	}
	void DepthTexture::Bind( Graphics& gfx )
	{

		INFOMAN_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShaderResources( slot, 1u, &pTextureView ) );
		//GetContext( gdi )->PSSetShaderResources( slot, 1u, &pTextureView );
	}
	std::shared_ptr<DepthTexture> DepthTexture::Resolve( Graphics& gdi, const std::string& path, UINT slot )
	{
		return Codex::Resolve<DepthTexture>( gdi, path, slot );
	}
	std::string DepthTexture::GenerateUID( const std::string& path, UINT slot )
	{
		using namespace std::string_literals;
		return typeid( DepthTexture ).name() + "#"s + path + "#" + std::to_string( slot );
	}
	std::string DepthTexture::GetUID() const noexcept
	{
		return GenerateUID( "depthTexture", slot);
	}
	bool DepthTexture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}
	std::shared_ptr<OutputOnlyDepthStencil> DepthTexture::GetDepthBuffer() const
	{
		return depthBuffer;
	}
#undef max
	UINT DepthTexture::CalculateNumberOfMipLevels( UINT width, UINT height ) noexcept
	{
		const float xSteps = std::ceil( log2( (float)width ) );
		const float ySteps = std::ceil( log2( (float)height ) );
		return (UINT)std::max( xSteps, ySteps );
	}
}