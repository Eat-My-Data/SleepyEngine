#include "App.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow ){
	try
	{
		return App{ lpCmdLine }.Launch();
	}
	catch ( const ChiliException& e )
	{
		MessageBox( nullptr, (LPCWSTR)e.what(), (LPCWSTR)e.GetType(), MB_OK | MB_ICONEXCLAMATION );
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