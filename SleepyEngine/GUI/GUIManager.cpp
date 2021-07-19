#include "GUIManager.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../Win32/Mouse.h"
#include "../Win32/Keyboard.h"

void GUIManager::Initialize( GraphicsDeviceInterface& gdi, SceneManager& sceneManager, Mouse& mouse )
{
	m_pGDI = &gdi;
	m_pSceneManager = &sceneManager;
	m_pMouse = &mouse;
	m_pActive = new UI_ID{ (char*)"No Active Elements", 100 };
	m_pHot = new UI_ID{ (char*)"No Hot Elements", 100 };
	m_vecOfGUIElements.push_back( new ChooseRenderTechnique( -0.5f, -0.5f, 0.1f, 0.1f ) );
	m_bIsInitialized = true;
}

bool GUIManager::IsInitialized() noexcept
{
	return m_bIsInitialized;
}

void GUIManager::Update()
{
	for ( u32 i = 0; i < m_vecOfGUIElements.size(); i++ )
	{
		if ( m_vecOfGUIElements[i]->DoElement( *m_pActive, *m_pHot, *m_pMouse ) )
			m_vecOfGUIElements[i]->Interact( *m_pSceneManager );
	}
}

void GUIManager::Draw()
{
	m_pGDI->GetContext()->OMSetRenderTargets( 1u, m_pGDI->GetTarget(), nullptr );
	m_GUITransformCbuf.guiTransform = m_GUITransformCamera.GetViewMatrix();
	Bind::VertexConstantBuffer<GUITransform>::VertexConstantBuffer( *m_pGDI, m_GUITransformCbuf).Bind(* m_pGDI );
	for ( u32 i = 0; i < m_vecOfGUIElements.size(); i++ )
	{
		m_vecOfGUIElements[i]->Draw( *m_pGDI );
	}
}
