#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const std::string file, HRESULT hr);
		virtual const char* What()const noexcept override;
		virtual const char* GetType()const noexcept override;

		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT hr;
	};

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
	void SetTitle(const std::wstring& title);
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Keyboard kbd;
	Mouse mouse;
private:
	int height, width;
	HWND hWnd;
};

#define CHWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr)
#define CHWND_LAST_EXCEPT() Window::Exception(__LINE__,__FILE__,GetLastError())
