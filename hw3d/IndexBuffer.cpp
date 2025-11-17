#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices) :count(UINT(indices.size()))
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT; //默认用法：GPU 读写、CPU 不直接访问
	ibd.CPUAccessFlags = 0u;// 创建后不需要CPU访问
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);//缓冲区总大小（字节数）。整个顶点数组的大小
	ibd.StructureByteStride = sizeof(unsigned short);//每个元素的字节大小（结构体大小)

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();//实际的顶点数据 存储
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}
