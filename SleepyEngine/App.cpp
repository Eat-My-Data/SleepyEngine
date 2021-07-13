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

	m_SceneManager.Draw();

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
				m_Win32Window.DisableCursor();
				m_Win32Window.m_Mouse.EnableRaw();
			}
			else
			{
				m_Win32Window.EnableCursor();
				m_Win32Window.m_Mouse.DisableRaw();
			}
			break;
		}
	}

	if ( !m_Win32Window.CursorEnabled() )
	{
		f32 rotato = 500.f;
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
		else if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_SHIFT ) && !m_Win32Window.m_Kbd.KeyIsPressed( VK_CONTROL ) )
		{
			// point light translation
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'W' ) )
				m_SceneManager.TranslatePointLight( { dt * 20.0f, 0.0f, 0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'S' ) )
				m_SceneManager.TranslatePointLight( { -dt * 20.0f, 0.0f, 0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'A' ) )
				m_SceneManager.TranslatePointLight( { 0.0f, 0.0f, dt * 20.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'D' ) )
				m_SceneManager.TranslatePointLight( { 0.0f, 0.0f, -dt * 20.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'R' ) )
				m_SceneManager.TranslatePointLight( { 0.0f, dt * 20.0f, 0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'F' ) )
				m_SceneManager.TranslatePointLight( { 0.0f, -dt * 20.0f, 0.0f } );
		}
		else if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_CONTROL ) && !m_Win32Window.m_Kbd.KeyIsPressed( VK_SHIFT ) )
		{
			// directional light translation
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'W' ) )
				m_SceneManager.TranslateDirectionalLight( { 0.0f,0.0f,dt } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'S' ) )
				m_SceneManager.TranslateDirectionalLight( { 0.0f,0.0f,-dt } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'A' ) )
				m_SceneManager.TranslateDirectionalLight( { -dt,0.0f,0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'D' ) )
				m_SceneManager.TranslateDirectionalLight( { dt,0.0f,0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'R' ) )
				m_SceneManager.TranslateDirectionalLight( { 0.0f,dt,0.0f } );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( 'F' ) )
				m_SceneManager.TranslateDirectionalLight( { 0.0f,-dt,0.0f } );

			// directional light rotation
			if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_UP ) )
				m_SceneManager.RotateDirectionalLight( 0.0f, -dt * rotato );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_DOWN ) )
				m_SceneManager.RotateDirectionalLight( 0.0f, dt * rotato );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_RIGHT ) )
				m_SceneManager.RotateDirectionalLight( dt * rotato, 0.0f );
			if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_LEFT ) )
				m_SceneManager.RotateDirectionalLight( -dt * rotato, 0.0f );
		}
	}
	else if ( m_Win32Window.CursorEnabled() )
	{
		// render techniques
		if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_SHIFT ) && m_Win32Window.m_Kbd.KeyIsPressed( 'D' ) )
			m_SceneManager.SetRenderTechnique( RenderTechnique::Deferred );
		if ( m_Win32Window.m_Kbd.KeyIsPressed( VK_SHIFT ) && m_Win32Window.m_Kbd.KeyIsPressed( 'F' ) )
			m_SceneManager.SetRenderTechnique( RenderTechnique::Forward );
	}

	// raw mouse input
	/*while ( const auto delta = m_Win32Window.m_Mouse.ReadRawDelta() )
	{
		if ( !m_Win32Window.CursorEnabled() )
			m_SceneManager.RotateCamera( (f32)delta->x, (f32)delta->y );
	} */

	m_SceneManager.Present();
}
