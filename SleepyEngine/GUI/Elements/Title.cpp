#include "Title.h"
#include "../Forms/Rectangle.h"
#include "../../Bindable/BindableCommon.h"

Title::Title( char* title )
{
	m_iWidth = 100;
	m_iHeight = 20;
}

void Title::Draw()
{
	// can add in different draw states based on wheter
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Rectangle::Make( (f32)0 - 640.0f, (f32)0 + 360.0f - (f32)m_iHeight, (f32)m_iWidth, (f32)m_iHeight );

	//VertexBuffer::VertexBuffer( gdi, model.m_VBVertices ).Bind( gdi );
	//auto pvs = VertexShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/PhongVS.cso" );
	//pvs->Bind( gdi );
	//auto pvsbc = pvs->GetBytecode();
	//PixelShader::PixelShader( gdi, "../SleepyEngine/Shaders/Bin/PhongPS.cso" ).Bind( gdi );
	//Bind::PixelConstantBuffer<ChooseRenderTechnique::ColorConst>::PixelConstantBuffer( gdi, color, 1u ).Bind( gdi );
	//InputLayout::InputLayout( gdi, model.m_VBVertices.GetLayout(), pvsbc ).Bind( gdi );
	//Topology::Topology( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ).Bind( gdi );
	//Rasterizer::Rasterizer( gdi, true ).Bind( gdi );

	//gdi.GetContext()->Draw( 6u, 0u )
}
