#pragma once
#include "Window.h"
#include <string>
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
class App
{
public:
	App();
	~App();
	int Go();
private:
	void DoFrame();
private:
	//最先构造
	ImguiManager imgui;
	
	const wchar_t* wndName = L"hwdDumb";
	Window wnd;
	ChiliTimer timer;
	Camera camera;
	PointLight light;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
	float speed_factor = 1.0f;
};