#include "D3D11Interface.h"
#include "d3dcompiler.h"

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

    // swap chain description
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
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

    // create swap chain/device/context
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

    // gain access to texture subresource in swap chain (back buffer)
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

    //m_pContext->OMSetRenderTargets(1u, &m_pTarget, nullptr);

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_pContext->RSSetViewports( 1u, &vp );
}

void D3D11Interface::DrawTriangle()
{
    HRESULT hr;

    float color[4] = { 0.0f,1.0,1.0,1.0f };
    m_pContext->ClearRenderTargetView( m_pTarget, color );

    struct Vertex
    {
        float x;
        float y;
    };

    // create vertex buffer (1 2d triangle at center of screen)
    const Vertex vertices[] =
    {
        { 0.0f,0.5f },
        { 0.5f,-0.5f },
        { -0.5f,-0.5f }
    };

    ID3D11Buffer* pVertexBuffer;
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof( vertices );
    bd.StructureByteStride = sizeof( Vertex );
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    if ( FAILED( m_pDevice->CreateBuffer( &bd, &sd, &pVertexBuffer ) ) )
    {
        throw std::exception();
    }

    // Bind vertex buffer to pipeline
    const UINT stride = sizeof( Vertex );
    const UINT offset = 0u;
    m_pContext->IASetVertexBuffers( 0u, 1u, &pVertexBuffer, &stride, &offset );

    // create pixel shader
    ID3D11PixelShader* pPixelShader;
    ID3DBlob* pBlob;
    if ( FAILED( hr = D3DReadFileToBlob( L"PixelShader.cso", &pBlob ) ) )
    {
        throw std::exception();
    }
    if ( FAILED( hr = m_pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader ) ) )
    {
        throw std::exception();
    }

    // bind pixel shader
    m_pContext->PSSetShader( pPixelShader, nullptr, 0u );

    // create vertex shader
    ID3D11VertexShader* pVertexShader;
    if ( FAILED( hr = D3DReadFileToBlob( L"VertexShader.cso", &pBlob ) ) )
    {
        throw std::exception();
    }
    if ( FAILED( hr = m_pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader ) ) )
    {
        throw std::exception();
    }

    // bind vertex shader
    m_pContext->VSSetShader( pVertexShader, nullptr, 0u );

    // input (vertex) layout (2d position only)
    ID3D11InputLayout* pInputLayout;
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
    };
    if ( FAILED( hr = m_pDevice->CreateInputLayout( ied, (UINT)std::size( ied ), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout ) ) )
    {
        throw std::exception();
    }

    // bind vertex layout
    m_pContext->IASetInputLayout( pInputLayout );

    // bind render target
    m_pContext->OMSetRenderTargets( 1u, &m_pTarget, nullptr );

    // Set primitive topology to triangle list ( groups of 3 vertices )
    m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // draw to screen
    m_pContext->Draw( (UINT)std::size( vertices ), 0u );

    if ( FAILED( hr = m_pSwap->Present( 1u, 0u ) ) )
    {
        throw std::exception();
    }
}
