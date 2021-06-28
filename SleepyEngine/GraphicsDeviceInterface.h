#pragma once
#include "D3D11Interface.h"

enum class GraphicsAPI
{
	Uninitialized,
	DirectX,
	OpenGL,
	Vulkan
};

class GraphicsDeviceInterface
{
public:
	GraphicsDeviceInterface();
	void InitializeGraphics( HWND& hWnd, GraphicsAPI api, u32 width, u32 height );
	void Draw();
	bool IsInitialized() noexcept;
private:
	D3D11Interface m_D3D11Interface;
	GraphicsAPI m_GraphicsApi = GraphicsAPI::Uninitialized;
};