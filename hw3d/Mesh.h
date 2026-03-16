#pragma once
#include "DrawableBase.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "ConditionalNoexcept.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
	friend class ModelWindow;
public:
	Node(const std::string& name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	void ShowTree(int& nodeIndex, std::optional<int>& selectedIndex,Node*& pSelectedNode)const noexcept;
	void SetAppliedTransform(DirectX::XMMATRIX transform);
private:
	void AddChild(std::unique_ptr<Node> pChild) noxnd;
private:
	std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 baseTransform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model(Graphics& gfx, const std::string fileName);
	~Model()noexcept;
	void Draw(Graphics& gfx) const noxnd;
	void ShowWindow(const char* windowName);
private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(const aiNode& node)noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	
	std::unique_ptr<class ModelWindow> pWindow;
};