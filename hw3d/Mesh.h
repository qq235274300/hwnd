#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "ConditionalNoexcept.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelException : public ChiliException
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;
	const char* What() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};

class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	Node(int _id,const std::string& name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	void ShowTree(Node*& pSelectedNode)const noexcept;
	void SetAppliedTransform(DirectX::XMMATRIX transform);
	int GetId() const noexcept;
private:
	void AddChild(std::unique_ptr<Node> pChild) noxnd;
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
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
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh,const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId,const aiNode& node)noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	
	std::unique_ptr<class ModelWindow> pWindow;
};