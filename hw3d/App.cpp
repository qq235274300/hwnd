#include "App.h"
#include "ChiliStringHelper.h"
#include <sstream>
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "SkinnedBox.h"
#include "Sheet.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

GDIPlusManager gdipm;

App::App(): wnd(1200, 900, wndName), light(wnd.Gfx())
{
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			return std::make_unique<Box>(
				gfx, rng, adist, ddist,
				odist, rdist, bdist
			);

			/*switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
				);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
				);
			case 3:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			case 4:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}*/
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		//std::uniform_int_distribution<int> latdist{ 5,20 };
		//std::uniform_int_distribution<int> longdist{ 10,40 };
		//std::uniform_int_distribution<int> typedist{ 0,4 };//生成随机值
	};

	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	//加载图片
	//const auto s = Surface::FromFile("Images\\kappa50.png");

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
	light.Bind(wnd.Gfx());

	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}
	light.Draw(wnd.Gfx());
	
	static char buffer[1024];
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 20.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::InputText("Butts", buffer, sizeof(buffer));
	}
	ImGui::End();

	
	camera.SpawnControlWindow();
	light.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}
