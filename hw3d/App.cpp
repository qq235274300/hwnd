#include "App.h"
#include "ChiliStringHelper.h"
#include <sstream>
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"


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

	
	nano.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (e->IsPress() && e->GetCode() == VK_INSERT)
		{
			if (wnd.GetCursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
		}
	}

	camera.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	nano.ShowWindow("Model");
	ShowRawInputWindow();

	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	static bool showdemowindow = true;
	if (showdemowindow)
	{
		ImGui::ShowDemoWindow(&showdemowindow);
	}
}

void App::ShowRawInputWindow()
{
	while (const auto d = wnd.mouse.ReadRawDelta())
	{
		x += d->x;
		y += d->y;
	}
	if (ImGui::Begin("Raw Input"))
	{
		ImGui::Text("Tally: (%d,%d)", x, y);
		ImGui::Text("Cursor: %s", wnd.GetCursorEnabled() ? "enabled" : "disabled");
	}
	ImGui::End();
}


