#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"

void SceneManager::Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api )
{		
	m_pGDI = &gdi;
	m_GraphicsAPI = api;
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\Sponza\\sponza.obj", 1.0f / 20.0f ) );

	for ( u32 i = 0; i < 10; i++ )
	{
		f32 tmpPos[3] = { 10.0f * i, 10.0f * i, 10.0f * i };
		//m_vecOfCubes.push_back( new Cube( *m_pGDI, { { tmpPos[0], tmpPos[1], tmpPos[2] }, 0.0f, 0.0f, 0.0f }, 2.0f ) );
	}
}

bool SceneManager::IsInitialzed() noexcept
{
	return m_GraphicsAPI != GraphicsAPI::Uninitialized;
}

void SceneManager::Draw()
{
	const float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );
	m_pGDI->GetContext()->ClearDepthStencilView( *m_pGDI->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );

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

	for ( u32 i = 0; i < m_vecOfCubes.size(); i++ )
	{
		//m_vecOfCubes[i]->Draw( *m_pGDI );
	}

	m_vecOfModels[0]->Draw( *m_pGDI );

	m_pGDI->GetSwap()->Present( 1u, 0u );
}

void SceneManager::Update( f32 dt )
{
	m_Camera.Rotate( -dt, dt );
	m_Camera.Translate( { 0.0f, 0.0f, -dt * 0.15f } );

	for ( u32 i = 0; i < m_vecOfCubes.size(); i++ )
	{
		m_vecOfCubes[i]->SetPos( { dt, 0.0f, 0.0f }  );
		m_vecOfCubes[i]->SetRotation( 0.0f, 0.0f, dt );
	}
}
