#pragma once
#include "Window.h"
#include <string>
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>
class App
{
public:
	App();
	~App();
	int Go();
private:
	void DoFrame();
	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
private:
	//最先构造
	ImguiManager imgui;
	
	const wchar_t* wndName = L"hwdDumb";
	Window wnd;
	ChiliTimer timer;
	Camera camera;
	PointLight light;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class Box*> boxes;
	std::optional<int> comboBoxIndex = 0;
	std::set<int> boxControlIds;

	static constexpr size_t nDrawables = 180;
	float speed_factor = 1.0f;
};