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
	m_bIsInitialized = true;
}

bool GUIManager::IsInitialized() noexcept
{
	return m_bIsInitialized;
}

bool GUIManager::Begin( char* title )
{
	GUIContainer::AddElement( new Title( title ) );
	return true;
}

void GUIManager::Text( char* text )
{	
	GUIContainer::AddElement( new TextBox( text ) );
}

bool GUIManager::Button( char* sentence )
{
	// TODO: 
	// Add Button Element to Container and return whether it has been clicked
	return false;
}

void GUIManager::End()
{
	// GUIContainer::Draw(  m_pGDI );
}

//void GUIManager::Update()
//{
//	delete m_pHot;
//	m_pHot = new UI_ID{ (char*)"No Hot Elements", 100 };
//
//	m_pGDI->GetContext()->OMSetRenderTargets( 1u, m_pGDI->GetTarget(), nullptr );
//	m_GUITransformCbuf.guiTransform = m_GUITransformCamera.GetProjectionMatrix();
//	Bind::VertexConstantBuffer<GUITransform>::VertexConstantBuffer( *m_pGDI, m_GUITransformCbuf ).Bind( *m_pGDI );
//
//	for ( u32 i = 0; i < m_vecOfGUIElements.size(); i++ )
//	{
//		//if ( m_vecOfGUIElements[i]->DoElement( *m_pGDI, *m_pActive, *m_pHot, *m_pMouse ) )
//		//{
//		//	m_vecOfGUIElements[i]->Interact( *m_pSceneManager );
//
//		//	delete m_pActive;
//		//	m_pActive = new UI_ID{ (char*)"No Active Elements", 100 };
//		//}
//	}
//}