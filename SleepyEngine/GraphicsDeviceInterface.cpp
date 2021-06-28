#include "GraphicsDeviceInterface.h"

GraphicsDeviceInterface::GraphicsDeviceInterface()
{}

void GraphicsDeviceInterface::InitializeGraphics( HWND& hWnd, GraphicsAPI api, u32 width, u32 height )
{
	m_D3D11Interface.Initialize( hWnd, width, height );
}

void GraphicsDeviceInterface::Draw()
{
	m_D3D11Interface.DrawTriangle();
}

bool GraphicsDeviceInterface::IsInitialized() noexcept
{
	if ( m_GraphicsApi == GraphicsAPI::Uninitialized )
		return false;
	return true;
}

void GraphicsDeviceInterface::BindCameraToGraphics()
{
	m_Camera.GetViewMatrix();
}

