#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"

SceneManager::~SceneManager()
{
	for ( u32 i = 0; i < m_vecOfModels.size(); i++ )
	{
		delete m_vecOfModels[i];
	}
	delete m_pForwardDirectionalLight;
	delete m_pDeferredDirectionalLight;
	delete m_pForwardPointLight;
}

void SceneManager::Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api )
{		
	m_pGDI = &gdi;
	m_GraphicsAPI = api;
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", true, 1.0f / 20.0f ) );
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", false, 1.0f / 20.0f ) );
	m_pForwardDirectionalLight = new DirectionalLight( gdi, RenderTechnique::Forward );
	m_pDeferredDirectionalLight = new DirectionalLight( gdi, RenderTechnique::Deferred );
	m_pForwardPointLight = new PointLight( gdi, 10.0f,RenderTechnique::Forward );
	m_pDeferredPointLight = new PointLight( gdi, 10.0f, RenderTechnique::Deferred );
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

void SceneManager::Present()
{
	m_pGDI->GetSwap()->Present( 1u, 0u );
}

void SceneManager::RotateCamera( const f32 dx, const f32 dy )
{
	m_Camera.Rotate( dx, dy );
}

void SceneManager::TranslateCamera( DirectX::XMFLOAT3 camDelta )
{
	m_Camera.Translate( camDelta );
}

void SceneManager::TranslatePointLight( DirectX::XMFLOAT3 translation )
{
	if ( m_RenderTechnique == RenderTechnique::Forward )
		m_pForwardPointLight->Translate( translation );
	else if ( m_RenderTechnique == RenderTechnique::Deferred )
		m_pDeferredPointLight->Translate( translation );
}

void SceneManager::RotatePointLight( const f32 dx, const f32 dy )
{
}

void SceneManager::TranslateDirectionalLight( DirectX::XMFLOAT3 translation )
{
	if ( m_RenderTechnique == RenderTechnique::Forward )
		m_pForwardDirectionalLight->Translate( translation );
	else if ( m_RenderTechnique == RenderTechnique::Deferred )
		m_pDeferredDirectionalLight->Translate( translation );
}

void SceneManager::RotateDirectionalLight( const f32 dx, const f32 dy )
{
	if ( m_RenderTechnique == RenderTechnique::Forward )
		m_pForwardDirectionalLight->Rotate( dx, dy );
	else if ( m_RenderTechnique == RenderTechnique::Deferred )
		m_pDeferredDirectionalLight->Rotate( dx, dy );
}

void SceneManager::ForwardRender()
{
	// setup
	const float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetShadowDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );

	// depth from light
	m_pGDI->SetViewMatrix( m_pForwardDirectionalLight->GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_pForwardDirectionalLight->GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, *m_pGDI->GetShadowDSV() );
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// point light
	m_pForwardPointLight->UpdateForwardCBuffer( *m_pGDI );

	// directional light
	m_pForwardDirectionalLight->UpdateForwardCBuffer( *m_pGDI );

	// forward render
	m_pGDI->GetContext()->OMSetRenderTargets( 1u, m_pGDI->GetTarget(), *m_pGDI->GetDSV() );
	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	m_pGDI->GetContext()->PSSetShaderResources( 4, 1, m_pGDI->GetShadowResource() );
	m_vecOfModels[0]->Draw( *m_pGDI, false );

	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	m_pGDI->GetContext()->PSSetShaderResources( 0, 5, null );
}

void SceneManager::DeferredRender()
{
	// setup
	const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );
	for ( int i = 0; i < 3; i++ )
	{
		m_pGDI->GetContext()->ClearRenderTargetView( m_pGDI->GetGBuffers()[i], color );
	}
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetShadowDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );

	// gbuffers
	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 3, m_pGDI->GetGBuffers(), *m_pGDI->GetDSV() );
	m_vecOfModels[1]->Draw( *m_pGDI, false );

	// depth from light
	m_pGDI->SetViewMatrix( m_pDeferredDirectionalLight->GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_pDeferredDirectionalLight->GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, *m_pGDI->GetShadowDSV() );
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// directional light
	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );
	m_pGDI->GetContext()->OMSetRenderTargets( 1, m_pGDI->GetTarget(), *m_pGDI->GetDSV_ReadOnly() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetLightDSS(), 1u );
	m_pGDI->GetContext()->PSSetShaderResources( 0, 3, m_pGDI->GetShaderResources() );
	m_pGDI->GetContext()->PSSetShaderResources( 3, 1, m_pGDI->GetDepthResource() );
	m_pGDI->GetContext()->PSSetShaderResources( 4, 1, m_pGDI->GetShadowResource() );
	m_pDeferredDirectionalLight->UpdateCBuffers( *m_pGDI, m_Camera.GetPosition() );
	m_pDeferredDirectionalLight->Draw( *m_pGDI );

	// point light
	m_pDeferredPointLight->UpdateCBuffers( *m_pGDI, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix(), m_Camera.GetPosition() );
	m_pDeferredPointLight->Draw( *m_pGDI, m_Camera.GetPosition() );

	// clear shader resources
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	m_pGDI->GetContext()->PSSetShaderResources( 0, 5, null );
}