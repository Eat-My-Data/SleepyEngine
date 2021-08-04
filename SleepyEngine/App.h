#pragma once
#include "./Win32/Win32Window.h"
#include "./GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "./SceneManager/SceneManager.h"
#include "./GUI/GUIManager.h"
#include "./Utilities/FrameTimer.h"
#include "./Win32/Win32MessageMap.h"

class App
{
public:
	App();
	u32 Launch();
	~App();
private:
	void ExecuteFrame();
private:
	Win32Window m_Win32Window;
	GraphicsDeviceInterface m_GDI;
	SceneManager m_SceneManager;
	GUIManager m_GUIManager;
	FrameTimer timer;
	// TODO: 
	// - Raw Mouse Input
	// - Platform Interface
};