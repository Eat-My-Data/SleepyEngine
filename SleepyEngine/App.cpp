#include "App.h"

App::App()
{}

int App::Launch()
{
	while ( true )
	{
		ExecuteFrame();
	}
}

App::~App()
{}

void App::ExecuteFrame()
{
	// TODO: Setup rendering code
}
