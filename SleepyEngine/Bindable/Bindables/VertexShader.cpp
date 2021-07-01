#include "VertexShader.h"
#include "../BindableCodex.h"
#include <d3dcompiler.h>
#include "../../Utilities/SleepyUtil.h"

namespace Bind
{
	VertexShader::VertexShader( GraphicsDeviceInterface& gdi,const std::string& path )
		:
		m_sPath( path )
	{
		D3DReadFileToBlob( ToWide( path ).c_str(), &m_pBytecodeBlob );
		GetDevice( gdi )->CreateVertexShader(
			m_pBytecodeBlob->GetBufferPointer(),
			m_pBytecodeBlob->GetBufferSize(),
			nullptr,
			&m_pVertexShader
		);
	}
	void VertexShader::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		GetContext( gdi )->VSSetShader( m_pVertexShader, nullptr, 0u );
	}
	ID3DBlob* VertexShader::GetBytecode() const noexcept
	{
		return m_pBytecodeBlob;
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
		return GenerateUID( m_sPath );
	}
}

