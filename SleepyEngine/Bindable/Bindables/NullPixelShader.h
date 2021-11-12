#pragma once
#include "../Bindable.h"

namespace Bind
{
	class NullPixelShader : public Bindable
	{
	public:
		NullPixelShader( GraphicsDeviceInterface& gfx );
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
		static std::shared_ptr<NullPixelShader> Resolve( GraphicsDeviceInterface& gfx );
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;
	};
}