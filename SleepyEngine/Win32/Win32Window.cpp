#include "Win32Window.h"
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
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

	/*// register mouse raw input device
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if ( RegisterRawInputDevices( &rid, 1, sizeof( rid ) ) == FALSE )
	{
		throw std::exception();
	} */
}

Win32Window::~Win32Window()
{
	DestroyWindow( m_hWnd );
}


void Win32Window::EnableCursor() noexcept
{
	m_bCursorEnabled = true;
	ShowCursor();
	FreeCursor();
}

void Win32Window::DisableCursor() noexcept
{
	m_bCursorEnabled = false;
	HideCursor();
	ConfineCursor();
}

bool Win32Window::CursorEnabled() noexcept
{
	return m_bCursorEnabled;
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

void Win32Window::InitializeGraphics( GraphicsDeviceInterface& graphicsInterface, GraphicsAPI api )
{
	graphicsInterface.InitializeGraphics( m_hWnd, api, m_iWidth, m_iHeight  );
}

void Win32Window::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect( m_hWnd, &rect );
	MapWindowPoints( m_hWnd, nullptr, reinterpret_cast<POINT*>( &rect ), 2 );
	ClipCursor( &rect );
}

void Win32Window::FreeCursor() noexcept
{
	ClipCursor( nullptr );
}

void Win32Window::HideCursor() noexcept
{
	while ( ::ShowCursor( FALSE ) >= 0 );
}

void Win32Window::ShowCursor() noexcept
{
	while ( ::ShowCursor( TRUE ) < 0 );
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
	case WM_KILLFOCUS:
		m_Kbd.ClearState();
		break;
	case WM_ACTIVATE:
		// confine/free cursor on window to foreground/background if cursor disabled
		if ( !m_bCursorEnabled )
		{
			if ( wParam & WA_ACTIVE )
			{
				ConfineCursor();
				HideCursor();
			}
			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
	case WM_KEYDOWN:
		// syskey commands need to be handled to track ALT key ( VK_MENU ) and F10
	case WM_SYSKEYDOWN:
		if ( !( lParam & 0x40000000 ) || m_Kbd.AutorepeatIsEnabled() )
		{
			m_Kbd.OnKeyPressed( static_cast<unsigned char>( wParam ) );
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		m_Kbd.OnKeyReleased( static_cast<unsigned char>( wParam ) );
		break;
	case WM_CHAR:
		m_Kbd.OnChar( static_cast<unsigned char>( wParam ) );
		break;
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		// cursorless exclusive gets first dibs
		if ( !m_bCursorEnabled )
		{
			if ( !m_Mouse.IsInWindow() )
			{
				SetCapture( m_hWnd );
				m_Mouse.OnMouseEnter();
				HideCursor();
			}
			break;
		}
		// in client region -> log move, and log enter + capture mouse ( if not previously stored )
		if ( pt.x >= 0 && pt.x < (SHORT)m_iWidth && pt.y >= 0 && pt.y < (SHORT)m_iHeight )
		{
			m_Mouse.OnMouseMove( pt.x, pt.y );
			if ( !m_Mouse.IsInWindow() )
			{
				SetCapture( m_hWnd );
				m_Mouse.OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if ( wParam & ( MK_LBUTTON | MK_RBUTTON ) )
			{
				m_Mouse.OnMouseMove( pt.x, pt.y );
			}
			// button up -> release capture / log event for leaving
			else
			{
				ReleaseCapture();
				m_Mouse.OnMouseLeave();
			}
		}
	}
	case WM_LBUTTONDOWN:
	{
		SetForegroundWindow( m_hWnd );
		if ( !m_bCursorEnabled )
		{
			ConfineCursor();
			HideCursor();
		}
		const POINTS pt = MAKEPOINTS( lParam );
		m_Mouse.OnLeftPressed( pt.x, pt.y );
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		m_Mouse.OnRightPressed( pt.x, pt.y );
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		m_Mouse.OnLeftReleased( pt.x, pt.y );
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		m_Mouse.OnRightReleased( pt.x, pt.y );
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		const int delta = GET_WHEEL_DELTA_WPARAM( wParam );
		m_Mouse.OnWheelDelta( pt.x, pt.y, delta );
		break;
	}
	case WM_INPUT:
	{
		if ( !m_Mouse.RawEnabled() )
			break;

		UINT size;
		// first get the size of the input data
		if ( GetRawInputData(
			reinterpret_cast<HRAWINPUT>( lParam ),
			RID_INPUT,
			nullptr,
			&size,
			sizeof( RAWINPUTHEADER ) ) == -1 )
		{
			// bail msg processing if error
			break;
		}
		m_RawMouseBuffer.resize( size );
		// read in the input data
		if ( GetRawInputData(
			reinterpret_cast<HRAWINPUT>( lParam ),
			RID_INPUT,
			m_RawMouseBuffer.data(),
			&size,
			sizeof( RAWINPUTHEADER ) ) != size )
		{
			// bail msg processing if error
			break;
		}
		// process the raw input data
		auto& ri = reinterpret_cast<const RAWINPUT&>( *m_RawMouseBuffer.data() );
		if ( ri.header.dwType == RIM_TYPEMOUSE &&
			( ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0 ) )
		{
			m_Mouse.OnRawDelta( ri.data.mouse.lLastX, ri.data.mouse.lLastY );
		}
		break;
	}
	}
	return DefWindowProc( m_hWnd, msg, wParam, lParam );
}