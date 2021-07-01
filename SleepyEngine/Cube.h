#pragma once
#include "Drawable.h"
#include "IndexedTriangleList.h"
#include "Vertex.h"

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
private:
	IndexedTriangleList MakeIndependent( Dvtx::VertexLayout layout );
private:
	struct PSMaterialConstant
	{
		float specularIntensity = 0.1f;
		float specularPower = 20.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];
	} pmc;
};