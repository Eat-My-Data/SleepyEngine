#pragma once
#include "./Win32/WinDefines.h"
#include "./Win32/Window.h"
#include "./Utilities/FrameTimer.h"
#include "./GUI/ImguiManager.h"
#include "./Camera/CameraContainer.h"
#include "./Drawable/PointLight.h"
#include "./Drawable/TestCube.h"
#include "./Renderer/Model/Model.h"
#include "./Utilities/ScriptCommander.h"
#include "./Renderer/Jobber/BlurOutlineRenderGraph.h"
#include "./Utilities/ChiliMath.h"

class App
{
public:
	App( const std::string& commandLine = "" );
	u32 Launch();
	~App();
private:
	void ExecuteFrame( float dt );
	void HandleInput( float dt );
private:
	std::string commandLine;
	bool showDemoWindow = false;
	ImguiManager imgui;
	Window wnd;
	ScriptCommander scriptCommander;
	Rgph::BlurOutlineRenderGraph rg{ wnd.Gfx() };
	FrameTimer timer;
	float speed_factor = 1.0f;
	CameraContainer cameras;
	PointLight light;
	TestCube cube{ wnd.Gfx(),4.0f };
	TestCube cube2{ wnd.Gfx(),4.0f };
	Model sponza{ wnd.Gfx(),"Models\\sponza\\sponza.obj",1.0f / 20.0f };
	Model gobber{ wnd.Gfx(),"Models\\gobber\\GoblinX.obj",4.0f };
	Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj",2.0f };
	bool savingDepth = false;
};