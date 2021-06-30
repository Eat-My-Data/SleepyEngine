#pragma once
#include "Drawable.h"

class Cube : public Drawable
{
	Cube( GraphicsDeviceInterface& gdi, float size );
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	void SetRotation( f32 roll, f32 pitch, f32 yaw ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};