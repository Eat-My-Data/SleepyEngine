#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"

SceneManager::~SceneManager()
{
	//delete m_pDirectionalLight;
}

void SceneManager::Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api )
{		
	m_pGDI = &gdi;
	m_GraphicsAPI = api;
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", true, 1.0f / 20.0f ) );
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", false, 1.0f / 20.0f ) );
	m_pForwardDirectionalLight = new DirectionalLight( gdi, RenderTechnique::Forward );
	m_pDeferredDirectionalLight = new DirectionalLight( gdi, RenderTechnique::Deferred );
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

void SceneManager::ForwardRender()
{
	// setup
	const float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetShadowDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );

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
	} plcbuf;

	// point light data
	D3D11_BUFFER_DESC plcbd;
	plcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	plcbd.Usage = D3D11_USAGE_DYNAMIC;
	plcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	plcbd.MiscFlags = 0u;
	plcbd.ByteWidth = sizeof( PointLightCBuf );
	plcbd.StructureByteStride = 0u;
	ID3D11Buffer* pConstantBuffer;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &plcbuf;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	m_pGDI->GetDevice()->CreateBuffer( &plcbd, &InitData, &pConstantBuffer );
	m_pGDI->GetContext()->PSSetConstantBuffers( 0u, 1u, &pConstantBuffer );

	// depth from light
	m_pGDI->SetViewMatrix( m_DirectionalLightOrthoCamera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_DirectionalLightOrthoCamera.GetProjectionMatrix() );
	m_pGDI->GetContext()->OMSetDepthStencilState( m_pGDI->GetBufferDSS(), 1u );
	m_pGDI->GetContext()->OMSetRenderTargets( 0, nullptr, *m_pGDI->GetShadowDSV() );
	m_vecOfModels[1]->Draw( *m_pGDI, true );

	// forward render
	m_pGDI->GetContext()->OMSetRenderTargets( 1u, m_pGDI->GetTarget(), *m_pGDI->GetDSV() );
	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );
	m_pGDI->GetContext()->PSSetShaderResources( 4, 1, m_pGDI->GetShadowResource() );
	//m_pForwardDirectionalLight->UpdateCBuffers( *m_pGDI, m_DirectionalLightOrthoCamera.GetViewMatrix(), m_DirectionalLightOrthoCamera.GetProjectionMatrix(), m_Camera.GetPosition() );
	
	// bind view project matrix to vertex buffer for forward
	struct DirectionalLightMatrix
	{
		DirectX::XMMATRIX lightViewMatrix;
		DirectX::XMMATRIX lightProjMatrix;
	} dlcbuf;

	dlcbuf.lightViewMatrix = m_DirectionalLightOrthoCamera.GetViewMatrix();
	dlcbuf.lightProjMatrix = m_DirectionalLightOrthoCamera.GetProjectionMatrix();

	D3D11_BUFFER_DESC dlcbd;
	dlcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dlcbd.Usage = D3D11_USAGE_DYNAMIC;
	dlcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dlcbd.MiscFlags = 0u;
	dlcbd.ByteWidth = sizeof( DirectionalLightMatrix );
	dlcbd.StructureByteStride = 0u;
	ID3D11Buffer* pConstantBuffer2;

	D3D11_SUBRESOURCE_DATA InitData2;
	InitData2.pSysMem = &dlcbuf;
	InitData2.SysMemPitch = 0;
	InitData2.SysMemSlicePitch = 0;

	m_pGDI->GetDevice()->CreateBuffer( &dlcbd, &InitData2, &pConstantBuffer2 );
	m_pGDI->GetContext()->VSSetConstantBuffers( 1u, 1u, &pConstantBuffer2 );

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
	m_pGDI->SetViewMatrix( m_DirectionalLightOrthoCamera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_DirectionalLightOrthoCamera.GetProjectionMatrix() );
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
	m_pDeferredDirectionalLight->UpdateCBuffers( *m_pGDI, m_DirectionalLightOrthoCamera.GetViewMatrix(), m_DirectionalLightOrthoCamera.GetProjectionMatrix(), m_Camera.GetPosition() );
	m_pDeferredDirectionalLight->Draw( *m_pGDI );

	// point light
	//m_pGDI->GetContext()->OMSetBlendState( m_pGDI->GetBlendState(), blendFactor, 0xffffffff );
	//m_pPointLight->UpdateCBuffers( *m_pGDI, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix(), m_Camera.GetPosition() );
	//m_pPointLight->Draw( *m_pGDI, m_Camera.GetPosition() );

	// clear shader resources
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	m_pGDI->GetContext()->PSSetShaderResources( 0, 5, null );
}