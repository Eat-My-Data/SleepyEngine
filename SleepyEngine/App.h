#pragma once
#include "./Win32/WinDefines.h"
#include "./Win32/Window.h"
#include "./Utilities/FrameTimer.h"
#include "./GUI/ImguiManager.h"
#include "./Camera/CameraContainer.h"
#include "./Drawable/PointLight.h"
#include "./Drawable/SpotLight.h"
#include "./Drawable/TestCube.h"
#include "./Renderer/Model/Model.h"
#include "./Utilities/ScriptCommander.h"
#include "./Renderer/Jobber/BlurOutlineRenderGraph.h"
#include "./Renderer/Jobber/DeferredRenderGraph.h"
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
	void ToggleRenderTechnique();
private:
	std::string commandLine;
	bool showDemoWindow = false;
	ImguiManager imgui;
	Window wnd;
	ScriptCommander scriptCommander;
	Rgph::BlurOutlineRenderGraph forward_rg{ wnd.Gfx() };
	Rgph::DeferredRenderGraph deferred_rg{ wnd.Gfx() };
	FrameTimer timer;
	float speed_factor = 1.0f;
	CameraContainer cameras;
	PointLight pointLight;
	SpotLight spotLight;
	TestCube cube{ wnd.Gfx(),4.0f };
	TestCube cube2{ wnd.Gfx(),4.0f };
	Model sponza{ wnd.Gfx(),"Models\\sponza\\sponza.obj", 1.0f / 20.0f};
	Model gobber{ wnd.Gfx(),"Models\\gobber\\GoblinX.obj", 4.0f};
	Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj", 2.0f};

	bool savingDepth = false;
	bool isDeferred = false;

	// TODO: 
	//	- Runtime rendergraph switching with a pointer to rendergraph?
};