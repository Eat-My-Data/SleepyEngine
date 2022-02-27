#pragma once
#include <unordered_map>
#include "WinDefines.h"
#include <Windows.h>

class Win32MessageMap
{
public:
	Win32MessageMap() noexcept;
	std::string operator()( DWORD msg, LPARAM lp, WPARAM wp ) const noexcept;
private:
	std::unordered_map<DWORD, std::string> map;
};