#include "ChooseRenderTechnique.h"
#include "../../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../../SceneManager/SceneManager.h"
#include "../Forms/Rectangle.h"
#include "../../Bindable/BindableCommon.h"
#include "../../Win32/Mouse.h"

bool ChooseRenderTechnique::DoElement( class GraphicsDeviceInterface& gdi, UI_ID& active, UI_ID& hot, Mouse& mouse )
{
	//// chack whether the button should be hot
	//if ( Inside( mouse.GetPos() ) )
	//{
	//	hot.Name = m_sName;
	//	if ( active.Name == "No Active Elements" && mouse.LeftIsPressed() )
	//		active.Name = m_sName;
	//}

	//// render button
	//Draw( gdi );

	//// if button is hot and active, but mouse button is not
	//// down, the user must have clicked the button.
	//if ( mouse.LeftIsReleased() && active.Name == m_sName && hot.Name == m_sName )
	//	return true;

	return false;
}

void ChooseRenderTechnique::Draw( GraphicsDeviceInterface& gdi )
{
	// can add in different draw states based on wheter
	using namespace Bind;
	namespace dx = DirectX;

	//auto model = Rectangle::Make( (f32)m_iX - 640.0f, (f32)m_iY + 360.0f - (f32)m_iHeight, (f32)m_iWidth, (f32)m_iHeight );

	//VertexBuffer::VertexBuffer( gdi, model.m_VBVertices ).Bind( gdi );
	//auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/PhongVS.cso" );
	//pvs->Bind( gdi );
	//auto pvsbc = pvs->GetBytecode();
	//PixelShader::PixelShader( gdi, "./Shaders/Bin/PhongPS.cso" ).Bind( gdi );
	//Bind::PixelConstantBuffer<ChooseRenderTechnique::ColorConst>::PixelConstantBuffer( gdi, color, 1u ).Bind( gdi );
	//InputLayout::InputLayout( gdi, model.m_VBVertices.GetLayout(), pvsbc ).Bind( gdi );
	//Topology::Topology( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ).Bind( gdi );
	//Rasterizer::Rasterizer( gdi, true ).Bind( gdi );
	//Texture::Texture( gdi, "./GUI/Text/font_01.jpg",2u ).Bind( gdi );
	//gdi.GetContext()->Draw( 6u, 0u );
}

void ChooseRenderTechnique::Interact( SceneManager& sceneManager )
{
	if ( isForwardRender )
	{
		color.color = { 0.0f, 1.0f, 0.0f, 1.0f };
		sceneManager.SetRenderTechnique( RenderTechnique::Forward );
		isForwardRender = false;
	}	
	else
	{
		color.color = { 0.0f, 0.0f, 1.0f, 1.0f };
		sceneManager.SetRenderTechnique( RenderTechnique::Deferred );
		isForwardRender = true;
	}
}
//
//bool ChooseRenderTechnique::Inside( std::pair<u32, u32> mousePos )
//{
//	//return  ( m_iX < mousePos.first && mousePos.first < m_iX + m_iWidth )
//	//	 && ( m_iY < mousePos.second && mousePos.second < m_iY + m_iHeight );
//}
