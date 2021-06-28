#include "App.h"

App::App()
	:
	m_Win32Window( 1280, 720, L"Sleepy Engine" )
{}

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
	// TODO: Setup rendering
}
