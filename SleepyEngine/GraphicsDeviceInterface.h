#pragma once
#include "D3D11Interface.h"
#include "Camera.h"
#include "SleepyMath.h"

namespace Bind
{
	class Bindable;
}

enum class GraphicsAPI
{
	Uninitialized,
	DirectX,
	OpenGL,
	Vulkan
};

class GraphicsDeviceInterface
{
	friend class Bind::Bindable;
public:
	GraphicsDeviceInterface();
	~GraphicsDeviceInterface() = default;
public:
	void InitializeGraphics( HWND& hWnd, GraphicsAPI api, u32 width, u32 height );
	bool IsInitialized() noexcept;
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	ID3D11RenderTargetView* GetTarget();
	void DrawIndexed( UINT count ) noexcept;
private:
	D3D11Interface m_D3D11Interface;
	GraphicsAPI m_GraphicsApi = GraphicsAPI::Uninitialized;
	Camera m_Camera = { L"Character Camera", MatrixType::Perspective, ViewSpace( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), DirectX::XMFLOAT3{ -13.5f, 0.0f, 3.5f }, 0.0f, PI / 2.0f };
};