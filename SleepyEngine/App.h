#pragma once
#include "Win32Window.h"
#include "GraphicsDeviceInterface.h"
#include "SceneManager.h"
#include "FrameTimer.h"

class App
{
public:
	App();
	u32 Launch();
	~App();
private:
	void ExecuteFrame();
private:
	// TODO: 
	// - Platform Interface
	Win32Window m_Win32Window;
	GraphicsDeviceInterface m_GDI;
	SceneManager m_SceneManager;
	FrameTimer timer;
};