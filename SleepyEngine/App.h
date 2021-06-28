#pragma once
#include "Win32Window.h"
#include "GraphicsDeviceInterface.h"

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
	// - Scene Manager
	// - GraphicsDeviceInterface
	Win32Window m_Win32Window;
	GraphicsDeviceInterface m_GDI;
};