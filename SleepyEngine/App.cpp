#include "App.h"

App::App()
	:
	m_Win32Window( 1280, 720, L"Sleepy Engine" )
{
	m_SceneManager.SetRenderTechnique( RenderTechnique::Deferred );
}

u32 App::Launch()
{
	while ( true )
	{
		// process all messages pending, but to not block for new messages
		if ( const auto ecode = Win32Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		ExecuteFrame();
	}
}

App::~App()
{}

void App::ExecuteFrame()
{
	if ( !m_GDI.IsInitialized() )
		m_Win32Window.InitializeGraphics( m_GDI, GraphicsAPI::DirectX );	
	if ( !m_SceneManager.IsInitialzed() )
		m_SceneManager.Initialize( m_GDI, GraphicsAPI::DirectX );

		
	const f32 dt = timer.Mark();

	m_SceneManager.Update( dt );
	m_SceneManager.Draw();
}
