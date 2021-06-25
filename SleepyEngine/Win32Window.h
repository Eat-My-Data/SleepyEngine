#pragma once
#include <Windows.h>
#include <optional>
#include "NumericDataTypes.h"

class Win32Window
{
private:
	class WindowSingleton
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowSingleton() noexcept;
		~WindowSingleton();
		WindowSingleton( const WindowSingleton& ) = delete;
		WindowSingleton& operator=( const WindowSingleton& ) = delete;
		static constexpr const char* m_sWindowName = "Win32Window";
		static WindowSingleton m_wSingleton;
		HINSTANCE m_hInst;
	};
public:
	Win32Window( u32 width, u32 height, const char* name );
	~Win32Window();
	Win32Window( const Win32Window& ) = delete;
	Win32Window& operator=( const Win32Window& ) = delete;
	static std::optional<u32> ProcessMessages() noexcept;
private:
	static LRESULT CALLBACK HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;
	static LRESULT WINAPI HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;
	LRESULT HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;
private:
	HWND m_hWnd;
	u32 m_iWidth;
	u32 m_iHeight;
};