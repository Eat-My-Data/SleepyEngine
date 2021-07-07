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
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
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

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for ( int i = 0; i < bufferCount; i++ )
	{
		HRESULT result = m_pDevice->CreateTexture2D( &textureDesc, NULL, &m_pTextures[i] );
		if ( FAILED( result ) )
		{
			throw std::exception();
		}
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	m_pSwap->GetBuffer( 0, __uuidof( ID3D11Resource ), (void**)&pBackBuffer );
	m_pDevice->CreateRenderTargetView( pBackBuffer, nullptr, &m_pTarget );

	for ( int i = 0; i < bufferCount; i++ )
	{
		HRESULT result = m_pDevice->CreateRenderTargetView( m_pTextures[i], &renderTargetViewDesc, &m_pGBuffers[i] );
		if ( FAILED( result ) )
		{
			throw std::exception();
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for ( int i = 0; i < bufferCount; i++ )
	{
		HRESULT result = m_pDevice->CreateShaderResourceView( m_pTextures[i], &shaderResourceViewDesc, &m_pShaderResources[i] );
		if ( FAILED( result ) )
		{
			throw std::exception();
		}
	}

	D3D11_BLEND_DESC blendDescDR;
	ZeroMemory( &blendDescDR, sizeof( blendDescDR ) );
	blendDescDR.AlphaToCoverageEnable = false;
	blendDescDR.IndependentBlendEnable = false;
	blendDescDR.RenderTarget[0].BlendEnable = true;
	blendDescDR.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescDR.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescDR.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_pDevice->CreateBlendState( &blendDescDR, &m_pBlendState );

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = m_pDevice->CreateDepthStencilState( &dsDesc, &m_pGBufferDSS );
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	D3D11_DEPTH_STENCIL_DESC dsDescLight = {};
	dsDescLight.DepthEnable = FALSE;
	dsDescLight.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	hr = m_pDevice->CreateDepthStencilState( &dsDescLight, &m_pLightingDSS );

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    m_pDevice->CreateTexture2D( &descDepth, nullptr, &pDepthStencil );

	m_pDevice->CreateTexture2D( &descDepth, nullptr, &m_pShadowTexture );

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    hr = m_pDevice->CreateDepthStencilView( pDepthStencil, &descDSV, &m_pDSV );
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	hr = m_pDevice->CreateDepthStencilView( m_pShadowTexture, &descDSV, &m_pShadowDSV );
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	descDSV.Flags = D3D11_DSV_READ_ONLY_DEPTH;
	m_pDevice->CreateDepthStencilView( pDepthStencil, &descDSV, &m_pDSV_ReadOnly );

	D3D11_SHADER_RESOURCE_VIEW_DESC depthShaderResourceDesc = {};
	depthShaderResourceDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthShaderResourceDesc.Texture2D.MostDetailedMip = 0;
	depthShaderResourceDesc.Texture2D.MipLevels = 1;
	hr = m_pDevice->CreateShaderResourceView( pDepthStencil, &depthShaderResourceDesc, &m_pDepthResource );
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	hr = m_pDevice->CreateShaderResourceView( m_pShadowTexture, &depthShaderResourceDesc, &m_pShadowSRV );
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	// viewport
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

ID3D11DepthStencilView** D3D11Interface::GetDSV() noexcept
{
    return &m_pDSV;
}

ID3D11RenderTargetView** D3D11Interface::GetGBuffers() noexcept
{
	return m_pGBuffers;
}

ID3D11ShaderResourceView** D3D11Interface::GetShaderResources() noexcept
{
	return m_pShaderResources;
}

ID3D11ShaderResourceView** D3D11Interface::GetDepthResource() noexcept
{
	return &m_pDepthResource;
}

ID3D11ShaderResourceView** D3D11Interface::GetShadowResource() noexcept
{
	return &m_pShadowSRV;
}

ID3D11BlendState* D3D11Interface::GetBlendState() noexcept
{
	return m_pBlendState;
}

ID3D11DepthStencilView** D3D11Interface::GetDSV_ReadOnly() noexcept
{
	return &m_pDSV_ReadOnly;
}

ID3D11DepthStencilState* D3D11Interface::GetLightDSS() noexcept
{
	return m_pLightingDSS;
}

ID3D11DepthStencilState* D3D11Interface::GetGBufferDSS() noexcept
{
	return m_pGBufferDSS;
}

ID3D11DepthStencilView** D3D11Interface::GetShadowDSV() noexcept
{
	return &m_pShadowDSV;
}
