#pragma once
#include "D3D11Interface.h"
#include "../Utilities/SleepyMath.h"
#include <memory>

namespace Bind
{
	class Bindable;
	class RenderTarget;
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
	friend class GraphicsResource;
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
	ID3D11RenderTargetView** GetTargetDeprecated() noexcept;
	ID3D11DepthStencilView** GetDSV() noexcept;
public:
	ID3D11RenderTargetView** GetGBuffers() noexcept;
	ID3D11ShaderResourceView** GetShaderResources() noexcept;
	ID3D11ShaderResourceView** GetDepthResource() noexcept;
	ID3D11ShaderResourceView** GetShadowResource() noexcept;
	ID3D11BlendState* GetBlendState();
	ID3D11DepthStencilView** GetDSV_ReadOnly() noexcept;
	ID3D11DepthStencilState* GetLightDSS() noexcept;
	ID3D11DepthStencilState* GetBufferDSS() noexcept;
	ID3D11DepthStencilState* GetBufferDSS2() noexcept;
	ID3D11DepthStencilView** GetShadowDSV() noexcept;
	ID3D11DepthStencilView** GetShadowDSV2() noexcept;
	ID3D11ShaderResourceView** GetShadowResource2() noexcept;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	std::shared_ptr<Bind::RenderTarget> GetTarget();
private:
	std::shared_ptr<Bind::RenderTarget> pTarget;
private:
	UINT m_iWidth;
	UINT m_iHeight;	D3D11Interface m_D3D11Interface;
	GraphicsAPI m_GraphicsAPI = GraphicsAPI::Uninitialized;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjMatrix;
};