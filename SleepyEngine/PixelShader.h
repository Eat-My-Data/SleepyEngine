#pragma once
#include "Bindable.h"

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		PixelShader( GraphicsDeviceInterface& gfx, const std::string& path );
		void Bind( GraphicsDeviceInterface& gfx ) noexcept override;
		static std::shared_ptr<PixelShader> Resolve( GraphicsDeviceInterface& gfx, const std::string& path );
		static std::string GenerateUID( const std::string& path );
		std::string GetUID() const noexcept override;
	protected:
		std::string path;
		ID3D11PixelShader* pPixelShader;
	};
}
