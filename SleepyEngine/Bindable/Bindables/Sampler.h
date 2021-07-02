#pragma once
#include "../Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler( GraphicsDeviceInterface& gdi );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		static std::shared_ptr<Sampler> Resolve( GraphicsDeviceInterface& gfx );
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;
	protected:
		ID3D11SamplerState* pSampler;
	};
}
