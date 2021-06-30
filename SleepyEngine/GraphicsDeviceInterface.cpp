#include "GraphicsDeviceInterface.h"

GraphicsDeviceInterface::GraphicsDeviceInterface()
{}

void GraphicsDeviceInterface::InitializeGraphics( HWND& hWnd, GraphicsAPI api, u32 width, u32 height )
{
	m_D3D11Interface.Initialize( hWnd, width, height );
}

void GraphicsDeviceInterface::Draw()
{
	m_D3D11Interface.DrawCube( m_Camera.GetViewMatrix() * m_Camera.GetProjectionMatrix() );
}

bool GraphicsDeviceInterface::IsInitialized() noexcept
{
	if ( m_GraphicsApi == GraphicsAPI::Uninitialized )
		return false;
	return true;
}

void GraphicsDeviceInterface::BindCameraToGraphics()
{
	// TODO: Once Scene Manager is implemented, I will likely need this function
}

u32 count = 0;
bool flag = true;
void GraphicsDeviceInterface::UpdateCamera()
{
	if ( flag )
	{
		m_Camera.Rotate( -1.0f, 1.0f );
		m_Camera.Translate( { -0.02f, 0.0f, 0.0f } );
		count++;
	}
	else
	{
		m_Camera.Rotate( 1.0f, -1.0f );
		m_Camera.Translate( { 0.02f, 0.0f, 0.0f } );
		count--;
	}
	
	if ( count > 30 )
	{
		flag = false;
	}
	
	if ( count < 1 )
	{
		flag = true;
	}
}

