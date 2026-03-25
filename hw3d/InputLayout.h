#pragma once
#include "Bindable.h"
#include "Vertex.h"

//需要传入Blob 也就是VertextBuffer数据
namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx,
			Dvtx::VertexLayout layout,
			ID3DBlob* pVertexShaderBytecode);
		void Bind(Graphics& gfx)noexcept override;

		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
			const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode);
		static std::string GenerateUID(const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr);
		std::string GetUID() const noexcept override;
	protected:
		Dvtx::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
