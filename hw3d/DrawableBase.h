#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"
#include <cassert>

template<class T>
class DrawableBase : public Drawable
{
public:
	bool IsStaticInitialized() const noexcept
	{
		return !binds.empty();
	}
	
	void AddStaticBind(std::unique_ptr<class Bindable> bind)noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf)noexcept
	{
		assert(pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	void SetIndexFromStatic()noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds)
		{
			//static_cast 不会做类型检查
			if (const auto  p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
			
		};
		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
	static std::vector<std::unique_ptr<class Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<class Bindable>> DrawableBase<T>::staticBinds;