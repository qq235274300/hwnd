#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"


namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw(Graphics& gfx)const noxnd;
	virtual void Update(float dt) noexcept {}; // 改为非必须继承
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
protected:
	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
			{
				return pt;
			}
		}
		return nullptr;
	}
	void AddBind(std::unique_ptr<Bind::Bindable> bind)noxnd;
	void AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> ibuf)noxnd;
private:
	virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	std::vector<std::unique_ptr<Bind::Bindable>> binds;
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
};

