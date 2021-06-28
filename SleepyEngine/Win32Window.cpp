#include "Win32Window.h"
#include "D3D11Interface.h"
#include <exception>

Win32Window::WindowSingleton Win32Window::WindowSingleton::m_wSingleton;

Win32Window::WindowSingleton::WindowSingleton() noexcept
	:
	m_hInst( GetModuleHandle( nullptr ) )
{
	// register window class
	WNDCLASS wc = { 0 };
	//wc.cbSize = sizeof( wc );
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = (LPCWSTR)GetName();
	RegisterClass( &wc );
}

Win32Window::WindowSingleton::~WindowSingleton()
{
	UnregisterClass( (LPCWSTR)m_sWindowName, GetInstance() );
}

const wchar_t* Win32Window::WindowSingleton::GetName() noexcept
{
	return m_sWindowName;
}

HINSTANCE Win32Window::WindowSingleton::GetInstance() noexcept
{
	return m_wSingleton.m_hInst;
}

Win32Window::Win32Window( u32 width, u32 height, const wchar_t* name )
	:
	m_iWidth( width ),
	m_iHeight( height )
{
	// create window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if ( AdjustWindowRect( &wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE ) == 0 )
	{
		throw std::exception();
	}

	// create window and get hWnd
	m_hWnd = CreateWindow(
		WindowSingleton::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowSingleton::GetInstance(), this
	);
	// check for error
	if ( m_hWnd == nullptr )
	{
		throw std::exception();
	}

	// newly created windows start off as hidden
	ShowWindow( m_hWnd, SW_SHOWDEFAULT );

	// register mouse raw input device
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if ( RegisterRawInputDevices( &rid, 1, sizeof( rid ) ) == FALSE )
	{
		throw std::exception();
	}
}

Win32Window::~Win32Window()
{
	DestroyWindow( m_hWnd );
}

std::optional<u32> Win32Window::ProcessMessages() noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them ( but do not block on no messages )
	while ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
	{
		// check for quit because peekmessage does not signal the via return
		if ( msg.message == WM_QUIT )
		{
			// return optional wrapping int ( arg to PostQuitMessage is in wparam )
			return (int)msg.wParam;
		}

		// TranslateMessage will post auxillary WM_CHAR messages from key msgs
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// return empty optional when not quitting app
	return {};
}

void Win32Window::InitializeGraphics( D3D11Interface* graphicsInterface )
{
	graphicsInterface->Initialize( m_hWnd, m_iWidth, m_iHeight  );
}

LRESULT Win32Window::HandleMsgSetup( HWND m_hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer
	if ( msg == WM_NCCREATE )
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>( lParam );
		Win32Window* const pWnd = static_cast<Win32Window*>( pCreate->lpCreateParams );

		// set WinApi-managed user data to store ptr to window class
		SetWindowLongPtr( m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( pWnd ) );

		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr( m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( &Win32Window::HandleMsgThunk ) );

		// forward message to window class handler
		return pWnd->HandleMsg( m_hWnd, msg, wParam, lParam );
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler;
	return DefWindowProc( m_hWnd, msg, wParam, lParam );
}

LRESULT WINAPI Win32Window::HandleMsgThunk( HWND m_hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	// retrieve ptr to window class
	Win32Window* const pWnd = reinterpret_cast<Win32Window*>( GetWindowLongPtr( m_hWnd, GWLP_USERDATA ) );

	// forward message to window class handler
	return pWnd->HandleMsg( m_hWnd, msg, wParam, lParam );
}

LRESULT Win32Window::HandleMsg( HWND m_hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	switch ( msg )
	{
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
	}
	return DefWindowProc( m_hWnd, msg, wParam, lParam );
}