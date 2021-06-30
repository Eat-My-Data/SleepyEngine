#include "PixelShader.h"
#include "BindableCodex.h"
#include <d3dcompiler.h>
#include "SleepyUtil.h"

namespace Bind
{
	PixelShader::PixelShader( GraphicsDeviceInterface& gfx, const std::string& path )
		:
		path( path )
	{

		ID3DBlob* pBlob;
		D3DReadFileToBlob( ToWide( path ).c_str(), &pBlob );
		GetDevice( gfx )->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader );
	}
	void PixelShader::Bind( GraphicsDeviceInterface& gfx ) noexcept
	{
		GetContext( gfx )->PSSetShader( pPixelShader, nullptr, 0u );
	}
	std::shared_ptr<PixelShader> PixelShader::Resolve( GraphicsDeviceInterface& gfx, const std::string& path )
	{
		return Codex::Resolve<PixelShader>( gfx, path );
	}
	std::string PixelShader::GenerateUID( const std::string& path )
	{
		using namespace std::string_literals;
		return typeid( PixelShader ).name() + "#"s + path;
	}
	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID( path );
	}
}
