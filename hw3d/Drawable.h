#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw(Graphics& gfx)const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
protected:
	void AddBind(std::unique_ptr<class Bindable> bind)noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf)noexcept(!IS_DEBUG);
private:
	virtual const std::vector<std::unique_ptr<class Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	std::vector<std::unique_ptr<class Bindable>> binds;
	const class IndexBuffer* pIndexBuffer;
};

