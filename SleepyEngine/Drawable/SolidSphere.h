#pragma once
#include "../Drawable/Drawable.h"

class SolidSphere : public Drawable
{
public:
	SolidSphere( GraphicsDeviceInterface& gdi, float radius );
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};