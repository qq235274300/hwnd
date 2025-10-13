#include <Windows.h>

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
	wc.lpfnWndProc = DefWindowProc;
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

	while (true)
	{
	}

	return 0;
}