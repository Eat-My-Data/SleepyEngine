#pragma once
#include <d3d11.h>
#include "NumericDataTypes.h"


class D3D11Interface
{
public:
	D3D11Interface();
	D3D11Interface( const D3D11Interface& ) = delete;
	D3D11Interface& operator=( const D3D11Interface& ) = delete;
	~D3D11Interface();
public:
	void Initialize( HWND& hWnd, u32 width, u32 height );
	void DrawTriangle();
private:
	IDXGISwapChain* m_pSwap;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
private:
	ID3D11RenderTargetView* m_pTarget;
};