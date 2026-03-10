#include "Drawable.h"
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>

using namespace Bind;

void Drawable::Draw(Graphics& gfx) const noxnd
{
	for (auto& bind : binds)
	{
		bind->Bind(gfx);
	}

	for (auto& staticBind : GetStaticBinds())
	{
		staticBind->Bind(gfx);
	}

	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<class Bindable> bind) noxnd
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noxnd
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();
	//binds.push_back(ibuf); unique_ptr ²»ÔÊÐí¿½±´
	binds.push_back(std::move(ibuf));
}
