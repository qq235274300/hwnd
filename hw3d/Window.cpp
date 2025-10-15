#include "Window.h"
#include "WindowsMessageMap.h"
#include <sstream>

Window::WindowClass Window::WindowClass::wndClass;

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

const wchar_t* Window::WindowClass::GetWndClassName() noexcept
{
	return WndClassName;
}

Window::WindowClass::WindowClass() noexcept
	:hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hCursor = nullptr;
	wc.hIcon = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WndClassName;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(GetWndClassName(), GetInstance());
}

Window::Window(int width, int height, const wchar_t* name)
{
	RECT rc;
	rc.left = 100;
	rc.right = rc.left + width; rc.top = 100; rc.bottom = rc.top + height;
	AdjustWindowRect(&rc, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, FALSE);
	
	hWnd = CreateWindowEx(0, WindowClass::GetWndClassName(), name, WS_SYSMENU |WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,width,height,nullptr,nullptr, WindowClass::GetInstance(),this);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE) // First time create window
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam); //�������͵�ָ��
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams); //ָ�뵽ָ��
		
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgThunk));
		
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageMap mm;
	std::string wndMessage = mm(msg, lParam, wParam).c_str();
	OutputDebugStringA(wndMessage.c_str());

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
