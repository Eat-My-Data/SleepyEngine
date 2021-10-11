#pragma once
#include "Drawable.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../SceneManager/RenderTechnique.h"
#include "../SceneManager/Camera.h"
#include "../Drawable/SolidCone.h"

class SpotLight : public Drawable
{
public:
	SpotLight( GraphicsDeviceInterface& gdi, f32 scale );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Update( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos );
	void Draw( GraphicsDeviceInterface& gdi ) const noexcept;
public:
	void Translate( DirectX::XMFLOAT3 translation );
	void Rotate( const f32 dx, const f32 dy );
	DirectX::XMMATRIX GetViewMatrix() noexcept;
	DirectX::XMMATRIX GetProjectionMatrix() noexcept;
	Camera& GetCamera();
private:
	Camera m_PerspectiveCamera = { L"Spot Light Camera", MatrixType::Perspective, ViewSpace( 1.0f, 1.0f, 0.5f, 20.0f ), DirectX::XMFLOAT3{ 0.0f,10.0f,0.0f },  -PI / 2.0f, -PI };
public:
	struct SpotLightData
	{
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		float range = 20.0f;
		DirectX::XMFLOAT3 pos = { 0.0f, 10.0f, 0.0f };
		float cone = 5.0f;
		DirectX::XMMATRIX spotViewProjectionMatrix;
	};
	SpotLightData m_StructuredBufferData;
public:
	SolidCone* m_pSolidCone;
private:
	struct ForwardMatrices
	{
		DirectX::XMMATRIX lightViewMatrix;
		DirectX::XMMATRIX lightProjMatrix;
	} matrixcbuf;
	std::shared_ptr<Bind::VertexConstantBuffer<ForwardMatrices>> m_pForwardLightMatrices;
};
