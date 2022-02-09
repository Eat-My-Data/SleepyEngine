#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../Libraries/imgui/backends/imgui_impl_dx11.h"
#include "../Libraries/imgui/backends/imgui_impl_win32.h"
#include "../Utilities/Testing.h"
#include "../ResourceManager/Material.h"

#include "../ResourceManager/Jobber/TestModelProbe.h"

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
	rg = new Rgph::BlurOutlineRenderGraph( *m_pGDI );
	m_GraphicsAPI = api;
	//m_pTestCube = new Cube( *m_pGDI, { { 4.0f,0.0f,0.0f }, 0.0f, 0.0f, 0.0f } );
	//m_pTestCube2 = new Cube( *m_pGDI, { { 0.0f,4.0f,0.0f }, 0.0f, 0.0f, 0.0f } );
	sponza = new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", 1.0f / 20.0f );
	m_pCameraBuffer = new Bind::PixelConstantBuffer<CameraData>{ gdi, 6u };
	m_LightManager.Initialize( *m_pGDI );

	//m_pTestCube->LinkTechniques( *rg );
	//m_pTestCube2->LinkTechniques( *rg );
	//m_LightManager.LinkTechniques( *rg );
	sponza->LinkTechniques( *rg );
	
	
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
	namespace dx = DirectX;

	PrepareFrame();

	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	UpdateCameraBuffer();

	//m_LightManager.Submit();
	m_LightManager.UpdateBuffers();
	//m_pTestCube->Submit();
	//m_pTestCube2->Submit();
	sponza->Submit();

	static MP modelProbe;
	modelProbe.SpawnWindow( *sponza );

	rg->Execute( *m_pGDI );

	if ( imguiEnabled )
	{
		// imgui windows
		DrawControlPanel();
	}

	rg->Reset();

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
		if ( ImGui::Button( "Toggle Render Technique" ) )
			m_RenderTechnique == RenderTechnique::Deferred ? SetRenderTechnique( RenderTechnique::Forward ) : SetRenderTechnique( RenderTechnique::Deferred );
		ImGui::SameLine();
		ImGui::Text( m_RenderTechnique == RenderTechnique::Deferred ? "Deferred" : "Forward" );
	}
	ImGui::End();

	m_LightManager.DrawControlPanel();
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
	// imgui begin frame
	if ( imguiEnabled )
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	//// clearing shader inputs to prevent simultaneous in/out bind carried over from prev frame
	//ID3D11ShaderResourceView* const pNullTex = nullptr;
	//m_pGDI->GetContext()->PSSetShaderResources( 0, 1, &pNullTex ); // fullscreen input texture
	//m_pGDI->GetContext()->PSSetShaderResources( 3, 1, &pNullTex ); // shadow map texture
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