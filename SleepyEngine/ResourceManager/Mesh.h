#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "../Drawable/Drawable.h"

class Material;
class FrameCommander;
struct aiMesh;

class Mesh : public Drawable
{
public:
	Mesh( GraphicsDeviceInterface& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit( DirectX::FXMMATRIX accumulatedTranform ) const noexcept;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};