#include "Window.h"
#include "ChiliStringHelper.h"

int CALLBACK WinMain(
	 HINSTANCE hInstance,
	 HINSTANCE hPrevInstance,
	 LPSTR lpCmdLine,
	 int nShowCmd)
{
	try
	{
		const auto WndName = L"hwdDumb";
		Window wnd1(680, 480, WndName);

		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			/*if (wnd1.kbd.KeyIsPressed(VK_MENU))
			{
				MessageBox(nullptr, L"Something Happon!", L"ALT Key Was Pressed", MB_OK | MB_ICONEXCLAMATION);
			}*/
		}
		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;
	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, ChiliStringHelper::ToWide(e.What()).c_str(), ChiliStringHelper::ToWide(e.GetType()).c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, ChiliStringHelper::ToWide(e.what()).c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	
}