#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"

SceneManager::~SceneManager()
{
	delete m_pDirectionalLight;
}

void SceneManager::Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api )
{		
	m_pGDI = &gdi;
	m_GraphicsAPI = api;
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", true, 1.0f / 20.0f ) );
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", false, 1.0f / 20.0f ) );
	m_pDirectionalLight = new DirectionalLight( gdi );
	m_pPointLight = new PointLight( gdi, 10.0f );
}

bool SceneManager::IsInitialzed() noexcept
{
	return m_GraphicsAPI != GraphicsAPI::Uninitialized;
}

void SceneManager::SetRenderTechnique( RenderTechnique renderTechnique ) noexcept
{
	m_RenderTechnique = renderTechnique;
}

void SceneManager::Draw()
{
	if ( m_RenderTechnique == RenderTechnique::Forward )
		ForwardRender();
	else if ( m_RenderTechnique == RenderTechnique::Deferred )
		DeferredRender();
}

void SceneManager::Update( f32 dt )
{
	m_Camera.Rotate( -dt, dt );
	m_Camera.Translate( { 0.0f, 0.0f, dt * 0.15f } );
}

void SceneManager::ForwardRender()
{
	const float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );

	m_pGDI->GetContext()->OMSetRenderTargets( 1u, m_pGDI->GetTarget(), *m_pGDI->GetDSV() );

	// temporary bind light cbuf
	struct PointLightCBuf
	{
		alignas( 16 ) DirectX::XMFLOAT3 pos = { 10.0f, 9.0f, 2.5f };
		alignas( 16 ) DirectX::XMFLOAT3 ambient = { 0.05f, 0.05f, 0.05f };
		alignas( 16 ) DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 1.0f;
		float attConst = 1.0f;
		float attLin = 0.045f;
		float attQuad = 0.0075f;
	} cbuf;

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof( PointLightCBuf );
	cbd.StructureByteStride = 0u;
	ID3D11Buffer* pConstantBuffer;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &cbuf;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	m_pGDI->GetDevice()->CreateBuffer( &cbd, &InitData, &pConstantBuffer );
	m_pGDI->GetContext()->PSSetConstantBuffers( 0u, 1u, &pConstantBuffer );

	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );

	m_vecOfModels[0]->Draw( *m_pGDI );

	m_pGDI->GetSwap()->Present( 1u, 0u );
}

void SceneManager::DeferredRender()
{
	// initial forward render
	const float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetGBuffers(), color );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );

	m_pGDI->GetContext()->OMSetRenderTargets( 3, m_pGDI->GetGBuffers(), *m_pGDI->GetDSV() );

	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );

	m_vecOfModels[1]->Draw( *m_pGDI );

	// directional light
	m_pGDI->GetContext()->OMSetRenderTargets( 1, m_pGDI->GetTarget(), *m_pGDI->GetDSV_ReadOnly() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetLightDSS(), 1u );

	const float blendFactor[4] = { 1.f, 1.f, 1.f, 1.f };
	m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );

	m_pGDI->GetContext()->PSSetShaderResources( 0, 3, m_pGDI->GetShaderResources() );
	m_pGDI->GetContext()->PSSetShaderResources( 3, 1, m_pGDI->GetDepthResource() );
	
	m_pDirectionalLight->UpdateCBuffers( *m_pGDI, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix(),m_Camera.GetPosition() );
	m_pDirectionalLight->Draw( *m_pGDI );

	// point light
	m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );
	m_pPointLight->UpdateCBuffers( *m_pGDI, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix(), m_Camera.GetPosition() );
	m_pPointLight->Draw( *m_pGDI, m_Camera.GetPosition() );

	// present
	m_pGDI->GetSwap()->Present( 1u, 0u );

	// clear shader resources
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr };
	m_pGDI->GetContext()->PSSetShaderResources( 0, 4, null );
}