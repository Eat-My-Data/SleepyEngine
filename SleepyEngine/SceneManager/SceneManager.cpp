#include "SceneManager.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"

void SceneManager::Initialize( GraphicsDeviceInterface& gdi, GraphicsAPI api )
{		
	m_pGDI = &gdi;
	m_GraphicsAPI = api;
	m_vecOfModels.push_back( new Model( *m_pGDI, "Models\\sponza\\sponza.obj", 1.0f / 20.0f ) );

	for ( u32 i = 0; i < 10; i++ )
	{
		f32 tmpPos[3] = { 10.0f * i, 10.0f * i, 10.0f * i };
		m_vecOfCubes.push_back( new Cube( *m_pGDI, { { tmpPos[0], tmpPos[1], tmpPos[2] }, 0.0f, 0.0f, 0.0f }, 2.0f ) );
	}
}

bool SceneManager::IsInitialzed() noexcept
{
	return m_GraphicsAPI != GraphicsAPI::Uninitialized;
}

void SceneManager::Draw()
{
	float color[4] = { 1.0f,1.0,1.0,1.0f };
	m_pGDI->GetContext()->ClearRenderTargetView( *m_pGDI->GetTarget(), color );

	m_pGDI->SetViewMatrix( m_Camera.GetViewMatrix() );
	m_pGDI->SetProjMatrix( m_Camera.GetProjectionMatrix() );

	for ( u32 i = 0; i < m_vecOfCubes.size(); i++ )
	{
		m_vecOfCubes[i]->Draw( *m_pGDI );
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
