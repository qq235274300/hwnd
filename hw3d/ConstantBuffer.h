#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	//默认带固定数据
	ConstantBuffer(Graphics& gfx,const C& consts,UINT slot = 0u)
		:slot(slot)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// 创建后不需要CPU访问
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);//缓冲区总大小（字节数）。整个顶点数组的大小
		cbd.StructureByteStride = 0u;//不像VB PB需要填每个元素的大小
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;//实际的顶点数据 存储
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}
	//默认不带数据 后续通过Update上传
	ConstantBuffer(Graphics& gfx, UINT slot = 0u)
		:slot(slot)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// 创建后不需要CPU访问
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);//缓冲区总大小（字节数）。整个顶点数组的大小
		cbd.StructureByteStride = 0u;//不像VB PB需要填每个元素的大小
		//D3D11_SUBRESOURCE_DATA csd = {};
		//csd.pSysMem = &cb;//实际的顶点数据 存储
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd,nullptr, &pConstantBuffer));
	}
	
	void Update(Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);
		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetContext(gfx)->Map(pConstantBuffer.Get(), 0u, 
			D3D11_MAP_WRITE_DISCARD, 0u, &msr)); //映射方式（写 + 丢弃旧内容）
		memcpy(msr.pData, &consts, sizeof(consts));
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT slot;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	//引入父类的构造函数
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::slot;
	using Bindable::GetContext;
public:
	//引入父类的成员变量
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx)noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	//引入父类的成员变量
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::slot;
	using Bindable::GetContext;
public:
	//引入父类的构造函数
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx)noexcept override
	{
		GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
};