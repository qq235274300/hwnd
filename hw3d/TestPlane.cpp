#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"

TestPlane::TestPlane(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto plane = Plane::Make();
	plane.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	std::string tag = "$plane" + std::to_string(size);
	
	AddBind(VertexBuffer::Resolve(gfx, tag, plane.vertices));
	AddBind(IndexBuffer::Resolve(gfx, tag, plane.indices));
	AddBind(Texture::Resolve(gfx,"Images\\brickwall.jpg",0u));
	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvso = pvs->GetBytecode();
	AddBind(std::move(pvs));
	AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));

	struct PSMaterialConstant
	{
		float specularIntensity = 0.8f;
		float specularPower = 45.0f;
		float padding[2];
	} pmc;
	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

	AddBind(InputLayout::Resolve(gfx, plane.vertices.GetLayout(), pvso));
	AddBind(Topology::Resolve(gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	 this->pos = pos;
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
