#include "Rasterizer.h"
#include "../BindableCodex.h"

namespace Bind
{
	Rasterizer::Rasterizer( GraphicsDeviceInterface& gdi, bool twoSided )
		:
		m_bTwoSided( twoSided )
	{
		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC( CD3D11_DEFAULT{} );
		rasterDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

		GetDevice( gdi )->CreateRasterizerState( &rasterDesc, &m_pRasterizer );
	}

	void Rasterizer::Bind( GraphicsDeviceInterface& gdi ) noexcept
	{
		GetContext( gdi )->RSSetState( m_pRasterizer );
	}

	std::shared_ptr<Rasterizer> Rasterizer::Resolve( GraphicsDeviceInterface& gdi, bool twoSided )
	{
		return Codex::Resolve<Rasterizer>( gdi, twoSided );
	}
	std::string Rasterizer::GenerateUID( bool twoSided )
	{
		using namespace std::string_literals;
		return typeid( Rasterizer ).name() + "#"s + ( twoSided ? "2s" : "1s" );
	}
	std::string Rasterizer::GetUID() const noexcept
	{
		return GenerateUID( m_bTwoSided );
	}
}