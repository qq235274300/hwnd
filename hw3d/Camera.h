#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera()noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy)noexcept;
	void Translate(DirectX::XMFLOAT3 translation)noexcept;
private: 
	//这三个都是相对于中心点 
	//float r = 20.0f; //相机距离中心(世界)的距离
	//float theta = 0.0f; //Y 轴（水平方向）旋转的角度（类似“横向环绕”）
	//float phi = 0.0f; //X 轴（垂直方向）旋转的角度（类似“上下仰角”）
	
	DirectX::XMFLOAT3 pos;
	//这三个相对于自身
	float pitch = 0.0f;
	float yaw = 0.0f;
	static constexpr float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
};
