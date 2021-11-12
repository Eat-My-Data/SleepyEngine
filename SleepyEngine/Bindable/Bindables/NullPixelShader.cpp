#include "NullPixelShader.h"
#include "../BindableCodex.h"

namespace Bind
{
	NullPixelShader::NullPixelShader( GraphicsDeviceInterface& gfx )
	{
	}
	void NullPixelShader::Bind( GraphicsDeviceInterface& gfx ) noexcept
	{
		GetContext( gfx )->PSSetShader( nullptr, nullptr, 0u );
	}
	std::shared_ptr<NullPixelShader> NullPixelShader::Resolve( GraphicsDeviceInterface& gfx )
	{
		return Codex::Resolve<NullPixelShader>( gfx );
	}
	std::string NullPixelShader::GenerateUID()
	{
		return typeid( NullPixelShader ).name();
	}
	std::string NullPixelShader::GetUID() const noexcept
	{
		return GenerateUID();
	}
}