#pragma once
#include "Graphics.h"
#include "ConditionalNoexcept.h"
namespace Bind
{
	
class Bindable
{
public:
	~Bindable() = default;
	virtual void Bind(Graphics& gfx)noexcept = 0;
	virtual std::string GetUID()const noexcept
	{
		assert(false); //走到这里必崩 也就是这个函数必须被继承重写
		return "";
	}
protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx)noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx)noexcept;
	static DxgiInfoManager& GetInfoManager(Graphics& gfx);
	
};

}
