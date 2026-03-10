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
	void ShowModelWindow();
private:
	//最先构造
	ImguiManager imgui;
	
	const wchar_t* wndName = L"hwdDumb";
	Window wnd;
	ChiliTimer timer;
	Camera camera;
	PointLight light;
	float speed_factor = 1.0f;
	
	Model nano{ wnd.Gfx(),"Models\\nanosuit.obj" };
	struct 
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	}pos;
};