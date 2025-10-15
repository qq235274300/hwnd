#include <Windows.h>
#include "WindowsMessageMap.h"
#include <sstream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageMap mm;
	std::string wndMessage = mm(msg, lParam, wParam).c_str();
	int len = MultiByteToWideChar(CP_UTF8, 0, wndMessage.c_str(), -1, nullptr, 0);
	std::wstring wMsg(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, wndMessage.c_str(), -1, &wMsg[0], len);
	OutputDebugString(wMsg.c_str());

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;

	case WM_KEYDOWN:
		if (wParam == 'F')
		{
			SetWindowText(hWnd,L"No Respect");
		}
		break;

	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowText(hWnd,L"Respect");
		}
		break;

	case WM_CHAR:
		{
			static std::wstring typeText;
			typeText.push_back((char)wParam);
			SetWindowText(hWnd, typeText.c_str());
			break;
		}	
	case WM_LBUTTONDOWN:
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream oss;
		oss << "(" <<pt.x << "," << pt.y << ")";
		int len = MultiByteToWideChar(CP_UTF8, 0, oss.str().c_str(), -1, nullptr, 0);
		std::wstring wMsg(len, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, oss.str().c_str(), -1, &wMsg[0], len);
		SetWindowText(hWnd, wMsg.c_str());
		break;
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

int CALLBACK WinMain(
	 HINSTANCE hInstance,
	 HINSTANCE hPrevInstance,
	 LPSTR lpCmdLine,
	 int nShowCmd)
{
	
	const auto WndClassName = L"hwdClass";
	const auto WndName = L"hwdDumb";

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hCursor = nullptr;
	wc.hIcon = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WndClassName;
	RegisterClassEx(&wc);

	HWND hWnd = CreateWindowEx(0,WndClassName, WndName, WS_SYSMENU |WS_CAPTION | WS_MINIMIZEBOX,200,200,640,480,nullptr,nullptr,hInstance,nullptr);
	
	ShowWindow(hWnd, SW_SHOW);

	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0) )> 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (gResult == -1)
	{
		return -1;
	}
	else
	{
		return msg.wParam;
	}
		
	

}