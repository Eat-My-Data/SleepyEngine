#include "SceneManager.h"
#include "GraphicsDeviceInterface.h"

void SceneManager::Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api )
{		
	m_pGDI = &gdi;
	m_GraphicsAPI = api;

	for ( u32 i = 0; i < 2; i++ )
	{
		f32 tmpPos[3] = { 0.0f, 0.0f, 0.01f * i };
		m_vecOfCubes.push_back( new Cube( *m_pGDI, { { tmpPos[0], tmpPos[1], tmpPos[2] }, 0.0f, 0.0f, 0.0f }, 2.0f ) );
	}
}

bool SceneManager::IsInitialzed() noexcept
{
	return m_GraphicsAPI != GraphicsAPI::Uninitialized;
}

void SceneManager::Draw()
{
	float color[4] = { 0.0f,1.0,1.0,1.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );

	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );

	for ( u32 i = 0; i < m_vecOfCubes.size(); i++ )
	{
		m_vecOfCubes[i]->Draw( *m_pGDI );
	}

	m_pGDI->GetSwap()->Present( 1u, 0u );
}

void SceneManager::Update( f32 dt )
{
	m_Camera.Rotate( -dt * 2.0f, dt );
	//m_Camera.Translate( { 0.0f, 0.0f, dt } );

	for ( u32 i = 0; i < m_vecOfCubes.size(); i++ )
	{
		m_vecOfCubes[i]->SetPos( { 0.0f, 0.0f, 0.0f }  );
		m_vecOfCubes[i]->SetRotation( 0.0f, 0.0f, 0.0f );
	}
}
