#pragma once
#include "Graphics.h"

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
private:
	//这三个都是相对于中心点 
	float r = 20.0f; //相机距离中心(世界)的距离
	float theta = 0.0f; //Y 轴（水平方向）旋转的角度（类似“横向环绕”）
	float phi = 0.0f; //X 轴（垂直方向）旋转的角度（类似“上下仰角”）
	//这三个相对于自身
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
};
