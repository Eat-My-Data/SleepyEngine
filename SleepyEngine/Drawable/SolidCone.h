#pragma once
#include "../Drawable/Drawable.h"

class SolidCone : public Drawable
{
public:
	SolidCone( GraphicsDeviceInterface& gdi );
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	void Rotate( f32 dx, f32 dy ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 rot = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};