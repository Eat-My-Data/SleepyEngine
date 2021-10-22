#pragma once
#include "Drawable.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../SceneManager/RenderTechnique.h"
#include "../SceneManager/Camera.h"

class DirectionalLight : public Drawable
{
public:
	DirectionalLight( GraphicsDeviceInterface& gdi );
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
	f32 m_fPitch = PI / 2.0f;
	f32 m_fYaw = -PI;
	Camera m_OrthoCamera = { L"Directional Light Camera", MatrixType::Orthographic, ViewSpace( 400.0f, 400.0f, 1.0f, 1200.0f ), DirectX::XMFLOAT3{ 0.0f,200.8f,100.0f },  PI / 2.0f, -PI };
public:
	struct DirectionalLightData
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float att = 0.7f;
		float padding;
		DirectX::XMMATRIX lightViewProjectionMatrix;
	};
	DirectionalLightData m_StructuredBufferData;
	DirectX::XMFLOAT3 homePos = { 10.0f, 90.0f, 2.5f };
private:
	struct ForwardMatrices
	{
		DirectX::XMMATRIX lightViewMatrix;
		DirectX::XMMATRIX lightProjMatrix;
	} matrixcbuf;
	std::shared_ptr<Bind::VertexConstantBuffer<ForwardMatrices>> m_pForwardLightMatrices;
};
