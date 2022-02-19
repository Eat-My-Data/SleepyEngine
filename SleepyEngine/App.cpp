#include "App.h"

App::App()
	:
	m_Win32Window( 1280, 720, L"Sleepy Engine" )
{
	m_SceneManager.SetRenderTechnique( RenderTechnique::Forward );
}

u32 App::Launch()
{
	while ( true )
	{
		if ( const auto ecode = Win32Window::ProcessMessages() )
			return *ecode;
		const auto dt = timer.Mark() * speed_factor;
		HandleInput( dt );
		ExecuteFrame( dt );
	}
}

App::~App()
{}

void App::ExecuteFrame( float dt )
{
	if ( !m_GDI.IsInitialized() )
		m_Win32Window.InitializeGraphics( m_GDI, GraphicsAPI::DirectX );
	if ( !m_SceneManager.IsInitialzed() )
		m_SceneManager.Initialize( m_GDI, GraphicsAPI::DirectX );

	m_SceneManager.Draw();
	m_SceneManager.Present();
}

void App::HandleInput( float dt )
{
	while ( const auto e = m_Win32Window.m_Kbd.ReadKey() )
	{
		// free camera / cursor functionality
		if ( !e->IsPress() )
			continue;
		switch ( e->GetCode() )
		{
		case VK_ESCAPE:
			if ( m_Win32Window.CursorEnabled() )
			{
				m_SceneManager.ToggleImGuiEngabled();
				m_Win32Window.DisableCursor();
				m_Win32Window.m_Mouse.EnableRaw();
			}
			else
			{
				m_SceneManager.ToggleImGuiEngabled();
				m_Win32Window.EnableCursor();
				m_Win32Window.m_Mouse.DisableRaw();
			}
			break;
		}
	}

	f32 rotato = 500.f;
	if ( !m_Win32Window.CursorEnabled() )
	{
		if ( !m_Win32Window.m_Kbd.KeyIsPressed( VK_SHIFT ) && !m_Win32Window.m_Kbd.KeyIsPressed( VK_CONTROL ) )
		{
			// camera translation
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'W' ) )
				m_SceneManager.TranslateCamera( { 0.0f,0.0f,dt } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'S' ) )
				m_SceneManager.TranslateCamera( { 0.0f,0.0f,-dt } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'A' ) )
				m_SceneManager.TranslateCamera( { -dt,0.0f,0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'D' ) )
				m_SceneManager.TranslateCamera( { dt,0.0f,0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'R' ) )
				m_SceneManager.TranslateCamera( { 0.0f,dt,0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'F' ) )
				m_SceneManager.TranslateCamera( { 0.0f,-dt,0.0f } );

			// camera rotation
			if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_UP ) )
				m_SceneManager.RotateCamera( 0.0f, -dt * rotato );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_DOWN ) )
				m_SceneManager.RotateCamera( 0.0f, dt * rotato );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_RIGHT ) )
				m_SceneManager.RotateCamera( dt * rotato, 0.0f );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_LEFT ) )
				m_SceneManager.RotateCamera( -dt * rotato, 0.0f );
		}
	}

	// raw mouse input
	//while ( const auto delta = m_Win32Window.m_Mouse.Read() )
	//{
	//	if ( !m_Win32Window.CursorEnabled() )
	//		m_SceneManager.RotateCamera( (f32)delta->GetPosX(), (f32)delta->GetPosY() );
	//}
}
