#include "App.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow )
{
	try
	{
		return App{}.Launch();
	}
	catch ( std::exception& e )
	{
		MessageBox( nullptr, (LPCTSTR)e.what(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch ( ... )
	{
		MessageBox( nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION );
	}
	return -1;
}