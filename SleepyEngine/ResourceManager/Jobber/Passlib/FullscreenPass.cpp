#include "FullscreenPass.h"
#include "../../../Bindable/BindableCommon.h"


namespace dx = DirectX;

FullscreenPass::FullscreenPass( const std::string name,GraphicsDeviceInterface& gfx ) noexcept
	:
	BindingPass( std::move( name ) )
{
	// setup fullscreen geometry
	Dvtx::VertexLayout lay;
	lay.Append( Dvtx::VertexLayout::Position2D );
	Dvtx::VertexBuffer bufFull{ lay };
	bufFull.EmplaceBack( dx::XMFLOAT2{ -1,1 } );
	bufFull.EmplaceBack( dx::XMFLOAT2{ 1,1 } );
	bufFull.EmplaceBack( dx::XMFLOAT2{ -1,-1 } );
	bufFull.EmplaceBack( dx::XMFLOAT2{ 1,-1 } );
	AddBind( Bind::VertexBuffer::Resolve( gfx, "$Full", std::move( bufFull ) ) );
	std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
	AddBind( Bind::IndexBuffer::Resolve( gfx, "$Full", std::move( indices ) ) );
	// setup other common fullscreen bindables
	auto vs = Bind::VertexShader::Resolve( gfx, "Fullscreen_VS.cso" );
	AddBind( Bind::InputLayout::Resolve( gfx, lay, vs->GetBytecode() ) );
	AddBind( std::move( vs ) );
	AddBind( Bind::Topology::Resolve( gfx ) );
	AddBind( Bind::Rasterizer::Resolve( gfx, false ) );
}

void FullscreenPass::Execute( GraphicsDeviceInterface& gfx ) const noexcept
{
	BindAll( gfx );
	gfx.DrawIndexed( 6u );
}