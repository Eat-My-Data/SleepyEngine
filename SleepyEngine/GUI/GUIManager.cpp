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
	//m_vecOfGUIElements.push_back( new ChooseRenderTechnique( 0.0f, 0.0f, 100.0f, 50.0f ) );
	m_bIsInitialized = true;
}

bool GUIManager::IsInitialized() noexcept
{
	return m_bIsInitialized;
}

bool GUIManager::Begin( char* title )
{
	// TODO: 
	// - Create new container that holds items and has a title
	return false;
}

void GUIManager::Text( char* sentence )
{	
	// TODO: 
	// - Add Text Element to Container
}

bool GUIManager::Button( char* sentence )
{
	// TODO: 
	// Add Button Element to Container and return whether it has been clicked
	return false;
}

void GUIManager::End()
{
	// TODO: 
	// - Draw Elements
	// - Deconstruct Container For Next Element
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