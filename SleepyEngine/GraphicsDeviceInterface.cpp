#include "GraphicsDeviceInterface.h"

GraphicsDeviceInterface::GraphicsDeviceInterface()
{}

void GraphicsDeviceInterface::InitializeGraphics( HWND& hWnd, GraphicsAPI api, u32 width, u32 height )
{
	m_GraphicsAPI = api;

	if ( api == GraphicsAPI::DirectX )
		m_D3D11Interface.Initialize( hWnd, width, height );
}

void GraphicsDeviceInterface::DrawIndexed( UINT count ) noexcept
{
	m_D3D11Interface.GetContext()->DrawIndexed( count, 0u, 0u );
}

void GraphicsDeviceInterface::SetViewMatrix( DirectX::XMMATRIX viewMatrix ) noexcept
{
	m_ViewMatrix = viewMatrix;
}

void GraphicsDeviceInterface::SetProjMatrix( DirectX::XMMATRIX projMatrix ) noexcept
{
	m_ProjMatrix = projMatrix;
}

DirectX::XMMATRIX GraphicsDeviceInterface::GetViewMatrix() noexcept
{
	return m_ViewMatrix;
}

DirectX::XMMATRIX GraphicsDeviceInterface::GetProjMatrix() noexcept
{
	return m_ProjMatrix;
}

bool GraphicsDeviceInterface::IsInitialized() noexcept
{
	return  m_GraphicsAPI != GraphicsAPI::Uninitialized;
}

IDXGISwapChain* GraphicsDeviceInterface::GetSwap() noexcept
{
	return m_D3D11Interface.GetSwap();
}

ID3D11Device* GraphicsDeviceInterface::GetDevice() noexcept
{
	return m_D3D11Interface.GetDevice();
}

ID3D11DeviceContext* GraphicsDeviceInterface::GetContext() noexcept
{
	return m_D3D11Interface.GetContext();
}

ID3D11RenderTargetView** GraphicsDeviceInterface::GetTarget() noexcept
{
	return m_D3D11Interface.GetTarget();
}

// TODO: BEGIN FRAME
//	float color[4] = { 0.0f,1.0,1.0,1.0f };
//	gdi.GetContext()->ClearRenderTargetView( gdi.GetTarget(), color );
