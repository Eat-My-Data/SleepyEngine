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
public:
	f32 m_fPitch;
	f32 m_fYaw;
public:
	struct SpotLightData
	{
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		float range = 20.0f;
		DirectX::XMFLOAT3 pos = { 0.0f, 10.0f, 0.0f };
		float outerRadius = 5.0f;
		float innerRadius = 2.5f;
		float padding[3];
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
