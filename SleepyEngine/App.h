#pragma once
#include <Windows.h>
#include <exception>

class App
{
public:
	App();
	int Launch();
	~App();
private:
	void ExecuteFrame();
private:
	// TODO: Platform Independence Layer
};