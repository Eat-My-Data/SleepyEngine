#pragma once
#include "Win32Window.h"
#include "D3D11Interface.h"

class App
{
public:
	App();
	u32 Launch();
	~App();
private:
	void ExecuteFrame();
private:
	// TODO: Platform Independence Layer
	Win32Window m_Win32Window;
	D3D11Interface m_D3D11Interface;
};