#include "App.h"
#include <algorithm>
#include "./Utilities/ChiliMath.h"
#include "./Libraries/imgui/imgui.h"
#include "./Utilities/ChiliUtil.h"
#include "./Utilities/Testing.h"
#include "./Utilities/PerfLog.h"
#include "./Renderer/Jobber/TestModelProbe.h"

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280, 720, "Sleepy Engine" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	light( wnd.Gfx() )
{
	D3DTestScratchPad( wnd );
	cube.SetPos( { 4.0f,0.0f,0.0f } );
	cube2.SetPos( { 0.0f,4.0f,0.0f } );
	nano.SetRootTransform(
		dx::XMMatrixRotationY( PI / 2.f ) *
		dx::XMMatrixTranslation( 27.f, -0.56f, 1.7f )
	);
	gobber.SetRootTransform(
		dx::XMMatrixRotationY( -PI / 2.f ) *
		dx::XMMatrixTranslation( -8.f, 10.f, 0.f )
	);

	cube.LinkTechniques( rg );
	cube2.LinkTechniques( rg );
	light.LinkTechniques( rg );
	sponza.LinkTechniques( rg );
	gobber.LinkTechniques( rg );
	nano.LinkTechniques( rg );

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ) );
}

void App::HandleInput( float dt )
{
	while ( const auto e = wnd.kbd.ReadKey() )
	{
		if ( !e->IsPress() )
		{
			continue;
		}

		switch ( e->GetCode() )
		{
		case VK_ESCAPE:
			if ( wnd.CursorEnabled() )
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			showDemoWindow = true;
			break;
		}
	}

	if ( !wnd.CursorEnabled() )
	{
		if ( wnd.kbd.KeyIsPressed( 'W' ) )
		{
			cam.Translate( { 0.0f,0.0f,dt } );
		}
		if ( wnd.kbd.KeyIsPressed( 'A' ) )
		{
			cam.Translate( { -dt,0.0f,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'S' ) )
		{
			cam.Translate( { 0.0f,0.0f,-dt } );
		}
		if ( wnd.kbd.KeyIsPressed( 'D' ) )
		{
			cam.Translate( { dt,0.0f,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'R' ) )
		{
			cam.Translate( { 0.0f,dt,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'F' ) )
		{
			cam.Translate( { 0.0f,-dt,0.0f } );
		}
	}

	while ( const auto delta = wnd.mouse.ReadRawDelta() )
	{
		if ( !wnd.CursorEnabled() )
		{
			cam.Rotate( (float)delta->x, (float)delta->y );
		}
	}
}

void App::ExecuteFrame( float dt )
{
	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	//light.Bind( wnd.Gfx(), cam.GetMatrix() );

	//light.Submit();
	cube.Submit();
	sponza.Submit();
	cube2.Submit();
	gobber.Submit();
	nano.Submit();

	rg.Execute( wnd.Gfx() );

	// imgui windows
	static MP sponzeProbe{ "Sponza" };
	static MP gobberProbe{ "Gobber" };
	static MP nanoProbe{ "Nano" };
	sponzeProbe.SpawnWindow( sponza );
	gobberProbe.SpawnWindow( gobber );
	nanoProbe.SpawnWindow( nano );
	cam.SpawnControlWindow();
	//light.SpawnControlWindow();
	cube.SpawnControlWindow( wnd.Gfx(), "Cube 1" );
	cube2.SpawnControlWindow( wnd.Gfx(), "Cube 2" );

	rg.RenderWidgets( wnd.Gfx() );

	// present
	wnd.Gfx().EndFrame();
	rg.Reset();
}

App::~App()
{}

u32 App::Launch()
{
	while ( true )
	{
		// process all messages pending, but to not block for new messages
		if ( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		// execute the game logic
		const auto dt = timer.Mark() * speed_factor;
		HandleInput( dt );
		ExecuteFrame( dt );
	}
}