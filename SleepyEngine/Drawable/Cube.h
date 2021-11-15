#pragma once
#include "Drawable.h"
#include "../ResourceManager/IndexedTriangleList.h"
#include "../ResourceManager/Vertex.h"

class Cube : public Drawable
{
private:
	struct Data
	{
		DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
	} m_Data;
public:
	Cube( GraphicsDeviceInterface& gdi, Data pos, f32 size = 1.0f );
	~Cube() = default;
	Cube( const Cube& cube );
	Cube& operator=( const Cube& cube );
public:
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	void SetRotation( f32 roll, f32 pitch, f32 yaw ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	const Data GetData() noexcept;
	void DrawControlPanel( const char* name );
private:
	IndexedTriangleList MakeIndependent( Dvtx::VertexLayout layout );
	IndexedTriangleList MakeIndependentTextured();
};