#include "D3D11Interface.h"
#include "d3dcompiler.h"
#include <d3dcompiler.h>

#include <exception>
#include <sstream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

D3D11Interface::D3D11Interface()
{}

D3D11Interface::~D3D11Interface()
{
    if ( m_pSwap != nullptr )
        m_pSwap->Release();

    if ( m_pDevice != nullptr )
        m_pDevice->Release();

    if ( m_pContext != nullptr )
        m_pContext->Release();

    if ( m_pTarget != nullptr )
        m_pTarget->Release();
}

void D3D11Interface::Initialize( HWND& hWnd, u32 width, u32 height )
{
    HRESULT hr;

    DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    if ( FAILED( hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        swapCreateFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &m_pSwap,
        &m_pDevice,
        nullptr,
        &m_pContext ) ) )
    {
        throw std::exception();
    }

    ID3D11Resource* pBackBuffer = nullptr;
    if ( FAILED( hr = m_pSwap->GetBuffer(
        0,
        __uuidof( ID3D11Resource ),
        (void**)&pBackBuffer ) ) )
    {
        throw std::exception();
    }

    if ( FAILED( hr = m_pDevice->CreateRenderTargetView(
        pBackBuffer,
        nullptr,
        &m_pTarget ) ) )
    {
        throw std::exception();
    }

    m_pContext->OMSetRenderTargets( 1u, &m_pTarget, nullptr );
    
    D3D11_VIEWPORT vp;
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_pContext->RSSetViewports( 1u, &vp );
}

IDXGISwapChain* D3D11Interface::GetSwap() noexcept
{
    return m_pSwap;
}

ID3D11Device* D3D11Interface::GetDevice() noexcept
{
    return m_pDevice;
}

ID3D11DeviceContext* D3D11Interface::GetContext() noexcept
{
    return m_pContext;
}

ID3D11RenderTargetView** D3D11Interface::GetTarget() noexcept
{
    return &m_pTarget;
}
