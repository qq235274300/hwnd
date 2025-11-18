#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	parent(parent)
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique< VertextConstantBuffer<DirectX::XMMATRIX >>(gfx);
	}
}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	pVcbuf->Update(gfx,
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	
	pVcbuf->Bind(gfx);
}
std::unique_ptr<VertextConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::pVcbuf;