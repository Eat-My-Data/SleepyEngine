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

void D3D11Interface::DrawCube( DirectX::XMMATRIX projectionMatrix )
{
    float color[4] = { 0.0f,1.0,1.0,1.0f };
    m_pContext->ClearRenderTargetView( m_pTarget, color );

    struct Vertex
    {
        struct
        {
            float x;
            float y;
            float z;
        } pos;
    };

    // create vertex buffer (1 2d triangle at center of screen)
    Vertex vertices[] =
    {
        { -1.0f,-1.0f,-1.0f	 },
        { 1.0f,-1.0f,-1.0f	 },
        { -1.0f,1.0f,-1.0f	 },
        { 1.0f,1.0f,-1.0f	  },
        { -1.0f,-1.0f,1.0f	 },
        { 1.0f,-1.0f,1.0f	  },
        { -1.0f,1.0f,1.0f	 },
        { 1.0f,1.0f,1.0f	 },
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
    m_pDevice->CreateBuffer( &bd, &sd, &pVertexBuffer );

    // Bind vertex buffer to pipeline
    const UINT stride = sizeof( Vertex );
    const UINT offset = 0u;
    m_pContext->IASetVertexBuffers( 0u, 1u, &pVertexBuffer, &stride, &offset );


    // create index buffer
    const unsigned short indices[] =
    {
        0,2,1, 2,3,1,
        1,3,5, 3,7,5,
        2,6,3, 3,6,7,
        4,5,7, 4,7,6,
        0,4,2, 2,4,6,
        0,1,4, 1,5,4
    };
    ID3D11Buffer* pIndexBuffer;
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof( indices );
    ibd.StructureByteStride = sizeof( unsigned short );
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    m_pDevice->CreateBuffer( &ibd, &isd, &pIndexBuffer );

    // bind index buffer
    m_pContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u );


    // create constant buffer for transformation matrix
    struct ConstantBuffer
    {
        DirectX::XMMATRIX transform;
    };
    const ConstantBuffer cb =
    {
        {
            DirectX::XMMatrixTranspose(
                DirectX::XMMatrixRotationZ( 1.0f ) *
                DirectX::XMMatrixRotationX( 1.0f ) *
                DirectX::XMMatrixTranslation( 1.0f,1.0f,4.0f ) * 
                projectionMatrix
            )
        }
    };
    ID3D11Buffer* pConstantBuffer;
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof( cb );
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    m_pDevice->CreateBuffer( &cbd, &csd, &pConstantBuffer );

    // bind constant buffer to vertex shader
    m_pContext->VSSetConstantBuffers( 0u, 1u, &pConstantBuffer );


    // lookup table for cube face colors
    struct ConstantBuffer2
    {
        struct
        {
            float r;
            float g;
            float b;
            float a;
        } face_colors[6];
    };
    const ConstantBuffer2 cb2 =
    {
        {
            {1.0f,0.0f,1.0f},
            {1.0f,0.0f,0.0f},
            {0.0f,1.0f,0.0f},
            {0.0f,0.0f,1.0f},
            {1.0f,1.0f,0.0f},
            {1.0f,1.0f,1.0f},
        }
    };
    ID3D11Buffer* pConstantBuffer2;
    D3D11_BUFFER_DESC cbd2;
    cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd2.Usage = D3D11_USAGE_DEFAULT;
    cbd2.CPUAccessFlags = 0u;
    cbd2.MiscFlags = 0u;
    cbd2.ByteWidth = sizeof( cb2 );
    cbd2.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd2 = {};
    csd2.pSysMem = &cb2;
    m_pDevice->CreateBuffer( &cbd2, &csd2, &pConstantBuffer2 );

    // bind constant buffer to pixel shader
    m_pContext->PSSetConstantBuffers( 0u, 1u, &pConstantBuffer2 );



    // create pixel shader
    ID3D11PixelShader* pPixelShader;
    ID3DBlob* pBlob;
    D3DReadFileToBlob( L"PixelShader.cso", &pBlob );
    m_pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader );

    // bind pixel shader
    m_pContext->PSSetShader( pPixelShader, nullptr, 0u );


    // create vertex shader
    ID3D11VertexShader* pVertexShader;
    D3DReadFileToBlob( L"VertexShader.cso", &pBlob );
    m_pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader );

    // bind vertex shader
    m_pContext->VSSetShader( pVertexShader, nullptr, 0u );


    // input (vertex) layout (2d position only)
    ID3D11InputLayout* pInputLayout;
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
    };
    m_pDevice->CreateInputLayout(
        ied, (UINT)std::size( ied ),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &pInputLayout
    );

    // bind vertex layout
    m_pContext->IASetInputLayout( pInputLayout );


    // bind render target
    m_pContext->OMSetRenderTargets( 1u, &m_pTarget, nullptr );


    // Set primitive topology to triangle list (groups of 3 vertices)
    m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = (float)1280;
    vp.Height = (float)720;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_pContext->RSSetViewports( 1u, &vp );


    m_pContext->DrawIndexed( (UINT)std::size( indices ), 0u, 0u );

    m_pSwap->Present( 1u, 0u );
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

void D3D11Interface::BindViewProjectionMatrix( DirectX::XMMATRIX viewProjectionMatrix )
{
    struct ConstantBuffer
    {
        DirectX::XMFLOAT4X4 modelViewProjection;
    };

    ID3D11Buffer* m_constantBuffer;
    ConstantBuffer m_constantBufferData;

    D3D11_BUFFER_DESC constantBufferDesc = { 0 };
    constantBufferDesc.ByteWidth = sizeof( m_constantBufferData );
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.StructureByteStride = 0;
    m_pDevice->CreateBuffer(
        &constantBufferDesc,
        nullptr,
        &m_constantBuffer
    );

    m_pContext->VSSetConstantBuffers( 1u, 1u, &m_constantBuffer );


    m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}
