#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh :public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> binds)
	{
		if (!IsStaticInitialized())
		{
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}
		for (auto& bind : binds)
		{
			if (auto pi = dynamic_cast<IndexBuffer*>(bind.get()))
			{
				AddIndexBuffer(std::unique_ptr<IndexBuffer>(pi));
				bind.release();
			}
			else
			{
				AddBind(std::move(bind));
			}
		}
		
		AddBind(std::make_unique<TransformCbuf>(gfx,*this));
	}

	void Draw(Graphics& gfx, DirectX::FXMMATRIX accmulatedTransform)const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&transform, accmulatedTransform);
		Drawable::Draw(gfx);
	}

	DirectX::XMMATRIX GetTransformXM()const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&transform);
	}

private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	//每一个Node 有一个或多个mesh  存在自己的transform 一个多个child
public:
	Node(std::vector<Mesh*> Meshs, DirectX::XMMATRIX& inTransform)noexcept(!IS_DEBUG)
		: meshPtrs(std::move(Meshs))
	{
		DirectX::XMStoreFloat4x4(&transform, inTransform);
	}

	void Draw(Graphics& gfx, DirectX::FXMMATRIX accmulatedTransform)const noexcept(!IS_DEBUG)
	{
		const auto build = DirectX::XMLoadFloat4x4(&transform) * accmulatedTransform;
		
		for (const auto mesh : meshPtrs)
		{
			mesh->Draw(gfx, build);
		}

		for (const auto& child : childPtrs)
		{
			child->Draw(gfx, build);
		}	
	}

private:
	void AddChild(std::unique_ptr<Node> child) noexcept(!IS_DEBUG)
	{
		assert(child);
		
		childPtrs.push_back(std::move(child));
	}

private:
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
};

class Model 
{
public:
	std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh)
	{
		
		hw3dexp::VertexBuffer vbf(
			hw3dexp::VertexLayout{}.Append(hw3dexp::VertexLayout::Position3D).Append(hw3dexp::VertexLayout::Normal)
		);
		
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbf.EmplaceBack(
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}
		
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		
		for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
		{
			const aiFace face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		
		std::vector<std::unique_ptr<Bindable>> bindablePtrs;
		bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx,vbf));
		bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));
		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbf.GetLayout().GetD3DLayout(), pvsbc));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		} pmc;
		bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}
	std::unique_ptr<Node> ParseNode(const aiNode& node)
	{
		
	}
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};

