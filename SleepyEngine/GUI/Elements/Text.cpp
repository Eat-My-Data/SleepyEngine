#include "Text.h"
#include "../Forms/Rectangle.h"
#include "../../Bindable/BindableCommon.h"

Text::Text( f32 x, f32 y, f32 width, f32 height )
	: GUIElement( x, y, width, height )
{}

void Text::Draw( GraphicsDeviceInterface & gdi )
{
	// can add in different draw states based on wheter
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Rectangle::Make( (f32)m_iX - 640.0f, (f32)m_iY + 360.0f - (f32)m_iHeight, (f32)m_iWidth, (f32)m_iHeight );

	VertexBuffer::VertexBuffer( gdi, model.m_VBVertices ).Bind( gdi );
	auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/PhongVS.cso" );
	pvs->Bind( gdi );
	auto pvsbc = pvs->GetBytecode();
	PixelShader::PixelShader( gdi, "./Shaders/Bin/PhongPS.cso" ).Bind( gdi );
	//Bind::PixelConstantBuffer<ChooseRenderTechnique::ColorConst>::PixelConstantBuffer( gdi, color, 1u ).Bind( gdi );
	InputLayout::InputLayout( gdi, model.m_VBVertices.GetLayout(), pvsbc ).Bind( gdi );
	Topology::Topology( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ).Bind( gdi );
	Rasterizer::Rasterizer( gdi, true ).Bind( gdi );
	gdi.GetContext()->Draw( 6u, 0u );
}
