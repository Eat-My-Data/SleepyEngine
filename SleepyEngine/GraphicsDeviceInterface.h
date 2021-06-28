#pragma once
#include "D3D11Interface.h"
#include "Camera.h"

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
	~GraphicsDeviceInterface() = default;
public:
	void InitializeGraphics( HWND& hWnd, GraphicsAPI api, u32 width, u32 height );
	bool IsInitialized() noexcept;

	void BindCameraToGraphics();
	void Draw();
	
private:
	D3D11Interface m_D3D11Interface;
	GraphicsAPI m_GraphicsApi = GraphicsAPI::Uninitialized;
	Camera m_Camera = { L"Character Camera", MatrixType::Perspective, ViewSpace( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), DirectX::XMFLOAT3{ -13.5f, 6.0f, 3.5f }, 0.0f, 3.14159265f / 2.0f };
};