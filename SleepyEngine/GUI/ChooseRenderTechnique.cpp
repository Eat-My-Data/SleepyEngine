#include "ChooseRenderTechnique.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../SceneManager/SceneManager.h"
#include "./Forms/Rectangle.h"
#include "../Bindable/BindableCommon.h"

ChooseRenderTechnique::ChooseRenderTechnique( f32 x, f32 y, f32 width, f32 height )
	: GUIElement( x,  y, width, height )
{}

void ChooseRenderTechnique::Draw( GraphicsDeviceInterface& gdi )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Rectangle::Make( (f32)m_iX, (f32)m_iY, (f32)m_iWidth, (f32)m_iHeight );

	VertexBuffer::VertexBuffer( gdi, model.m_VBVertices ).Bind( gdi );
	auto pvs = VertexShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/PhongVS.cso" );
	pvs->Bind( gdi );
	auto pvsbc = pvs->GetBytecode();
	PixelShader::PixelShader( gdi, "../SleepyEngine/Shaders/Bin/PhongPS.cso" ).Bind( gdi );
	Bind::PixelConstantBuffer<ChooseRenderTechnique::ColorConst>::PixelConstantBuffer( gdi, color, 1u ).Bind( gdi );
	InputLayout::InputLayout( gdi, model.m_VBVertices.GetLayout(), pvsbc ).Bind( gdi );
	Topology::Topology( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ).Bind( gdi );
	Rasterizer::Rasterizer( gdi, true ).Bind( gdi );

	gdi.GetContext()->Draw( 6u, 0u );
}

void ChooseRenderTechnique::Interact( SceneManager& sceneManager )
{
	if ( isForwardRender )
	{
		color.color = { 0.0f, 1.0f, 0.0f, 1.0f };
		sceneManager.SetRenderTechnique( RenderTechnique::Forward );
	}	
	else
	{
		color.color = { 0.0f, 0.0f, 1.0f, 1.0f };
		sceneManager.SetRenderTechnique( RenderTechnique::Deferred );
	}
}
