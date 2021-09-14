#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"

SceneManager::~SceneManager()
{
	for ( u32 i = 0; i < m_vecOfModels.size(); i++ )
	{
		delete m_vecOfModels[i];
	}
}

void SceneManager::Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api )
{		
	m_pGDI = &gdi;
	m_GraphicsAPI = api;
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", true, 1.0f / 20.0f ) );
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", false, 1.0f / 20.0f ) );
	m_LightManager.Initialize( *m_pGDI );
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
	PrepareFrame();

	if ( m_RenderTechnique == RenderTechnique::Forward )
		ForwardRender();
	else if ( m_RenderTechnique == RenderTechnique::Deferred )
		DeferredRender();

	// clear shader resources
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	m_pGDI->GetContext()->PSSetShaderResources( 0, 10, null );
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

void SceneManager::SetActiveLight( const u32 index )
{
	m_LightManager.SelectLight( index );
}

void SceneManager::TranslatePointLight( DirectX::XMFLOAT3 translation )
{
	m_LightManager.TranslatePointLight( translation );
}

void SceneManager::TranslateDirectionalLight( DirectX::XMFLOAT3 translation )
{
	m_LightManager.TranslateDirectionalLight( translation );
}

void SceneManager::RotateDirectionalLight( const f32 dx, const f32 dy )
{
	m_LightManager.RotateDirectionalLight( dx, dy );

}

void SceneManager::PrepareFrame()
{
	// setup
	const float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );
	for ( int i = 0; i < 3; i++ )
	{
		m_pGDI->GetContext()->ClearRenderTargetView( m_pGDI->GetGBuffers()[i], color );
	}
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetShadowDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
}

void SceneManager::ForwardRender()
{
	// depth from light
	m_LightManager.PrepareDepthFromLight();
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// point light depth pass
	m_LightManager.RenderPointLightCubeTextures( *m_vecOfModels[1] );

	// update and render
	m_pGDI->GetContext()->OMSetRenderTargets( 1u, m_pGDI->GetTarget(), *m_pGDI->GetDSV() );
	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	m_pGDI->GetContext()->PSSetShaderResources( 4, 1, m_pGDI->GetShadowResource() );
	m_LightManager.UpdateBuffers( m_Camera.GetPosition() );
	m_vecOfModels[0]->Draw( *m_pGDI, false );

	// light cores
	m_LightManager.RenderSolidSpheres();
}

void SceneManager::DeferredRender()
{
	// depth from light
	m_LightManager.PrepareDepthFromLight();
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// point light depth pass
	m_LightManager.RenderPointLightCubeTextures( *m_vecOfModels[1] );

	// gbuffers
	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 3, m_pGDI->GetGBuffers(), *m_pGDI->GetDSV() );
	m_vecOfModels[1]->Draw( *m_pGDI, false );

	// lights
	m_LightManager.UpdateBuffers( m_Camera.GetPosition() );
	m_LightManager.Draw();

	// light cores
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_LightManager.RenderSolidSpheres();
}