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

	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<class Bindable> bind) noxnd
{
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert("Binding multiple index buffer is not allowed" && (pIndexBuffer == nullptr));
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	binds.push_back(std::move(bind));
}

