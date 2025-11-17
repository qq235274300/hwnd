#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& gfx,const std::vector<V>& vertices)
		:stride(sizeof(V))
	{
		//保证获得hr 或者 infoManager来进行打印消息
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT; //默认用法：GPU 读写、CPU 不直接访问
		bd.CPUAccessFlags = 0u;// 创建后不需要CPU访问
		bd.MiscFlags = 0u;
		bd.ByteWidth =UINT(sizeof(V) * vertices.size());//缓冲区总大小（字节数）。整个顶点数组的大小
		bd.StructureByteStride = sizeof(V);//每个元素的字节大小（结构体大小)

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();//实际的顶点数据 存储

		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer)); //创建GPU内存 pVertextBuffer指向它
	}
	void Bind(Graphics& gfx)noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

