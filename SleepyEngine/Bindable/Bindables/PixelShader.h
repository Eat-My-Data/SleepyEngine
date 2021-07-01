#pragma once
#include "../Bindable.h"

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		PixelShader( GraphicsDeviceInterface& gdi, const std::string& path );
		void Bind( GraphicsDeviceInterface& gdi ) noexcept override;
		static std::shared_ptr<PixelShader> Resolve( GraphicsDeviceInterface& gdi, const std::string& path );
		static std::string GenerateUID( const std::string& path );
		std::string GetUID() const noexcept override;
	protected:
		std::string m_sPath;
		ID3D11PixelShader* m_pPixelShader;
	};
}
