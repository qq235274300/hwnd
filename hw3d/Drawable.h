#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw(Graphics& gfx)const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;
	
	void AddBind(std::unique_ptr<class Bindable> bind)noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf)noexcept;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
private:
	std::vector<std::unique_ptr<class Bindable>> binds;
	const class IndexBuffer* pIndexBuffer;
};

