#pragma once
#include <d3d11.h>
#include "../Utilities/NumericDataTypes.h"
#include <DirectXMath.h>


class D3D11Interface
{
public:
	D3D11Interface();
	D3D11Interface( const D3D11Interface& ) = delete;
	D3D11Interface& operator=( const D3D11Interface& ) = delete;
	~D3D11Interface();
public:
	void Initialize( HWND& hWnd, u32 width, u32 height );
	IDXGISwapChain* GetSwap() noexcept;
	ID3D11Device* GetDevice() noexcept;
	ID3D11DeviceContext* GetContext() noexcept;
	ID3D11RenderTargetView** GetTarget() noexcept;
	ID3D11DepthStencilView** GetDSV() noexcept;
public:
	ID3D11RenderTargetView** GetGBuffers() noexcept;
	ID3D11ShaderResourceView** GetShaderResources() noexcept;
	ID3D11ShaderResourceView** GetDepthResource() noexcept;
	ID3D11BlendState* GetBlendState() noexcept;
	ID3D11DepthStencilView* GetDSV_ReadOnly() noexcept;
	ID3D11DepthStencilState* GetLightDSS() noexcept;
private:
	IDXGISwapChain* m_pSwap;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	ID3D11RenderTargetView* m_pTarget;
	ID3D11DepthStencilView* m_pDSV;
private:
	static const int bufferCount = 3;
	ID3D11RenderTargetView* m_pGBuffers[bufferCount];
	ID3D11Texture2D* m_pTextures[bufferCount];
	ID3D11ShaderResourceView* m_pShaderResources[bufferCount];
	ID3D11ShaderResourceView* m_pDepthResource;
private:
	ID3D11BlendState* m_pBlendState;
	ID3D11DepthStencilState* m_pDSSGBuffer;
	ID3D11DepthStencilState* m_pDSSLighting;
	ID3D11DepthStencilView* m_pDSV_ReadOnly;
};