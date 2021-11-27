#include "Sampler.h"
#include "../BindableCodex.h"

namespace Bind
{
	Sampler::Sampler( GraphicsDeviceInterface& gdi, bool anisoEnable, bool reflect )
		:
		anisoEnable( anisoEnable ),
		reflect( reflect ) 
	{
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = anisoEnable ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		GetDevice( gdi )->CreateSamplerState( &samplerDesc, &pSampler );
	}
	void Sampler::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		GetContext( gdi )->PSSetSamplers( 0, 1, &pSampler );
	}
	std::shared_ptr<Sampler> Sampler::Resolve( GraphicsDeviceInterface& gdi, bool anisoEnable, bool reflect )
	{
		return Codex::Resolve<Sampler>( gdi, anisoEnable, reflect );
	}
	std::string Sampler::GenerateUID( bool anisoEnable, bool reflect )
	{

		using namespace std::string_literals;
		return typeid( Sampler ).name() + "#"s + ( anisoEnable ? "A"s : "a"s ) + ( reflect ? "R"s : "W"s );
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID( anisoEnable, reflect );
	}
}
