#pragma once
#include <unordered_map>
#include <string>
#include <Windows.h>
class WindowsMessageMap
{
public:
	WindowsMessageMap();
	std::string operator()(DWORD msg, LPARAM lParam, WPARAM wParam);
	std::unordered_map<DWORD, std::string> map;
};

