#pragma once
#include "ChiliWin.h"
class Window
{
private:
	class WindowClass
	{
	public:
		static HINSTANCE  GetInstance()noexcept;
		static const wchar_t* GetWndClassName()noexcept;
	private:
		WindowClass()noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		
		static WindowClass wndClass; //程序启动自动注册
		HINSTANCE hInst;
		static constexpr const wchar_t* WndClassName = L"Chili D3D11 WndClass";
	};

public:
	Window(int width,int height, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	int height, width;
	HWND hWnd;
};

