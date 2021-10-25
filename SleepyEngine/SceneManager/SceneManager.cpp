#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../Libraries/imgui/backends/imgui_impl_dx11.h"
#include "../Libraries/imgui/backends/imgui_impl_win32.h"

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
	m_pCameraBuffer = new Bind::PixelConstantBuffer<CameraData>{ gdi, 6u };
	//m_pMonster = new Model( *m_pGDI, "Models\\character_01\\character_01.obj", true, 2000.0f );
	//m_pMonster->SetRootTransform( DirectX::XMMatrixTranslation( 0.0f, -250.0f, 0.0f ) * DirectX::XMMatrixRotationY( -PI / 2.0f ) * DirectX::XMMatrixRotationZ( PI / 2.0f ) );
	m_LightManager.Initialize( *m_pGDI );
	ImGui_ImplDX11_Init( m_pGDI->GetDevice(), m_pGDI->GetContext() );
}

bool SceneManager::IsInitialzed() noexcept
{
	return m_GraphicsAPI != GraphicsAPI::Uninitialized;
}

void SceneManager::SetRenderTechnique( RenderTechnique renderTechnique ) noexcept
{
	m_RenderTechnique = renderTechnique;
}

void SceneManager::ToggleImGuiEngabled() noexcept
{
	imguiEnabled = !imguiEnabled;
}

void SceneManager::Draw()
{
	if ( imguiEnabled )
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	PrepareFrame();

	if ( m_RenderTechnique == RenderTechnique::Forward )
		ForwardRender();
	else if ( m_RenderTechnique == RenderTechnique::Deferred )
		DeferredRender();

	// clear shader resources
	ID3D11ShaderResourceView* null[12] = {};
	m_pGDI->GetContext()->PSSetShaderResources( 0, 12, null );
}

void SceneManager::DrawControlPanel()
{
	if ( imguiEnabled && ImGui::Begin( "Control Panel" ) )
	{
		ImGui::Text( "Camera" );
		ImGui::SliderFloat( "Camera X", &m_Camera.GetPosition().x, -80.0f, 80.0f );
		ImGui::SliderFloat( "Camera Y", &m_Camera.GetPosition().y, -80.0f, 80.0f );
		ImGui::SliderFloat( "Camera Z", &m_Camera.GetPosition().z, -80.0f, 80.0f );
		ImGui::Text( "Camera Orientation" );
		ImGui::SliderAngle( "Camera Pitch", &m_Camera.m_fPitch, 0.995f * -90.0f, 0.995f * 90.0f );
		ImGui::SliderAngle( "Camera Yaw", &m_Camera.m_fYaw, -180.0f, 180.0f );
		m_LightManager.DrawControlPanel();
		if ( ImGui::Button( "Toggle Render Technique" ) )
			m_RenderTechnique == RenderTechnique::Deferred ? SetRenderTechnique( RenderTechnique::Forward ) : SetRenderTechnique( RenderTechnique::Deferred );
		ImGui::SameLine();
		ImGui::Text( m_RenderTechnique == RenderTechnique::Deferred ? "Deferred" : "Forward" );
	}
	ImGui::End();
}

void SceneManager::Present()
{
	if ( imguiEnabled )
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
		ImGui::EndFrame();
	}

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

void SceneManager::SetActivePointLight( const u32 index )
{
	m_LightManager.SelectPointLight( index );
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

void SceneManager::SetActiveSpotLight( const u32 index )
{
	m_LightManager.SelectSpotLight( index );
}

void SceneManager::TranslateSpotLight( DirectX::XMFLOAT3 translation )
{
	m_LightManager.TranslateSpotLight( translation );
}

void SceneManager::RotateSpotLight( const f32 dx, const f32 dy )
{
	m_LightManager.RotateSpotLight( dx, dy );
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
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetShadowDSV2(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
}

void SceneManager::UpdateCameraBuffer()
{
	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( m_Camera.GetViewMatrix() );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, m_Camera.GetViewMatrix() );

	// get inverse of the projection matrix
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( m_Camera.GetProjectionMatrix() );
	DirectX::XMMATRIX projInvMatrix = DirectX::XMMatrixInverse( &determinant2, m_Camera.GetProjectionMatrix() );

	m_CameraCBufferaData.camPos = DirectX::XMFLOAT4( m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z, 1.0f);
	m_CameraCBufferaData.viewInvMatrix = cameraMatrix;
	m_CameraCBufferaData.projInvMatrix = projInvMatrix;

	m_pCameraBuffer->Update( *m_pGDI, m_CameraCBufferaData );
	m_pCameraBuffer->Bind( *m_pGDI );
}

void SceneManager::ForwardRender()
{
	// depth from light
	m_LightManager.PrepareDepthFromLight();
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// point light depth pass
	m_LightManager.RenderPointLightCubeTextures( *m_vecOfModels[1] );

	// spot light depth pass
	m_LightManager.PrepareDepthFromSpotLight();
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// update and render
	m_pGDI->GetContext()->OMSetRenderTargets( 1u, m_pGDI->GetTarget(), *m_pGDI->GetDSV() );
	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	m_pGDI->GetContext()->PSSetShaderResources( 5u, 1, m_pGDI->GetShadowResource() );
	m_LightManager.UpdateBuffers( m_Camera.GetPosition() );
	m_vecOfModels[0]->Draw( *m_pGDI, false );
	//m_pMonster->Draw( *m_pGDI, false );
	// light cores
	m_LightManager.RenderLightGeometry();
}

void SceneManager::DeferredRender()
{
	// depth from light
	m_LightManager.PrepareDepthFromLight();
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// point light depth pass
	m_LightManager.RenderPointLightCubeTextures( *m_vecOfModels[1] );

	// spot light depth pass
	m_LightManager.PrepareDepthFromSpotLight();
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// gbuffers
	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 3, m_pGDI->GetGBuffers(), *m_pGDI->GetDSV() );
	m_vecOfModels[1]->Draw( *m_pGDI, false );
	//m_pMonster->Draw( *m_pGDI, false );

	// lights
	UpdateCameraBuffer();
	m_LightManager.UpdateBuffers( m_Camera.GetPosition() );
	m_LightManager.Draw();

	// light cores
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_LightManager.RenderLightGeometry();
}