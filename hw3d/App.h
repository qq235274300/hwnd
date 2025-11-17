#pragma once
#include "Window.h"
#include <string>
#include "ChiliTimer.h"
class App
{
public:
	App();
	~App();
	int Go();
private:
	void DoFrame();
private:
	const wchar_t* wndName = L"hwdDumb";
	Window wnd;
	ChiliTimer timer;

	std::vector<std::unique_ptr<class Box>> boxes;
};