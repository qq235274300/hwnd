#pragma once
#include "ChiliWin.h"
#include <string>

class ChiliStringHelper
{
public:
	static std::wstring ToWide(const std::string& str)
	{
		if (str.empty()) return L"";
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
		if (!wstr.empty() && wstr.back() == L'\0') wstr.pop_back(); // 去掉结尾的 '\0'
		return wstr;
	}

	// （可选）将 std::wstring 转换为 std::string
	static std::string ToNarrow(const std::wstring& wstr)
	{
		if (wstr.empty()) return "";
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string str(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
		if (!str.empty() && str.back() == '\0') str.pop_back();
		return str;
	}
};