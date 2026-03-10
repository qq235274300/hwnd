#include "App.h"
#include "ChiliStringHelper.h"
#include <sstream>
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "AssTest.h"

GDIPlusManager gdipm;

App::App(): wnd(1200, 900, wndName), light(wnd.Gfx())
{

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{
}

int App::Go()
{
	while (true)
	{
		if (const auto ecode = wnd.ProcessMessages()) {
			return *ecode;
		}
		DoFrame();	
	}	
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	/*const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
	wnd.SetTitle(ChiliStringHelper::ToWide(oss.str()));*/
	
	const float c = std::clamp(sin(dt), 0.0f, 1.0f);
	wnd.Gfx().BeginFrame(c, c, 1.0f);
	wnd.Gfx().SetCamera(camera.GetMatrix());
	light.Bind(wnd.Gfx(),camera.GetMatrix());

	const auto transform = DirectX::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	nano.Draw(wnd.Gfx(),transform);
	light.Draw(wnd.Gfx());
	
	camera.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();

	wnd.Gfx().EndFrame();
}

void App::ShowModelWindow()
{
	if (ImGui::Begin("Model"))
	{
		using namespace std::string_literals;

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
	}
	ImGui::End();
}


