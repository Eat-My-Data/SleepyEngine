#pragma once
#include "D3D11Interface.h"
#include "../SleepyMath.h"

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
	void DrawIndexed( UINT count ) noexcept;
public:
	void SetViewMatrix( DirectX::XMMATRIX viewMatrix ) noexcept;
	void SetProjMatrix( DirectX::XMMATRIX projMatrix ) noexcept;
	DirectX::XMMATRIX GetViewMatrix() noexcept;
	DirectX::XMMATRIX GetProjMatrix() noexcept;
public:
	IDXGISwapChain* GetSwap() noexcept;
	ID3D11Device* GetDevice() noexcept;
	ID3D11DeviceContext* GetContext() noexcept;
	ID3D11RenderTargetView** GetTarget() noexcept;
private:
	D3D11Interface m_D3D11Interface;
	GraphicsAPI m_GraphicsAPI = GraphicsAPI::Uninitialized;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjMatrix;
};