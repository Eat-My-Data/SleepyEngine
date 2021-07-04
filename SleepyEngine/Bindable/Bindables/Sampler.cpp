#include "Sampler.h"
#include "../BindableCodex.h"

namespace Bind
{
	Sampler::Sampler( GraphicsDeviceInterface& gdi )
	{
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		GetDevice( gdi )->CreateSamplerState( &samplerDesc, &pSampler );
	}
	void Sampler::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		GetContext( gdi )->PSSetSamplers( 0, 1, &pSampler );
	}
	std::shared_ptr<Sampler> Sampler::Resolve( GraphicsDeviceInterface& gdi )
	{
		return Codex::Resolve<Sampler>( gdi );
	}
	std::string Sampler::GenerateUID()
	{
		return typeid( Sampler ).name();
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID();
	}
}
