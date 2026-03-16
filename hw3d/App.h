#pragma once
#include "Window.h"
#include <string>
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>
class App
{
public:
	App();
	~App();
	int Go();
private:
	void DoFrame();
	void ShowImguiDemoWindow();
private:
	//最先构造
	ImguiManager imgui;
	
	const wchar_t* wndName = L"hwdDumb";
	Window wnd;
	ChiliTimer timer;
	Camera camera;
	PointLight light;
	float speed_factor = 1.0f;
	
	Model nano{ wnd.Gfx(),"Models\\nano.gltf" };
	
};