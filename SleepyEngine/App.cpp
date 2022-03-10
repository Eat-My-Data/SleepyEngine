#include "App.h"
#include <algorithm>
#include "./Utilities/ChiliMath.h"
#include "./Libraries/imgui/imgui.h"
#include "./Utilities/ChiliUtil.h"
#include "./Utilities/Testing.h"
#include "./Utilities/PerfLog.h"
#include "./Renderer/Jobber/TestModelProbe.h"
#include "./Camera/Camera.h"
#include "./Renderer/Channels.h"

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280, 720, "Sleepy Engine" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	light( wnd.Gfx(), { 10.0f,5.0f,0.0f } )
{
	cameras.AddCamera( std::make_unique<Camera>( wnd.Gfx(), "A", dx::XMFLOAT3{ -13.5f,6.0f,3.5f }, 0.0f, PI / 2.0f ) );
	cameras.AddCamera( std::make_unique<Camera>( wnd.Gfx(), "B", dx::XMFLOAT3{ -13.5f,28.8f,-6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f ) );
	cameras.AddCamera( light.ShareCamera() );

	//cube.SetPos( { 10.0f,5.0f,6.0f } );
	//cube2.SetPos( { 10.0f,5.0f,14.0f } );
	nano.SetRootTransform(
		dx::XMMatrixRotationY( PI / 2.f ) *
		dx::XMMatrixTranslation( 27.f, -0.56f, 1.7f )
	);
	gobber.SetRootTransform(
		dx::XMMatrixRotationY( -PI / 2.f ) *
		dx::XMMatrixTranslation( -8.f, 10.f, 0.f )
	);


	//cube.LinkTechniques( rg );
	//cube2.LinkTechniques( rg );
	light.LinkTechniques( forward_rg );
	sponza.LinkTechniques( forward_rg );
	gobber.LinkTechniques( forward_rg );
	nano.LinkTechniques( forward_rg );

	cameras.LinkTechniques( forward_rg );
	forward_rg.BindShadowCamera( *light.ShareCamera() );

	//ToggleRenderTechnique();
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
		case VK_RETURN:
			savingDepth = true;
			break;
		case VK_F2:
			ToggleRenderTechnique();
			break;
		}
	}

	if ( !wnd.CursorEnabled() )
	{
		if ( wnd.kbd.KeyIsPressed( 'W' ) )
		{
			cameras->Translate( { 0.0f,0.0f,dt } );
		}
		if ( wnd.kbd.KeyIsPressed( 'A' ) )
		{
			cameras->Translate( { -dt,0.0f,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'S' ) )
		{
			cameras->Translate( { 0.0f,0.0f,-dt } );
		}
		if ( wnd.kbd.KeyIsPressed( 'D' ) )
		{
			cameras->Translate( { dt,0.0f,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'R' ) )
		{
			cameras->Translate( { 0.0f,dt,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'F' ) )
		{
			cameras->Translate( { 0.0f,-dt,0.0f } );
		}
	}

	while ( const auto delta = wnd.mouse.ReadRawDelta() )
	{
		if ( !wnd.CursorEnabled() )
		{
			cameras->Rotate( (float)delta->x, (float)delta->y );
		}
	}
}

void App::ToggleRenderTechnique()
{
	if ( isDeferred ) // Toggle Forward Shaders
	{
		//cube.LinkTechniques( rg );
		//cube2.LinkTechniques( rg );

		light.ToggleRenderTechnique( wnd.Gfx(), "" );
		sponza.ToggleRenderTechnique( wnd.Gfx(), "" );
		gobber.ToggleRenderTechnique( wnd.Gfx(), "" );
		nano.ToggleRenderTechnique( wnd.Gfx(), "" );

		light.LinkTechniques( forward_rg );
		sponza.LinkTechniques( forward_rg );
		gobber.LinkTechniques( forward_rg );
		nano.LinkTechniques( forward_rg );

		cameras.LinkTechniques( forward_rg );
		forward_rg.BindShadowCamera( *light.ShareCamera() );

		isDeferred = false;
		//cube.ToggleRenderTechnique( wnd.Gfx(), "" );
		//cube2.ToggleRenderTechnique( wnd.Gfx(), "" );
	}
	else // Toggle Deferred Shaders
	{
		//cube.LinkTechniques( rg );
		//cube2.LinkTechniques( rg );

		light.ToggleRenderTechnique( wnd.Gfx(), "Deferred" );
		sponza.ToggleRenderTechnique( wnd.Gfx(), "Deferred" );
		gobber.ToggleRenderTechnique( wnd.Gfx(), "Deferred" );
		nano.ToggleRenderTechnique( wnd.Gfx(), "Deferred" );

		light.LinkTechniques( deferred_rg );
		sponza.LinkTechniques( deferred_rg );
		gobber.LinkTechniques( deferred_rg );
		nano.LinkTechniques( deferred_rg );

		cameras.LinkTechniques( deferred_rg );
		deferred_rg.BindShadowCamera( *light.ShareCamera() );

		//cube.ToggleRenderTechnique( wnd.Gfx(), "Deferred" );
		//cube2.ToggleRenderTechnique( wnd.Gfx(), "Deferred" );
		isDeferred = true;
	}
}

void App::ExecuteFrame( float dt )
{
	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );	
	light.Bind( wnd.Gfx(), cameras->GetMatrix() );

	if ( isDeferred )
		deferred_rg.BindMainCamera( cameras.GetActiveCamera() );
	else
		forward_rg.BindMainCamera( cameras.GetActiveCamera() );

	light.Submit( Chan::main );
	//cube.Submit( Chan::main );
	sponza.Submit( Chan::main );
	//cube2.Submit( Chan::main );
	gobber.Submit( Chan::main );
	nano.Submit( Chan::main );
	cameras.Submit( Chan::main );

	//cube.Submit( Chan::shadow );
	sponza.Submit( Chan::shadow );
	//cube2.Submit( Chan::shadow );
	gobber.Submit( Chan::shadow );
	nano.Submit( Chan::shadow );

	if ( isDeferred )
		deferred_rg.Execute( wnd.Gfx() );
	else
		forward_rg.Execute( wnd.Gfx() );

	if ( savingDepth )
	{
		forward_rg.DumpShadowMap( wnd.Gfx(), "shadow.png" );
		savingDepth = false;
	}

	// imgui windows
	static MP sponzeProbe{ "Sponza" };
	static MP gobberProbe{ "Gobber" };
	static MP nanoProbe{ "Nano" };
	sponzeProbe.SpawnWindow( sponza );
	gobberProbe.SpawnWindow( gobber );
	nanoProbe.SpawnWindow( nano );
	cameras.SpawnWindow( wnd.Gfx() );
	light.SpawnControlWindow();
	//cube.SpawnControlWindow( wnd.Gfx(), "Cube 1" );
	//cube2.SpawnControlWindow( wnd.Gfx(), "Cube 2" );

	if ( isDeferred )
		deferred_rg.RenderWindows( wnd.Gfx() );
	else
		forward_rg.RenderWindows( wnd.Gfx() );

	// present
	wnd.Gfx().EndFrame();


	if ( isDeferred )
		deferred_rg.Reset();
	else
		forward_rg.Reset();
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