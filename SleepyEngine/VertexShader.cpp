#include "VertexShader.h"
#include "BindableCodex.h"
#include <d3dcompiler.h>
#include "SleepyUtil.h"

namespace Bind
{
	VertexShader::VertexShader( GraphicsDeviceInterface& gdi,const std::string& path )
		:
		path( path )
	{
		D3DReadFileToBlob( ToWide( path ).c_str(), &pBytecodeBlob );
		GetDevice( gdi )->CreateVertexShader(
			pBytecodeBlob->GetBufferPointer(),
			pBytecodeBlob->GetBufferSize(),
			nullptr,
			&pVertexShader
		);
	}
	void VertexShader::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		GetContext( gdi )->VSSetShader( pVertexShader, nullptr, 0u );
	}
	ID3DBlob* VertexShader::GetBytecode() const noexcept
	{
		return pBytecodeBlob;
	}
	std::shared_ptr<VertexShader> VertexShader::Resolve( GraphicsDeviceInterface& gdi, const std::string& path)
	{
		return Codex::Resolve<VertexShader>( gdi,path );
	}
	std::string VertexShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}
	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID( path );
	}
}
