#pragma once
#include "Bindable.h"

//需要传入Blob 也就是VertextBuffer数据
class InputLayout : public Bindable
{ 
public:
	InputLayout(Graphics& gfx,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		ID3DBlob* pVertexShaderBytecode);

	void Bind(Graphics& gfx)noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};

