#include "PixelShader.h"
#include "../BindableCodex.h"
#include <d3dcompiler.h>
#include "../../Utilities/ChiliUtil.h"

namespace Bind
{
	PixelShader::PixelShader( Graphics& gdi, const std::string& path )
		:
		m_sPath( path )
	{

		ID3DBlob* pBlob;
		D3DReadFileToBlob( ToWide( path ).c_str(), &pBlob );
		GetDevice( gdi )->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPixelShader );
	}
	void PixelShader::Bind( Graphics& gdi ) noexcept
	{
		GetContext( gdi )->PSSetShader( m_pPixelShader, nullptr, 0u );
	}
	std::shared_ptr<PixelShader> PixelShader::Resolve( Graphics& gdi, const std::string& path )
	{
		return Codex::Resolve<PixelShader>( gdi, path );
	}
	std::string PixelShader::GenerateUID( const std::string& path )
	{
		using namespace std::string_literals;
		return typeid( PixelShader ).name() + "#"s + path;
	}
	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID( m_sPath );
	}
	const std::string PixelShader::GetPath() const noexcept
	{
		return m_sPath;
	}
}
