#include "Window.h"
#include "WindowsMessageMap.h"
#include <sstream>
#include "WindowsThrowMacros.h"
#include "Imgui/imgui_impl_win32.h"

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
	:
	width(width),
	height(height)
{
	RECT rc;
	rc.left = 100;
	rc.right = rc.left + width; rc.top = 100; rc.bottom = rc.top + height;
	if (AdjustWindowRect(&rc, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, FALSE) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
	//WindowClass::GetWndClassName()
	hWnd = CreateWindowEx(0, WindowClass::GetWndClassName(), name, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,width,height,nullptr,nullptr, WindowClass::GetInstance(),this);

	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	//Init Imgui Win32 Impl
	ImGui_ImplWin32_Init(hWnd);

	pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::wstring& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return (int)msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE) // First time create window
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam); //其余类型到指针
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams); //指针到指针
		
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
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	static WindowsMessageMap mm;
	std::string wndMessage = mm(msg, lParam, wParam).c_str();
	//OutputDebugStringA(wndMessage.c_str());

	switch (msg)
	{
	//keyboard messages
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnable())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}	
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
	//Mouse
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		SetForegroundWindow(hWnd);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::HrException::HrException(int line, const std::string file, HRESULT hr) noexcept
	:Exception(line, file), hr(hr)
{
}

const char* Window::HrException::What() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	WhatBuffer = oss.str();
	return WhatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Chili Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	LPWSTR pMsgBuf = nullptr;
	const DWORD nMsgLen = ::FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		0,                           // 让系统自己挑语言
		(LPWSTR)&pMsgBuf,            // 注意：ALLOCATE_BUFFER 时是指针的地址
		0,
		nullptr);

	if (nMsgLen == 0 || !pMsgBuf) {
		return "Unidentified error code";
	}

	// 把系统返回的宽字符串拷贝出来，并去掉尾部的 \r\n
	std::wstring wmsg(pMsgBuf, nMsgLen);
	::LocalFree(pMsgBuf);
	while (!wmsg.empty() && (wmsg.back() == L'\r' || wmsg.back() == L'\n')) {
		wmsg.pop_back();
	}

	// 宽 -> UTF-8
	int bytes = ::WideCharToMultiByte(CP_UTF8, 0,
		wmsg.c_str(), (int)wmsg.size(),
		nullptr, 0, nullptr, nullptr);
	if (bytes <= 0) return "Unidentified error code";

	std::string utf8(bytes, '\0');
	::WideCharToMultiByte(CP_UTF8, 0,
		wmsg.c_str(), (int)wmsg.size(),
		utf8.data(), bytes, nullptr, nullptr);
	return utf8;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Chili Window Exception [No Graphics]";
}
