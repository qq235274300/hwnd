#include "Mesh.h"
#include "Imgui/imgui.h"
#include <unordered_map>
#include <sstream>
#include "Surface.h"
// Mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
{

	AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	for (auto& pb : bindPtrs)
	{	
		AddBind(std::move(pb));		
	}

	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));
}
void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}


// Node
Node::Node(int _id,const std::string& name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd
	:
	name(name),
	id(_id),
	meshPtrs(std::move(meshPtrs))
{
	DirectX::XMStoreFloat4x4(&this->transform, transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}
void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto built = DirectX::XMLoadFloat4x4(&appliedTransform) * DirectX::XMLoadFloat4x4(&transform)* accumulatedTransform;
	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

void Node::ShowTree(Node*& pSelectedNode) const noexcept
{
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	//绘制一个节点 如果点击了就会展开 其实这里还没结构
	const auto nodeflag = ImGuiTreeNodeFlags_OpenOnArrow |
		((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
		| (childPtrs.empty() ?  ImGuiTreeNodeFlags_Leaf : 0);
	const auto expand = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), nodeflag, name.c_str());
	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}
	if (expand)
	{	
		for (const auto& child : childPtrs)
		{
			child->ShowTree(pSelectedNode);
		}		
		ImGui::TreePop();
	}
}

void Node::SetAppliedTransform(DirectX::XMMATRIX transform)
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

int Node::GetId() const noexcept
{
	return id;
}

void Node::AddChild(std::unique_ptr<Node> pChild) noxnd
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

class ModelWindow
{
public:
	void show(const char* windowName,const Node& pRoot)
	{
		windowName = windowName ? windowName : "Model";
		int nodeIndexTrack = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			pRoot.ShowTree(pSelectedNode);
			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				 auto& transform = transforms[pSelectedNode->GetId()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.f);
			}
			
		}
		ImGui::End();
	}

	DirectX::XMMATRIX getTransform()const noexcept
	{
		assert(pSelectedNode != nullptr);
		const auto& transform = transforms.at(pSelectedNode->GetId());
		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw)
			* DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	Node* GetSelectedNode()const noexcept
	{
		return pSelectedNode;
	}
private:
	struct transformPara
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, transformPara> transforms;
	Node* pSelectedNode;
};

// Model
Model::Model(Graphics& gfx, const std::string fileName):
	pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if (pScene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i],pScene->mMaterials));
	}
	int nextId = 0;
	pRoot = ParseNode(nextId ,*pScene->mRootNode);
}
Model::~Model() noexcept
{
}
void Model::ShowWindow(const char* windowName)
{
	pWindow->show(windowName, *pRoot);
}
void Model::Draw(Graphics& gfx) const noxnd
{
	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pWindow->getTransform());
	}
	
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	namespace dx = DirectX;
	using namespace Bind;
	using Dvtx::VertexLayout;

	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D)
	));
	
	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]) //第0套UV的第i个顶点UV
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	using namespace std::string_literals;
	const auto base = "Models\\nano_textured\\"s;

	bool hasSpecularMap = false;
	float shininess = 35.0f;
	//并非每个mesh都会有材质
	if (mesh.mMaterialIndex >= 0)
	{

		auto& meshMaterial = *pMaterials[mesh.mMaterialIndex];
		aiString nameOfMaterial;
		
		if (meshMaterial.GetTexture(aiTextureType_DIFFUSE, 0, &nameOfMaterial) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(Texture::Resolve(gfx,base + nameOfMaterial.C_Str()));
		}
		if (meshMaterial.GetTexture(aiTextureType_SPECULAR, 0, &nameOfMaterial) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(Texture::Resolve(gfx, base + nameOfMaterial.C_Str(),1u));
			hasSpecularMap = true;
		}
		else
		{
			meshMaterial.Get(AI_MATKEY_SHININESS, shininess);
		}
		
		bindablePtrs.push_back(Sampler::Resolve(gfx));
	}
	
	auto meshTag = base + "%" + mesh.mName.C_Str();
	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

	if (hasSpecularMap)
	{
		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpecMap.cso"));
	}
	else
	{
		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
		struct PSMaterialConstant
		{
			float specularIntensity = 1.8f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
	}

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}
std::unique_ptr<Node> Model::ParseNode(int& nextId,const aiNode& node)noexcept
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(),std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	ChiliException(line, file),
	note(std::move(note))
{}

const char* ModelException::What() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::What() << std::endl
		<< "[Note] " << GetNote();
	WhatBuffer = oss.str();
	return WhatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Chili Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}
