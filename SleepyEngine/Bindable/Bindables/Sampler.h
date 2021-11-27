#pragma once
#include "../Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler( GraphicsDeviceInterface& gdi, bool anisoEnable, bool reflect );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		static std::shared_ptr<Sampler> Resolve( GraphicsDeviceInterface& gdi, bool anisoEnable = true, bool reflect = false );
		static std::string GenerateUID( bool anisoEnable, bool reflect );
		std::string GetUID() const noexcept override;
	protected:
		ID3D11SamplerState* pSampler;
		bool anisoEnable;
		bool reflect;
	};
}
